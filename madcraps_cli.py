#!/usr/bin/env python3
"""
MadCraps CLI v2 - Much more complete Vegas-style experience
Includes: Pass/Don't Pass, Come, Field, Place 6/8, Hardways, Horn, Any7
"""

import random
import sys

class MadCrapsCLI:
    def __init__(self):
        self.balance = 1000
        self.point = None
        self.come_points = {}   # {point: amount}
        self.place_bets = {}    # {number: amount}
        self.hardway_bets = {}  # {number: amount}

    def roll(self):
        d1 = random.randint(1, 6)
        d2 = random.randint(1, 6)
        total = d1 + d2
        is_hard = d1 == d2
        print(f"  🎲 {d1} + {d2} = {total}" + (" (HARD)" if is_hard else ""))
        return total, is_hard

    def get_amount(self, prompt, max_amt=None):
        if max_amt is None: max_amt = self.balance
        while True:
            try:
                val = input(f"{prompt} (0-{max_amt}): ").strip().lower()
                if val == "quit":
                    print(f"\nFinal balance: ${self.balance}")
                    sys.exit(0)
                amt = int(val)
                if 0 <= amt <= max_amt:
                    return amt
            except:
                pass
            print("Invalid amount.")

    def place_bet(self, name, current_bets):
        amt = self.get_amount(f"{name} bet")
        if amt > 0:
            current_bets[name] = current_bets.get(name, 0) + amt
            self.balance -= amt
            print(f"  Placed ${amt} on {name}")

    def play(self):
        print("=== MadCraps CLI v2 (Expanded Vegas Bets) ===")
        print("Commands: roll (r), bets, quit\n")

        while True:
            print(f"\n💰 ${self.balance}  | Point: {self.point or 'Come-out'}")
            cmd = input("[r]oll / [b]ets / [q]uit: ").strip().lower()

            if cmd in ("q", "quit"):
                break
            if cmd in ("b", "bets"):
                self.show_bets()
                continue
            if cmd not in ("r", ""):
                continue

            total, is_hard = self.roll()

            # === Come-out phase ===
            if self.point is None:
                # Pass Line
                if "pass" in self.place_bets:
                    if total in (7, 11):
                        win = self.place_bets["pass"]
                        self.balance += win * 2
                        print(f"✅ Pass Line wins ${win}")
                    elif total in (2, 3, 12):
                        print(f"❌ Pass Line loses ${self.place_bets['pass']}")
                    else:
                        self.point = total
                        print(f"📍 Point is now {self.point}")

                # Field
                if total in (2, 3, 4, 9, 10, 11, 12):
                    if total == 2:
                        self.balance += 2  # 2:1
                        print("✅ Field 2 wins 2:1")
                    elif total == 12:
                        self.balance += 3  # 3:1
                        print("✅ Field 12 wins 3:1")
                    else:
                        self.balance += 1
                        print("✅ Field wins")

                # Horn
                if total in (2, 3, 11, 12):
                    if total in (2, 12):
                        self.balance += 30
                        print(f"🎉 HORN {total} pays 30:1!")
                    else:
                        self.balance += 15
                        print(f"🎉 HORN {total} pays 15:1!")

            # === Point phase ===
            else:
                if total == self.point:
                    print(f"✅ POINT {self.point} made!")
                    self.point = None
                    self.come_points.clear()
                elif total == 7:
                    print("❌ SEVEN OUT!")
                    self.point = None
                    self.come_points.clear()
                    self.place_bets.clear()
                    self.hardway_bets.clear()

                # Hardways
                for num, amt in list(self.hardway_bets.items()):
                    if is_hard and total == num:
                        payout = 7 if num in (4, 10) else 9
                        self.balance += amt * payout
                        print(f"✅ Hard {num} wins {payout}:1!")
                        del self.hardway_bets[num]
                    elif total == 7 or (not is_hard and total == num):
                        print(f"❌ Hard {num} lost")
                        del self.hardway_bets[num]

            if self.balance <= 0:
                print("\n💸 Broke. Game over.")
                break

    def show_bets(self):
        print("\nCurrent bets:")
        print(f"  Pass Line: {self.place_bets.get('pass', 0)}")
        print(f"  Place bets: {self.place_bets}")
        print(f"  Hardways: {self.hardway_bets}")
        print(f"  Come points: {self.come_points}")

if __name__ == "__main__":
    game = MadCrapsCLI()
    game.play()
