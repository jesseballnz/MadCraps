#include "CrapsTableManager.h"
#include "CrapsTableActor.h"
#include "BetZoneComponent.h"
#include "DiceActor.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

ACrapsTableManager::ACrapsTableManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACrapsTableManager::BeginPlay()
{
    Super::BeginPlay();
    InitSession();
}

void ACrapsTableManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ---------------------------------------------------------------------------
// Session control
// ---------------------------------------------------------------------------

void ACrapsTableManager::InitSession()
{
    PlayerBalance = StartingBalance;
    CurrentPoint  = 0;
    RollCount     = 0;
    LastDieA      = 0;
    LastDieB      = 0;
    ActiveBets.Empty();

    if (TableActor)
    {
        TableActor->ClearAllBets();
        TableActor->SetCurrentPoint(0);
    }

    OnPointChanged.Broadcast(0);
}

void ACrapsTableManager::SetBalance(float NewBalance)
{
    PlayerBalance = FMath::Max(0.f, NewBalance);
}

// ---------------------------------------------------------------------------
// Betting
// ---------------------------------------------------------------------------

bool ACrapsTableManager::PlaceBet(ECrapsBetZone Zone, float Amount, int32 TargetNumber)
{
    if (Amount < MinBet || Amount > MaxBet)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("ACrapsTableManager: Bet amount %.2f is outside [%.2f, %.2f]"),
            Amount, MinBet, MaxBet);
        return false;
    }

    if (PlayerBalance < Amount)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("ACrapsTableManager: Insufficient balance %.2f for bet %.2f"),
            PlayerBalance, Amount);
        return false;
    }

    PlayerBalance -= Amount;

    FCrapsBet NewBet;
    NewBet.Zone         = Zone;
    NewBet.Amount       = Amount;
    NewBet.TargetNumber = TargetNumber;
    NewBet.bActive      = true;
    ActiveBets.Add(NewBet);

    if (TableActor)
    {
        TableActor->PlaceBet(Zone, Amount);
    }

    OnBetPlaced.Broadcast(Zone, Amount);
    return true;
}

bool ACrapsTableManager::RemoveBet(ECrapsBetZone Zone)
{
    if (!IsBetRemovable(Zone))
    {
        return false;
    }

    for (int32 i = ActiveBets.Num() - 1; i >= 0; --i)
    {
        if (ActiveBets[i].Zone == Zone)
        {
            const float Amount = ActiveBets[i].Amount;
            PlayerBalance += Amount;
            ActiveBets.RemoveAt(i);

            if (TableActor)
            {
                TableActor->RemoveBet(Zone);
            }

            OnBetRemoved.Broadcast(Zone, Amount);
            return true;
        }
    }
    return false;
}

void ACrapsTableManager::RemoveAllRemovableBets()
{
    for (int32 i = ActiveBets.Num() - 1; i >= 0; --i)
    {
        const ECrapsBetZone Zone = ActiveBets[i].Zone;
        if (IsBetRemovable(Zone))
        {
            const float Amount = ActiveBets[i].Amount;
            PlayerBalance += Amount;
            ActiveBets.RemoveAt(i);
            if (TableActor) { TableActor->RemoveBet(Zone); }
            OnBetRemoved.Broadcast(Zone, Amount);
        }
    }
}

float ACrapsTableManager::GetTotalWagered() const
{
    float Total = 0.f;
    for (const FCrapsBet& Bet : ActiveBets)
    {
        Total += Bet.Amount;
    }
    return Total;
}

// ---------------------------------------------------------------------------
// Roll submission
// ---------------------------------------------------------------------------

FCrapsRollResolution ACrapsTableManager::SubmitRoll(int32 DieA, int32 DieB)
{
    DieA = FMath::Clamp(DieA, 1, 6);
    DieB = FMath::Clamp(DieB, 1, 6);

    LastDieA = DieA;
    LastDieB = DieB;
    ++RollCount;

    // Animate dice actors if available
    if (TableActor)
    {
        if (TableActor->DiceA) { TableActor->DiceA->SnapToFaces(DieA, 0); }
        if (TableActor->DiceB) { TableActor->DiceB->SnapToFaces(DieB, 0); }
    }

    LastResolution = ResolveRollInternal(DieA, DieB);

    // Pay out winners
    PlayerBalance += LastResolution.TotalPayout;

    // Remove resolved losing bets from active list
    for (const FCrapsBet& LostBet : LastResolution.LosingBets)
    {
        ActiveBets.RemoveAll([&LostBet](const FCrapsBet& B) {
            return B.Zone == LostBet.Zone;
        });
    }

    // Update manager point state
    CurrentPoint = LastResolution.PointAfterRoll;

    // Drive table visuals
    if (TableActor)
    {
        TableActor->OnRollResult(LastResolution);
    }

    OnRollResolved.Broadcast(LastResolution);
    OnPointChanged.Broadcast(CurrentPoint);

    return LastResolution;
}

