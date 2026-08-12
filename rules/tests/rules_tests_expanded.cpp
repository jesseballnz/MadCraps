// Expanded tests for the Rules Engine covering more bet types
#include "../src/RulesEngine.h"
#include <iostream>
#include <cassert>

int main() {
    madcraps::RulesEngine engine;
    madcraps::TableConfig cfg;
    cfg.field_two_payout = 2.0;
    cfg.field_twelve_payout = 3.0;
    cfg.odds_payout[4] = 2.0;
    cfg.hardway_payout[4] = 7.0;
    cfg.hardway_payout[6] = 9.0;
    cfg.hardway_payout[8] = 9.0;
    cfg.hardway_payout[10] = 7.0;
    cfg.prop_payouts["Any7"] = 4.0;
    cfg.prop_payouts["AnyCraps"] = 7.0;
    cfg.prop_payouts["AceDeuce"] = 15.0;
    cfg.prop_payouts["Yo"] = 15.0;
    cfg.prop_payouts["Horn2or12"] = 30.0;
    cfg.prop_payouts["Horn3or11"] = 15.0;
    engine.setTableConfig(cfg);

    // deterministic seed for tests
    engine.seed(1);

    // PassLine come-out win on 11
    {
        madcraps::Bet b{madcraps::BetType::PassLine, 10.0};
        madcraps::RollResult r{5,6}; // 11
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts.size() == 1);
        assert(payouts[0].net == 10.0);
        std::cout << "PassLine come-out 11 test ok\n";
    }

    // Come bet with target 5 wins when 5 rolled
    {
        madcraps::Bet b{madcraps::BetType::Come, 5.0, 5};
        madcraps::RollResult r{2,3}; // 5
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 4);
        assert(payouts[0].net == 5.0);
        std::cout << "Come 5 wins test ok\n";
    }

    // Fresh Come and established Come resolve independently on the same roll
    {
        std::vector<madcraps::Bet> bets{
            madcraps::Bet{madcraps::BetType::Come, 5.0},
            madcraps::Bet{madcraps::BetType::Come, 10.0, 5}
        };
        madcraps::RollResult r{2,3}; // 5
        auto payouts = engine.resolveBetsOnRoll(bets, r, 8);
        assert(payouts.size() == 2);
        assert(payouts[0].net == 0.0);
        assert(payouts[0].description == "Come point established");
        assert(payouts[1].net == 10.0);
        std::cout << "Come bets keep independent points test ok\n";
    }

    // Fresh Don't Come and established Don't Come resolve independently on the same roll
    {
        std::vector<madcraps::Bet> bets{
            madcraps::Bet{madcraps::BetType::DontCome, 5.0},
            madcraps::Bet{madcraps::BetType::DontCome, 10.0, 4}
        };
        madcraps::RollResult r{2,2}; // 4
        auto payouts = engine.resolveBetsOnRoll(bets, r, 6);
        assert(payouts.size() == 2);
        assert(payouts[0].net == 0.0);
        assert(payouts[0].description == "DontCome point established");
        assert(payouts[1].net == -10.0);
        std::cout << "DontCome bets keep independent points test ok\n";
    }

    // Fresh Come / Don't Come should not resolve on a table come-out roll
    {
        std::vector<madcraps::Bet> bets{
            madcraps::Bet{madcraps::BetType::Come, 5.0},
            madcraps::Bet{madcraps::BetType::DontCome, 5.0}
        };
        madcraps::RollResult r{4,3}; // 7
        auto payouts = engine.resolveBetsOnRoll(bets, r, 0);
        assert(payouts.size() == 2);
        assert(payouts[0].net == 0.0);
        assert(payouts[0].description == "Come inactive on table come-out");
        assert(payouts[1].net == 0.0);
        assert(payouts[1].description == "DontCome inactive on table come-out");
        std::cout << "Come bets inactive on table come-out test ok\n";
    }

    // Odds bet on point 4 wins 2:1
    {
        madcraps::Bet b{madcraps::BetType::Odds, 20.0, 4};
        madcraps::RollResult r{2,2}; // 4
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 4);
        assert(payouts[0].net == 40.0); // 20 * 2.0
        std::cout << "Odds on 4 pays 2:1 test ok\n";
    }

    // Hardway 6 wins on double 3s
    {
        madcraps::Bet b{madcraps::BetType::Hardway, 2.0, 6};
        madcraps::RollResult r{3,3}; // hard 6
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        // payout 9:1 -> 2 * 9 = 18
        assert(payouts[0].net == 18.0);
        std::cout << "Hardway 6 wins test ok\n";
    }

    // Any7 prop wins
    {
        madcraps::Bet b{madcraps::BetType::Any7, 1.0};
        madcraps::RollResult r{4,3}; // 7
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 4.0);
        std::cout << "Any7 prop pays test ok\n";
    }

    // Horn is four equal bets: on a $4 horn, 3/11 nets +12
    {
        madcraps::Bet b{madcraps::BetType::Horn, 4.0};
        madcraps::RollResult r{1,2}; // 3
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 12.0);
        std::cout << "Horn 3 payout test ok\n";
    }

    // Yo prop wins on 11
    {
        madcraps::Bet b{madcraps::BetType::Yo, 1.0};
        madcraps::RollResult r{5,6}; // 11
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 15.0);
        std::cout << "Yo prop pays test ok\n";
    }

    // Ace-Deuce prop wins on 3
    {
        madcraps::Bet b{madcraps::BetType::AceDeuce, 1.0};
        madcraps::RollResult r{1,2}; // 3
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 15.0);
        std::cout << "Ace-Deuce prop pays test ok\n";
    }

    // AnyCraps loses when 8
    {
        madcraps::Bet b{madcraps::BetType::AnyCraps, 1.0};
        madcraps::RollResult r{4,4}; // 8
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == -1.0);
        std::cout << "AnyCraps lose test ok\n";
    }

    // Horn pays as four equal bets with Vegas 30:1 on 2/12
    {
        madcraps::Bet b{madcraps::BetType::Horn, 4.0};
        madcraps::RollResult r{1,1}; // 2
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 27.0); // 1 * 30 - 3 losing horn units
        std::cout << "Horn 2 payout test ok\n";
    }

    // Horn pays 15:1 on 3/11 after subtracting the three losing horn units
    {
        madcraps::Bet b{madcraps::BetType::Horn, 4.0};
        madcraps::RollResult r{1,2}; // 3
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 12.0); // 1 * 15 - 3 losing horn units
        std::cout << "Horn 3 payout test ok\n";
    }

    // Yo prop wins on 11 at 15:1
    {
        madcraps::Bet b{madcraps::BetType::Yo, 2.0};
        madcraps::RollResult r{5,6}; // 11
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 30.0);
        std::cout << "Yo payout test ok\n";
    }

    // Ace-Deuce prop wins on 3 at 15:1
    {
        madcraps::Bet b{madcraps::BetType::AceDeuce, 2.0};
        madcraps::RollResult r{1,2}; // 3
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 30.0);
        std::cout << "Ace-Deuce payout test ok\n";
    }

    std::cout << "Expanded tests passed.\n";
    return 0;
}
