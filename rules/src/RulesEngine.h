#ifndef MADCRAPS_RULES_ENGINE_H
#define MADCRAPS_RULES_ENGINE_H

#include <cstdint>
#include <vector>
#include <map>
#include <random>
#include <string>

namespace madcraps {

enum class BetType {
    PassLine,
    DontPass,
    Field,
    Place4,
    Place5,
    Place6,
    Place8,
    Place9,
    Place10,
    // ...additional bet types will be added
};

struct Bet {
    BetType type;
    double amount;
};

struct TableConfig {
    double field_two_payout = 2.0; // 2:1
    double field_twelve_payout = 3.0; // 3:1 (Vegas default)
    std::map<int,double> odds_payout; // payout multiplier for odds (e.g., 4 -> 2.0)
    std::map<int,double> place_payout; // payout ratio for place bets (win per unit)
    double buy_commission_pct = 0.05;
};

struct RollResult {
    int die1;
    int die2;
    int total() const { return die1 + die2; }
};

struct Payout {
    double net; // player net change (positive = player wins)
    std::string description;
};

class RulesEngine {
public:
    RulesEngine();
    void setTableConfig(const TableConfig& cfg);
    void seed(uint64_t s);
    RollResult rollDice();
    std::vector<Payout> resolveBetsOnRoll(const std::vector<Bet>& bets, const RollResult& roll, int point);

private:
    TableConfig cfg_;
    std::mt19937 rng_;
};

}

#endif