FCrapsRollResolution ACrapsTableManager::RollLocal()
{
    const int32 A = FMath::RandRange(1, 6);
    const int32 B = FMath::RandRange(1, 6);
    return SubmitRoll(A, B);
}

// ---------------------------------------------------------------------------
// Visual helpers
// ---------------------------------------------------------------------------

void ACrapsTableManager::SetTableTheme(ECrapsTableTheme Theme)
{
    if (TableActor)
    {
        TableActor->ApplyTheme(Theme);
    }
}

void ACrapsTableManager::SetCameraPreset(ECrapsCameraPreset Preset)
{
    // Camera actor is not directly referenced here — broadcast via the table actor's event
    // or let Blueprints handle camera transitions.
    UE_LOG(LogTemp, Log, TEXT("ACrapsTableManager: SetCameraPreset %d requested"),
        static_cast<int32>(Preset));
}

// ---------------------------------------------------------------------------
// Internal resolution
// ---------------------------------------------------------------------------

FCrapsRollResolution ACrapsTableManager::ResolveRollInternal(int32 DieA, int32 DieB)
{
    FCrapsRollResolution R;
    R.DieA  = DieA;
    R.DieB  = DieB;
    R.Total = DieA + DieB;

    // ---------- Come-out phase (CurrentPoint == 0) ----------
    if (CurrentPoint == 0)
    {
        if (R.Total == 7 || R.Total == 11)
        {
            R.Outcome        = ECrapsOutcome::NaturalWin;
            R.PointAfterRoll = 0;
        }
        else if (R.Total == 2 || R.Total == 3 || R.Total == 12)
        {
            R.Outcome        = ECrapsOutcome::CrapsOut;
            R.PointAfterRoll = 0;
        }
        else
        {
            R.Outcome        = ECrapsOutcome::PointSet;
            R.PointAfterRoll = R.Total;
        }
    }
    // ---------- Point phase ----------
    else
    {
        if (R.Total == CurrentPoint)
        {
            R.Outcome        = ECrapsOutcome::PointHit;
            R.PointAfterRoll = 0;
        }
        else if (R.Total == 7)
        {
            R.Outcome        = ECrapsOutcome::SevenOut;
            R.PointAfterRoll = 0;
        }
        else
        {
            R.Outcome        = ECrapsOutcome::None;
            R.PointAfterRoll = CurrentPoint;
        }
    }

    // ---------- Resolve active bets ----------
    float TotalPayout = 0.f;

    for (FCrapsBet& Bet : ActiveBets)
    {
        const float Payout = CalculatePayout(Bet, R.Total, CurrentPoint);
        if (Payout > 0.f)
        {
            FCrapsBet WinCopy = Bet;
            R.WinningBets.Add(WinCopy);
            TotalPayout += Payout;
        }
        else if (Payout < 0.f)
        {
            R.LosingBets.Add(Bet);
        }
    }

    R.TotalPayout = TotalPayout;
    return R;
}

