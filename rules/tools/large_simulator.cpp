#include "../src/RulesEngine.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <chrono>

using namespace madcraps;

double simulate_pass_line(RulesEngine &engine, int trials) {
 double net = 0.0;
 for (int i = 0; i < trials; ++i) {
 int point = 0;
 // Place $1 Pass Line
 while (true) {
 RollResult r = engine.rollDice();
 int total = r.total();
 if (point == 0) {
 if (total == 7 || total == 11) { net += 1.0; break; }
 else if (total == 2 || total == 3 || total == 12) { net -= 1.0; break; }
 else { point = total; }
 } else {
 if (total == point) { net += 1.0; break; }
 else if (total == 7) { net -= 1.0; break; }
 // else continue rolling
 }
 }
 }
 return net / trials; // average net per $1 bet
}

int main(int argc, char** argv) {
 int trials = 1000000; // default 1M trials
 if (argc > 1) trials = std::stoi(argv[1]);

 RulesEngine engine;
 engine.seed(123456789);

 std::cout << "Running large simulation (trials=" << trials << ")\n";

 std::unordered_map<std::string, double> results;

 auto start = std::chrono::high_resolution_clock::now();

 // 1) Pass Line (full round)
 results["PassLine"] = simulate_pass_line(engine, trials);

 // 2) Single-roll bets measured per roll
 // Field $1
 double net_field = 0.0;
 double net_any7 = 0.0;
 double net_anycraps = 0.0;
 double net_place6 = 0.0; // place to win on single roll
 double net_hard6 = 0.0;

 for (int i = 0; i < trials; ++i) {
 RollResult r = engine.rollDice();
 auto payouts_field = engine.resolveBetsOnRoll(std::vector<Bet>{Bet{BetType::Field,1.0}}, r, 0);
 net_field += payouts_field[0].net;

 auto payouts_any7 = engine.resolveBetsOnRoll(std::vector<Bet>{Bet{BetType::Any7,1.0}}, r, 0);
 net_any7 += payouts_any7[0].net;

 auto payouts_anycraps = engine.resolveBetsOnRoll(std::vector<Bet>{Bet{BetType::AnyCraps,1.0}}, r, 0);
 net_anycraps += payouts_anycraps[0].net;

 auto payouts_place6 = engine.resolveBetsOnRoll(std::vector<Bet>{Bet{BetType::Place,1.0,6}}, r, 0);
 net_place6 += payouts_place6[0].net;

 auto payouts_hard6 = engine.resolveBetsOnRoll(std::vector<Bet>{Bet{BetType::Hardway,1.0,6}}, r, 0);
 net_hard6 += payouts_hard6[0].net;
 }

 results["Field"] = net_field / trials;
 results["Any7"] = net_any7 / trials;
 results["AnyCraps"] = net_anycraps / trials;
 results["Place6"] = net_place6 / trials;
 results["Hardway6"] = net_hard6 / trials;

 auto end = std::chrono::high_resolution_clock::now();
 double seconds = std::chrono::duration<double>(end - start).count();

 // Write CSV and summary
 std::ofstream csv("simulation_results.csv");
 csv << "bet,trials,avg_net_per_bet" << std::endl;
 for (auto &kv : results) {
 csv << kv.first << "," << trials << "," << kv.second << std::endl;
 }
 csv.close();

 std::ofstream md("simulation_report.md");
 md << "# Simulation Report\n\n";
 md << "Trials: " << trials << "\n\n";
 md << "Elapsed time: " << seconds << " seconds\n\n";
 md << "Average net per $1 bet (positive = player wins; negative = house edge)\n\n";
 for (auto &kv : results) {
 md << "- " << kv.first << ": " << kv.second << "\n";
 }
 md.close();

 std::cout << "Simulation complete. Results written to simulation_results.csv and simulation_report.md\n";
 return 0;
}
