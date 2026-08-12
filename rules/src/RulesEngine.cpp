#include "RulesEngine.h"
#include <sstream>

namespace madcraps {

RulesEngine::RulesEngine() {
    std::random_device rd;
    rng_.seed(rd());
    // default payouts
    cfg_.odds_payout[4] = 2.0;
    cfg_.odds_payout[10] = 2.0;
    cfg_.odds_payout[5] = 1.5;
    cfg_.odds_payout[9] = 1.5;
    cfg_.odds_payout[6] = 1.2;
    cfg_.odds_payout[8] = 1.2;

    cfg_.place_payout[4] = 9.0/5.0; // 9:5 -> 1.8
    cfg_.place_payout[10] = 9.0/5.0;
    cfg_.place_payout[5] = 7.0/5.0; // 7:5 -> 1.4
    cfg_.place_payout[9] = 7.0/5.0;
    cfg_.place_payout[6] = 7.0/6.0; // 7:6 -> 1.1666667
    cfg_.place_payout[8] = 7.0/6.0;
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

std::vector<Payout> RulesEngine::resolveBetsOnRoll(const std::vector<Bet>& bets, const RollResult& roll, int point) {
    std::vector<Payout> payouts;
    int total = roll.total();

    for (const auto& b : bets) {
        Payout p{0.0, ""};
        switch (b.type) {
            case BetType::PassLine:
                if (point == 0) {
                    // come-out
                    if (total == 7 || total == 11) {
                        p.net = b.amount; p.description = "PassLine win (come-out)";
                    } else if (total == 2 || total == 3 || total == 12) {
                        p.net = -b.amount; p.description = "PassLine loss (come-out)";
                    } else {
                        p.net = 0.0; p.description = "PassLine point established";
                    }
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
            case BetType::Field:
                if (total == 2) { p.net = b.amount * cfg_.field_two_payout; p.description = "Field: 2"; }
                else if (total == 12) { p.net = b.amount * cfg_.field_twelve_payout; p.description = "Field: 12"; }
                else if (total == 3 || total == 4 || total == 9 || total == 10 || total == 11) { p.net = b.amount; p.description = "Field: other wins"; }
                else { p.net = -b.amount; p.description = "Field: lose"; }
                break;
            case BetType::Place4:
            case BetType::Place5:
            case BetType::Place6:
            case BetType::Place8:
            case BetType::Place9:
            case BetType::Place10:
            {
                int face = 0;
                if (b.type == BetType::Place4) face = 4;
                if (b.type == BetType::Place5) face = 5;
                if (b.type == BetType::Place6) face = 6;
                if (b.type == BetType::Place8) face = 8;
                if (b.type == BetType::Place9) face = 9;
                if (b.type == BetType::Place10) face = 10;

                if (total == face) {
                    double payoutRatio = 1.0;
                    auto it = cfg_.place_payout.find(face);
                    if (it != cfg_.place_payout.end()) payoutRatio = it->second;
                    p.net = b.amount * payoutRatio; p.description = "Place win";
                } else if (total == 7) { p.net = -b.amount; p.description = "Place lose (seven)"; }
                else { p.net = 0.0; p.description = "No resolution"; }
            }
            break;

            default:
                p.net = 0.0; p.description = "Unhandled bet type (prototype)";
        }
        payouts.push_back(p);
    }

    return payouts;
}

}