float ACrapsTableManager::CalculatePayout(const FCrapsBet& Bet, int32 Total, int32 Point) const
{
    // Returns net return (including stake) on a win, 0 on no resolution, -Bet.Amount on a loss.

    switch (Bet.Zone)
    {
    // ---- Pass Line ----
    case ECrapsBetZone::PassLine:
        if (Point == 0) // come-out
        {
            if (Total == 7 || Total == 11) return Bet.Amount * 2.f;
            if (Total == 2 || Total == 3 || Total == 12) return -Bet.Amount;
        }
        else
        {
            if (Total == Point) return Bet.Amount * 2.f;
            if (Total == 7)     return -Bet.Amount;
        }
        return 0.f;

    // ---- Don't Pass ----
    case ECrapsBetZone::DontPass:
        if (Point == 0)
        {
            if (Total == 2 || Total == 3) return Bet.Amount * 2.f;
            if (Total == 12)              return 0.f; // push
            if (Total == 7 || Total == 11) return -Bet.Amount;
        }
        else
        {
            if (Total == 7)     return Bet.Amount * 2.f;
            if (Total == Point) return -Bet.Amount;
        }
        return 0.f;

    // ---- Field ----
    case ECrapsBetZone::Field:
        {
            if (Total == 3 || Total == 4 || Total == 9 || Total == 10 || Total == 11)
                return Bet.Amount * 2.f;                  // 1:1
            if (Total == 2)  return Bet.Amount * 3.f;     // 2:1 (some tables pay 3:1)
            if (Total == 12) return Bet.Amount * 3.f;     // 2:1 (some tables pay 3:1)
            return -Bet.Amount;                           // 5,6,7,8 lose
        }

    // ---- Place bets ----
    case ECrapsBetZone::Place4:
        if (Total == 4) return Bet.Amount + Bet.Amount * 9.f / 5.f;  // 9:5
        if (Total == 7) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::Place5:
        if (Total == 5) return Bet.Amount + Bet.Amount * 7.f / 5.f;  // 7:5
        if (Total == 7) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::Place6:
        if (Total == 6) return Bet.Amount + Bet.Amount * 7.f / 6.f;  // 7:6
        if (Total == 7) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::Place8:
        if (Total == 8) return Bet.Amount + Bet.Amount * 7.f / 6.f;  // 7:6
        if (Total == 7) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::Place9:
        if (Total == 9) return Bet.Amount + Bet.Amount * 7.f / 5.f;  // 7:5
        if (Total == 7) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::Place10:
        if (Total == 10) return Bet.Amount + Bet.Amount * 9.f / 5.f; // 9:5
        if (Total == 7)  return -Bet.Amount;
        return 0.f;

    // ---- Hardways ----
    case ECrapsBetZone::HardWay4:
        if (Total == 4 && Bet.TargetNumber == 4) // hard: both dice show 2
            return Bet.Amount * 8.f; // 7:1
        if (Total == 7 || (Total == 4)) return -Bet.Amount; // seven out or easy way
        return 0.f;
    case ECrapsBetZone::HardWay6:
        if (Total == 6 && Bet.TargetNumber == 6)
            return Bet.Amount * 10.f; // 9:1
        if (Total == 7 || Total == 6) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::HardWay8:
        if (Total == 8 && Bet.TargetNumber == 8)
            return Bet.Amount * 10.f; // 9:1
        if (Total == 7 || Total == 8) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::HardWay10:
        if (Total == 10 && Bet.TargetNumber == 10)
            return Bet.Amount * 8.f; // 7:1
        if (Total == 7 || Total == 10) return -Bet.Amount;
        return 0.f;

    // ---- Any 7 ----
    case ECrapsBetZone::Any7:
        if (Total == 7)  return Bet.Amount * 5.f; // 4:1
        return -Bet.Amount;                         // always resolves per roll

    // ---- Any Craps ----
    case ECrapsBetZone::AnyCraps:
        if (Total == 2 || Total == 3 || Total == 12) return Bet.Amount * 8.f; // 7:1
        return -Bet.Amount;

    // ---- Horn (2, 3, 11, 12 — 1/4 each unit) ----
    case ECrapsBetZone::Horn:
        if (Total == 2 || Total == 12) return Bet.Amount * 27.f / 4.f; // approx
        if (Total == 3 || Total == 11) return Bet.Amount * 15.f / 4.f;
        return -Bet.Amount;

    // ---- Big 6 / Big 8 ----
    case ECrapsBetZone::Big6:
        if (Total == 6) return Bet.Amount * 2.f;
        if (Total == 7) return -Bet.Amount;
        return 0.f;
    case ECrapsBetZone::Big8:
        if (Total == 8) return Bet.Amount * 2.f;
        if (Total == 7) return -Bet.Amount;
        return 0.f;

    default:
        return 0.f;
    }
}

bool ACrapsTableManager::IsBetRemovable(ECrapsBetZone Zone) const
{
    // Pass-line flat bet is not removable after the point is set
    if (Zone == ECrapsBetZone::PassLine && CurrentPoint != 0)
    {
        return false;
    }

    // Don't Pass flat bet is technically removable (though disadvantageous)
    // Place, Come, Field, Hardways, Props are always removable
    return true;
}

void ACrapsTableManager::GetOddsRatio(int32 PointNumber, int32& OutNumerator, int32& OutDenominator) const
{
    // True-odds payouts for pass-line odds bets
    switch (PointNumber)
    {
    case 4:
    case 10:
        OutNumerator   = 2;
        OutDenominator = 1;
        break;
    case 5:
    case 9:
        OutNumerator   = 3;
        OutDenominator = 2;
        break;
    case 6:
    case 8:
        OutNumerator   = 6;
        OutDenominator = 5;
        break;
    default:
        OutNumerator   = 1;
        OutDenominator = 1;
        break;
    }
}
