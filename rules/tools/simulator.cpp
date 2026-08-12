# Simple simulator to estimate house edge for some bets
#include "../src/RulesEngine.h"
#include <iostream>
#include <chrono>

int main() {
    madcraps::RulesEngine engine;
    engine.seed(12345);

    const int trials = 2000000;
    double net = 0.0;

    for (int i = 0; i < trials; ++i) {
        // simulate a single-roll Field bet of $1
        madcraps::Bet b{madcraps::BetType::Field, 1.0};
        auto r = engine.rollDice();
        auto payouts = engine.resolveBetsOnRoll(std::vector<madcraps::Bet>{b}, r, 0);
        net += payouts[0].net;
    }

    std::cout << "Field $1 bet after " << trials << " rolls net=" << net << " average=" << (net / trials) << "\n";
    return 0;
}
