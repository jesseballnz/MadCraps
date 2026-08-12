#include "RulesEngine.h"
#include <sstream>
#include <cmath>

namespace madcraps {

RulesEngine::RulesEngine() {
    std::random_device rd;
    rng_.seed(rd());
    // default odds
    cfg_.odds_payout[4] = 2.0;
    cfg_.odds_payout[10] = 2.0;
    cfg_.odds_payout[5] = 1.5;
    cfg_.odds_payout[9] = 1.5;
    cfg_.odds_payout[6] = 1.2;
    cfg_.odds_payout[8] = 1.2;

    // place payouts (win per unit)
    cfg_.place_payout[4] = 9.0/5.0; // 9:5 -> 1.8
    cfg_.place_payout[10] = 9.0/5.0;
    cfg_.place_payout[5] = 7.0/5.0; // 7:5 -> 1.4
    cfg_.place_payout[9] = 7.0/5.0;
    cfg_.place_payout[6] = 7.0/6.0; // 7:6 -> 1.1666667
    cfg_.place_payout[8] = 7.0/6.0;

    // buy payouts (when paying commission, buy wins roughly true odds minus commission handled separately)
    cfg_.buy_payout[4] = 2.0; // true odds
    cfg_.buy_payout[10] = 2.0;
    cfg_.buy_payout[5] = 1.5;
    cfg_.buy_payout[9] = 1.5;
    cfg_.buy_payout[6] = 1.2;
    cfg_.buy_payout[8] = 1.2;

    // lay payouts (player lays against a number; payouts are returned relative to risk)
    // For simplicity we store the payout multiplier for a win per unit risked (standard market values)
    cfg_.lay_payout[4] = 0.5; // example: lay 4 pays 1:2 (wins half the amount risked) -> this is illustrative
    cfg_.lay_payout[10] = 0.5;
    cfg_.lay_payout[5] = 2.0/3.0; // 3:2 -> 0.666...
    cfg_.lay_payout[9] = 2.0/3.0;
    cfg_.lay_payout[6] = 5.0/6.0; // 6:5 -> 0.8333
    cfg_.lay_payout[8] = 5.0/6.0;

    // hardway payouts
    cfg_.hardway_payout[4] = 7.0; // pays 7:1
    cfg_.hardway_payout[10] = 7.0;
    cfg_.hardway_payout[6] = 9.0; // pays 9:1
    cfg_.hardway_payout[8] = 9.0;

    // proposition payouts
    cfg_.prop_payouts["Any7"] = 4.0; // pays 4:1 (common pay table)
    cfg_.prop_payouts["AnyCraps"] = 7.0; // pays 7:1 (2,3,12)
    cfg_.prop_payouts["AceDeuce"] = 15.0; // 3 pays 15:1 (Yo) typical
}

void RulesEngine::setTableConfig(const TableConfig& cfg) {
    cfg_ = cfg;
}

void RulesEngine::seed(uint64_t s) {
    rng_.seed(static_cast<unsigned int>(s));
}

RollResult RulesEngine::rollDice() {
    std::uniform_int_distribution<int> die(1,6);
    RollResult r{die(rng_), die(rng_)};
    return r;
}

static bool isCraps(int total) { return (total == 2 || total == 3 || total == 12); }

std::vector<Payout> RulesEngine::resolveBetsOnRoll(const std::vector<Bet>& bets, const RollResult& roll, int point) {
    std::vector<Payout> payouts;
    int total = roll.total();

    for (const auto& b : bets) {
        Payout p{0.0, ""};
        switch (b.type) {
            case BetType::PassLine:
                if (point == 0) {
                    // come-out
                    if (total == 7 || total == 11) { p.net = b.amount; p.description = "PassLine win (come-out)"; }
                    else if (total == 2 || total == 3 || total == 12) { p.net = -b.amount; p.description = "PassLine loss (come-out)"; }
                    else { p.net = 0.0; p.description = "PassLine point established"; }
                } else {
                    if (total == point) { p.net = b.amount; p.description = "PassLine win (point made)"; }
                    else if (total == 7) { p.net = -b.amount; p.description = "PassLine loss (seven out)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::DontPass:
                if (point == 0) {
                    if (total == 2 || total == 3) { p.net = b.amount; p.description = "DontPass win (come-out)"; }
                    else if (total == 7 || total == 11) { p.net = -b.amount; p.description = "DontPass loss (come-out)"; }
                    else if (total == 12) { p.net = 0.0; p.description = "DontPass push (12)"; }
                    else { p.net = 0.0; p.description = "DontPass point established"; }
                } else {
                    if (total == 7) { p.net = b.amount; p.description = "DontPass win (seven out)"; }
                    else if (total == point) { p.net = -b.amount; p.description = "DontPass loss (point made)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Come:
                if (b.target == 0) {
                    if (total == 7 || total == 11) { p.net = b.amount; p.description = "Come win (come-out)"; }
                    else if (total == 2 || total == 3 || total == 12) { p.net = -b.amount; p.description = "Come loss (come-out)"; }
                    else { p.net = 0.0; p.description = "Come point established"; }
                } else {
                    if (total == b.target) { p.net = b.amount; p.description = "Come win (point made)"; }
                    else if (total == 7) { p.net = -b.amount; p.description = "Come loss (seven out)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::DontCome:
                if (b.target == 0) {
                    if (total == 2 || total == 3) { p.net = b.amount; p.description = "DontCome win (come-out)"; }
                    else if (total == 7 || total == 11) { p.net = -b.amount; p.description = "DontCome loss (come-out)"; }
                    else if (total == 12) { p.net = 0.0; p.description = "DontCome push (12)"; }
                    else { p.net = 0.0; p.description = "DontCome point established"; }
                } else {
                    if (total == 7) { p.net = b.amount; p.description = "DontCome win (seven out)"; }
                    else if (total == b.target) { p.net = -b.amount; p.description = "DontCome loss (point made)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Odds:
                // Odds bet attached to a specific point in Bet.target
                if (b.target <= 0) { p.net = 0.0; p.description = "Odds bet without point target"; }
                else {
                    if (total == b.target) { auto it = cfg_.odds_payout.find(b.target); double mult = (it!=cfg_.odds_payout.end())?it->second:0.0; p.net = b.amount * mult; p.description = "Odds win"; }
                    else if (total == 7) { p.net = -b.amount; p.description = "Odds loss (seven)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Field:
                if (total == 2) { p.net = b.amount * cfg_.field_two_payout; p.description = "Field: 2"; }
                else if (total == 12) { p.net = b.amount * cfg_.field_twelve_payout; p.description = "Field: 12"; }
                else if (total == 3 || total == 4 || total == 9 || total == 10 || total == 11) { p.net = b.amount; p.description = "Field: other wins"; }
                else { p.net = -b.amount; p.description = "Field: lose"; }
                break;
            case BetType::Place:
                if (b.target <= 0) { p.net = 0.0; p.description = "Place bet without target"; }
                else {
                    if (total == b.target) {
                        auto it = cfg_.place_payout.find(b.target);
                        double ratio = (it!=cfg_.place_payout.end())?it->second:1.0;
                        p.net = b.amount * ratio; p.description = "Place win";
                    } else if (total == 7) { p.net = -b.amount; p.description = "Place lose (seven)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Buy:
                if (b.target <= 0) { p.net = 0.0; p.description = "Buy bet without target"; }
                else {
                    if (total == b.target) {
                        auto it = cfg_.buy_payout.find(b.target);
                        double mult = (it!=cfg_.buy_payout.end())?it->second:1.0;
                        double win = b.amount * mult;
                        // commission applied on win by default (some casinos take commission on bet amount)
                        double commission = win * cfg_.buy_commission_pct;
                        p.net = win - commission; p.description = "Buy win (commission applied)";
                    } else if (total == 7) { p.net = -b.amount; p.description = "Buy lose (seven)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Lay:
                if (b.target <= 0) { p.net = 0.0; p.description = "Lay bet without target"; }
                else {
                    if (total == 7) {
                        // player wins when a 7 is rolled before the target; payout depends on lay_payout (win per unit risk)
                        auto it = cfg_.lay_payout.find(b.target);
                        double winRatio = (it!=cfg_.lay_payout.end())?it->second:1.0;
                        p.net = b.amount * winRatio; p.description = "Lay win (seven out)";
                    } else if (total == b.target) { p.net = -b.amount; p.description = "Lay loss (point made)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Hardway:
                if (b.target <= 0) { p.net = 0.0; p.description = "Hardway without target"; }
                else {
                    if (roll.isHardway(b.target)) {
                        auto it = cfg_.hardway_payout.find(b.target);
                        double mult = (it!=cfg_.hardway_payout.end())?it->second:1.0;
                        p.net = b.amount * mult; p.description = "Hardway win";
                    } else if (total == 7 || (!roll.isHardway(b.target) && total == b.target)) { p.net = -b.amount; p.description = "Hardway lost (easy or seven)"; }
                    else { p.net = 0.0; p.description = "No resolution"; }
                }
                break;
            case BetType::Big6:
                if (total == 6) { p.net = b.amount; p.description = "Big 6 win (pays 1:1)"; }
                else if (total == 7) { p.net = -b.amount; p.description = "Big 6 lose (seven)"; }
                else { p.net = 0.0; p.description = "No resolution"; }
                break;
            case BetType::Big8:
                if (total == 8) { p.net = b.amount; p.description = "Big 8 win (pays 1:1)"; }
                else if (total == 7) { p.net = -b.amount; p.description = "Big 8 lose (seven)"; }
                else { p.net = 0.0; p.description = "No resolution"; }
                break;
            case BetType::Any7:
                if (total == 7) {
                    auto it = cfg_.prop_payouts.find("Any7"); double mult = (it!=cfg_.prop_payouts.end())?it->second:4.0;
                    p.net = b.amount * mult; p.description = "Any 7 wins";
                } else { p.net = -b.amount; p.description = "Any 7 loses"; }
                break;
            case BetType::AnyCraps:
                if (isCraps(total)) { auto it = cfg_.prop_payouts.find("AnyCraps"); double mult = (it!=cfg_.prop_payouts.end())?it->second:7.0; p.net = b.amount * mult; p.description = "Any Craps wins"; }
                else { p.net = -b.amount; p.description = "Any Craps loses"; }
                break;
            case BetType::Horn:
                if (total == 2 || total == 12) {
                    auto it = cfg_.prop_payouts.find("Horn"); double mult = (it!=cfg_.prop_payouts.end())?it->second:30.0; // common 30:1 on 2/12
                    p.net = b.amount * mult; p.description = "Horn wins (2 or 12)";
                } else if (total == 3 || total == 11) {
                    auto it = cfg_.prop_payouts.find("Horn"); double mult = (it!=cfg_.prop_payouts.end())?it->second:15.0; // 15:1 on 3/11
                    p.net = b.amount * mult; p.description = "Horn wins (3 or 11)";
                } else {
                    p.net = -b.amount; p.description = "Horn loses";
                }
                break;
            default:
                p.net = 0.0; p.description = "Unhandled bet type";
        }
        payouts.push_back(p);
    }

    return payouts;
}

}
