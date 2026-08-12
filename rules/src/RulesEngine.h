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
    Come,
    DontCome,
    Odds,      // attached to a point (target)
    Field,
    Place,     // target set in Bet.target
    Buy,       // target set in Bet.target (buy takes commission on win)
    Lay,       // target set in Bet.target
    Hardway,   // target set in Bet.target (4,6,8,10)
    Big6,
    Big8,
    Any7,
    AnyCraps, // 2,3,12
    // ...additional bet types may be added
};

struct Bet {
    BetType type;
    double amount;
    int target = 0; // e.g., point (4/5/6/8/9/10) for place/buy/lay/odds/hardway
    std::string tag; // optional freeform tag (e.g., owner/player id)
};

struct TableConfig {
    double field_two_payout = 2.0; // 2:1
    double field_twelve_payout = 3.0; // 3:1 (Vegas default)
    std::map<int,double> odds_payout; // payout multiplier for odds (e.g., 4 -> 2.0)
    std::map<int,double> place_payout; // payout ratio for place bets (win per unit)
    std::map<int,double> buy_payout; // payout ratio for buy bets (usually true odds minus commission)
    std::map<int,double> lay_payout; // payout ratio for lay bets (how much player wins per unit risked)
    std::map<int,double> hardway_payout; // payout (e.g., hard 4 -> 7:1 -> 7.0)
    double buy_commission_pct = 0.05; // 5% commission on buy wins by default
    std::map<std::string,double> prop_payouts; // Any7, AnyCraps, etc.
};

struct RollResult {
    int die1;
    int die2;
    int total() const { return die1 + die2; }
    bool isHardway(int target) const { return (die1 == die2) && (total() == target); }
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
    // Resolve bets for a single roll. 'point' is current point (0 == come-out)
    std::vector<Payout> resolveBetsOnRoll(const std::vector<Bet>& bets, const RollResult& roll, int point);

private:
    TableConfig cfg_;
    std::mt19937 rng_;
};

}

#endif
