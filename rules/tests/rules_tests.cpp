#include "../src/RulesEngine.h"
#include <iostream>
#include <cassert>

int main() {
    madcraps::RulesEngine engine;
    madcraps::TableConfig cfg;
    cfg.field_two_payout = 2.0;
    cfg.field_twelve_payout = 3.0;
    cfg.place_payout[6] = 7.0 / 6.0;
    engine.setTableConfig(cfg);

    // deterministic seed for tests
    engine.seed(1);

    // Test Pass Line come-out win on 7
    {
        madcraps::Bet b{madcraps::BetType::PassLine, 10.0};
        madcraps::RollResult r{3,4}; // 7
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts.size() == 1);
        assert(payouts[0].net == 10.0);
        std::cout << "PassLine come-out 7 test ok\n";
    }

    // Test DontPass push on 12
    {
        madcraps::Bet b{madcraps::BetType::DontPass, 5.0};
        madcraps::RollResult r{6,6}; // 12
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 0.0);
        std::cout << "DontPass 12 push test ok\n";
    }

    // Test Field 2 pays 2:1
    {
        madcraps::Bet b{madcraps::BetType::Field, 2.0};
        madcraps::RollResult r{1,1}; // 2
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        assert(payouts[0].net == 2.0 * 2.0);
        std::cout << "Field 2 pays test ok\n";
    }

    // Test Place 6 payout
    {
        madcraps::Bet b{madcraps::BetType::Place, 6.0, 6};
        madcraps::RollResult r{3,3}; // 6
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        // We expect payout 7:6 on a 6 -> 7/6 * 6 = 7
        assert(payouts[0].net == 7.0);
        std::cout << "Place 6 pays test ok\n";
    }

    std::cout << "All tests passed.\n";
    return 0;
}
