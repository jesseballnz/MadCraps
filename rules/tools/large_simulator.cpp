#include "../src/RulesEngine.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace madcraps;

namespace {

struct BetSpec {
    std::string name;
    Bet bet;
    bool resolves_over_multiple_rolls;
};

struct SimulationResult {
    std::string name;
    int64_t trials;
    double average_net;
    double house_edge_pct;
};

double resolve_single_roll_bet(RulesEngine& engine, const Bet& bet) {
    const RollResult roll = engine.rollDice();
    return engine.resolveBetsOnRoll(std::vector<Bet>{bet}, roll, 0).front().net;
}

double simulate_until_resolved(RulesEngine& engine, const Bet& bet) {
    if (bet.type == BetType::PassLine || bet.type == BetType::DontPass) {
        int point = 0;
        while (true) {
            const RollResult roll = engine.rollDice();
            const double net = engine.resolveBetsOnRoll(std::vector<Bet>{bet}, roll, point).front().net;
            if (point == 0) {
                const int total = roll.total();
                if (total == 4 || total == 5 || total == 6 || total == 8 || total == 9 || total == 10) {
                    point = total;
                    continue;
                }
            }
            if (std::abs(net) > 1e-12) {
                return net;
            }
        }
    }

    if (bet.type == BetType::Come || bet.type == BetType::DontCome) {
        int target = 0;
        while (true) {
            const RollResult roll = engine.rollDice();
            Bet active_bet = bet;
            active_bet.target = target;
            const double net = engine.resolveBetsOnRoll(std::vector<Bet>{active_bet}, roll, 0).front().net;
            if (target == 0) {
                const int total = roll.total();
                if (total == 4 || total == 5 || total == 6 || total == 8 || total == 9 || total == 10) {
                    target = total;
                    continue;
                }
            }
            if (std::abs(net) > 1e-12) {
                return net;
            }
        }
    }

    while (true) {
        const RollResult roll = engine.rollDice();
        const double net = engine.resolveBetsOnRoll(std::vector<Bet>{bet}, roll, 0).front().net;
        if (std::abs(net) > 1e-12) {
            return net;
        }
    }
}

SimulationResult run_simulation(RulesEngine& engine, const BetSpec& spec, int64_t trials) {
    double total_net = 0.0;
    for (int64_t i = 0; i < trials; ++i) {
        total_net += spec.resolves_over_multiple_rolls
            ? simulate_until_resolved(engine, spec.bet)
            : resolve_single_roll_bet(engine, spec.bet);
    }

    const double average_net = total_net / static_cast<double>(trials);
    const double house_edge_pct = (-average_net / spec.bet.amount) * 100.0;
    return SimulationResult{spec.name, trials, average_net, house_edge_pct};
}

std::vector<BetSpec> build_bet_specs() {
    return {
        {"PassLine", Bet{BetType::PassLine, 1.0}, true},
        {"DontPass", Bet{BetType::DontPass, 1.0}, true},
        {"Come", Bet{BetType::Come, 1.0}, true},
        {"DontCome", Bet{BetType::DontCome, 1.0}, true},
        {"Field", Bet{BetType::Field, 1.0}, false},
        {"Any7", Bet{BetType::Any7, 1.0}, false},
        {"AnyCraps", Bet{BetType::AnyCraps, 1.0}, false},
        {"Horn", Bet{BetType::Horn, 4.0}, false},
        {"Yo", Bet{BetType::Yo, 1.0}, false},
        {"AceDeuce", Bet{BetType::AceDeuce, 1.0}, false},
        {"Big6", Bet{BetType::Big6, 1.0}, true},
        {"Big8", Bet{BetType::Big8, 1.0}, true},
        {"Place4", Bet{BetType::Place, 1.0, 4}, true},
        {"Place5", Bet{BetType::Place, 1.0, 5}, true},
        {"Place6", Bet{BetType::Place, 1.0, 6}, true},
        {"Place8", Bet{BetType::Place, 1.0, 8}, true},
        {"Place9", Bet{BetType::Place, 1.0, 9}, true},
        {"Place10", Bet{BetType::Place, 1.0, 10}, true},
        {"Buy4", Bet{BetType::Buy, 1.0, 4}, true},
        {"Buy5", Bet{BetType::Buy, 1.0, 5}, true},
        {"Buy6", Bet{BetType::Buy, 1.0, 6}, true},
        {"Buy8", Bet{BetType::Buy, 1.0, 8}, true},
        {"Buy9", Bet{BetType::Buy, 1.0, 9}, true},
        {"Buy10", Bet{BetType::Buy, 1.0, 10}, true},
        {"Lay4", Bet{BetType::Lay, 1.0, 4}, true},
        {"Lay5", Bet{BetType::Lay, 1.0, 5}, true},
        {"Lay6", Bet{BetType::Lay, 1.0, 6}, true},
        {"Lay8", Bet{BetType::Lay, 1.0, 8}, true},
        {"Lay9", Bet{BetType::Lay, 1.0, 9}, true},
        {"Lay10", Bet{BetType::Lay, 1.0, 10}, true},
        {"Hardway4", Bet{BetType::Hardway, 1.0, 4}, true},
        {"Hardway6", Bet{BetType::Hardway, 1.0, 6}, true},
        {"Hardway8", Bet{BetType::Hardway, 1.0, 8}, true},
        {"Hardway10", Bet{BetType::Hardway, 1.0, 10}, true},
        {"Odds4", Bet{BetType::Odds, 1.0, 4}, true},
        {"Odds5", Bet{BetType::Odds, 1.0, 5}, true},
        {"Odds6", Bet{BetType::Odds, 1.0, 6}, true},
        {"Odds8", Bet{BetType::Odds, 1.0, 8}, true},
        {"Odds9", Bet{BetType::Odds, 1.0, 9}, true},
        {"Odds10", Bet{BetType::Odds, 1.0, 10}, true},
    };
}

void write_csv(const std::vector<SimulationResult>& results) {
    std::ofstream csv("simulation_results.csv");
    csv << "bet,trials,avg_net_per_wager,house_edge_pct\n";
    csv << std::fixed << std::setprecision(8);
    for (const auto& result : results) {
        csv << result.name << ","
            << result.trials << ","
            << result.average_net << ","
            << result.house_edge_pct << "\n";
    }
}

void write_report(const std::vector<SimulationResult>& results, int64_t trials, double seconds) {
    std::ofstream md("simulation_report.md");
    md << "# Simulation Report\n\n";
    md << "Trials per bet: " << trials << "\n\n";
    md << "Elapsed time: " << std::fixed << std::setprecision(2) << seconds << " seconds\n\n";
    md << "House edge is reported as the expected player loss divided by the initial wager size.\n\n";
    md << "| Bet | Avg net / wager | House edge |\n";
    md << "| --- | ---: | ---: |\n";
    md << std::fixed << std::setprecision(6);
    for (const auto& result : results) {
        md << "| " << result.name
           << " | " << result.average_net
           << " | " << result.house_edge_pct << "% |\n";
    }
}

} // namespace

int main(int argc, char** argv) {
    int64_t trials = 1000000;
    if (argc > 1) {
        trials = std::stoll(argv[1]);
    }

    RulesEngine engine;
    engine.seed(123456789);

    auto results = build_bet_specs();
    std::vector<SimulationResult> output;
    output.reserve(results.size());

    const auto start = std::chrono::high_resolution_clock::now();
    for (const auto& spec : results) {
        output.push_back(run_simulation(engine, spec, trials));
    }
    const auto end = std::chrono::high_resolution_clock::now();
    const double seconds = std::chrono::duration<double>(end - start).count();

    std::sort(output.begin(), output.end(), [](const SimulationResult& lhs, const SimulationResult& rhs) {
        return lhs.house_edge_pct < rhs.house_edge_pct;
    });

    write_csv(output);
    write_report(output, trials, seconds);

    std::cout << "Simulation complete. Results written to simulation_results.csv and simulation_report.md\n";
    return 0;
}
