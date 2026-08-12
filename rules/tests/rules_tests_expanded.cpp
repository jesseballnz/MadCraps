# Expanded tests for the Rules Engine covering more bet types
#include "../src/RulesEngine.h"
#include <iostream>
#include <cassert>

int main() {
    madcraps::RulesEngine engine;
    madcraps::TableConfig cfg;
    cfg.field_two_payout = 2.0;
    cfg.field_twelve_payout = 3.0;
    cfg.hardway_payout[4] = 7.0;
    cfg.hardway_payout[6] = 9.0;
    cfg.hardway_payout[8] = 9.0;
    cfg.hardway_payout[10] = 7.0;
    cfg.prop_payouts["Any7"] = 4.0;
    cfg.prop_payouts["AnyCraps"] = 7.0;
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

    // AnyCraps loses when 8
    {
        madcraps::Bet b{madcraps::BetType::AnyCraps, 1.0};
        madcraps::RollResult r{4,4}; // 8
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == -1.0);
        std::cout << "AnyCraps lose test ok\n";
    }

    std::cout << "Expanded tests passed.\n";
    return 0;
}
