#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrapsTableTypes.h"
#include "CrapsTableManager.generated.h"

class ACrapsTableActor;
class ADiceActor;
class AChipActor;
class UCameraComponent;

/**
 * ACrapsTableManager
 *
 * Singleton-style game controller that owns the craps session state and
 * drives all visual feedback through the ACrapsTableActor.
 *
 * Responsibilities:
 *   - Maintain the canonical game state: point, active bets, balances
 *   - Receive roll results from the rules engine (server or local)
 *   - Resolve all active bets and calculate payouts
 *   - Instruct the table actor to update its visuals accordingly
 *   - Manage chip actors representing player wagers
 *   - Expose the full API to Blueprints for UI binding
 *
 * Usage:
 *   1. Place an instance of this actor in your level.
 *   2. Assign the TableActor reference in the Details panel.
 *   3. Call InitSession() to reset the game state.
 *   4. Hook the OnRollResolved/OnPointChanged delegates for UI updates.
 *   5. Call SubmitRoll(DieA, DieB) from your dice or server response handler.
 */
UCLASS(BlueprintType, Blueprintable)
class MADCRAPSRULES_API ACrapsTableManager : public AActor
{
    GENERATED_BODY()

public:
    ACrapsTableManager();

    // ------------------------------------------------------------------
    // References assigned in editor
    // ------------------------------------------------------------------

    /** The physical table actor in the level. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager|References")
    ACrapsTableActor* TableActor;

    // ------------------------------------------------------------------
    // Game state configuration
    // ------------------------------------------------------------------

    /** Player's starting balance. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager|Config",
              meta = (ClampMin = "0.0"))
    float StartingBalance = 1000.f;

    /** Minimum allowable bet amount per zone. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager|Config",
              meta = (ClampMin = "1.0"))
    float MinBet = 5.f;

    /** Maximum allowable bet amount per zone. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager|Config",
              meta = (ClampMin = "1.0"))
    float MaxBet = 5000.f;

    /** Odds multiplier for pass-line odds bets (0=2x, 1=3x, 2=3-4-5x, etc.). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager|Config")
    int32 OddsMultiplier = 3;

    // ------------------------------------------------------------------
    // Blueprint-readable game state
    // ------------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    float PlayerBalance = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    int32 CurrentPoint = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    TArray<FCrapsBet> ActiveBets;

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    int32 RollCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    int32 LastDieA = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    int32 LastDieB = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Manager|State")
    FCrapsRollResolution LastResolution;

    // ------------------------------------------------------------------
    // Delegates / Events
    // ------------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "Manager|Events")
    FOnRollResult OnRollResolved;

    UPROPERTY(BlueprintAssignable, Category = "Manager|Events")
    FOnPointChanged OnPointChanged;

    UPROPERTY(BlueprintAssignable, Category = "Manager|Events")
    FOnBetPlaced OnBetPlaced;

    UPROPERTY(BlueprintAssignable, Category = "Manager|Events")
    FOnBetRemoved OnBetRemoved;

    // ------------------------------------------------------------------
    // Blueprint callable API — Session control
    // ------------------------------------------------------------------

    /** Reset the game: clear all bets, reset point to 0, restore balance. */
    UFUNCTION(BlueprintCallable, Category = "Manager|Session")
    void InitSession();

    /** Set or override the player balance directly (e.g. from server sync). */
    UFUNCTION(BlueprintCallable, Category = "Manager|Session")
    void SetBalance(float NewBalance);

    // ------------------------------------------------------------------
    // Blueprint callable API — Betting
    // ------------------------------------------------------------------

    /**
     * Attempt to place a bet on the specified zone.
     * Deducts Amount from PlayerBalance and adds a chip to the table.
     * Returns false if insufficient balance or bet is out of range.
     */
    UFUNCTION(BlueprintCallable, Category = "Manager|Betting")
    bool PlaceBet(ECrapsBetZone Zone, float Amount, int32 TargetNumber = 0);

    /**
     * Remove a bet from the specified zone (returns chips to balance).
     * Not all bets are removable after the come-out roll — this function
     * enforces standard craps rules (Pass-line odds are always removable;
     * flat Pass-line is not removable after point is set).
     */
    UFUNCTION(BlueprintCallable, Category = "Manager|Betting")
    bool RemoveBet(ECrapsBetZone Zone);

    /** Remove all bets that are currently removable. */
    UFUNCTION(BlueprintCallable, Category = "Manager|Betting")
    void RemoveAllRemovableBets();

    /** Returns the total amount wagered across all active bets. */
    UFUNCTION(BlueprintPure, Category = "Manager|Betting")
    float GetTotalWagered() const;

    // ------------------------------------------------------------------
    // Blueprint callable API — Roll submission
    // ------------------------------------------------------------------

    /**
     * Submit a roll result (from physics dice, server, or test RNG).
     * Resolves all active bets, updates game state, and drives the
     * table actor's visual feedback.
     *
     * @param DieA   Face value 1–6 for die A
     * @param DieB   Face value 1–6 for die B
     */
    UFUNCTION(BlueprintCallable, Category = "Manager|Roll")
    FCrapsRollResolution SubmitRoll(int32 DieA, int32 DieB);

    /**
     * Shorthand: roll two dice using a local PRNG (for single-player / offline).
     * In networked games, rolls should come from the authoritative server.
     */
    UFUNCTION(BlueprintCallable, Category = "Manager|Roll")
    FCrapsRollResolution RollLocal();

    // ------------------------------------------------------------------
    // Blueprint callable API — Table visual control
    // ------------------------------------------------------------------

    /** Apply a new theme to the table actor. */
    UFUNCTION(BlueprintCallable, Category = "Manager|Visual")
    void SetTableTheme(ECrapsTableTheme Theme);

    /** Smoothly transition to a different camera preset. */
    UFUNCTION(BlueprintCallable, Category = "Manager|Visual")
    void SetCameraPreset(ECrapsCameraPreset Preset);

    // ------------------------------------------------------------------
    // AActor overrides
    // ------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    /**
     * Internal roll resolution using built-in Vegas rules logic.
     * Mirrors the native C++ rules engine for offline/client-prediction use.
     */
    FCrapsRollResolution ResolveRollInternal(int32 DieA, int32 DieB);

    /** Pay out a winning bet and add the result to Resolution.WinningBets. */
    float CalculatePayout(const FCrapsBet& Bet, int32 Total, int32 Point) const;

    /** Returns true if this bet type is removable in the current game phase. */
    bool IsBetRemovable(ECrapsBetZone Zone) const;

    /** Helper: returns a standard Pass-line odds payout ratio numerator/denominator. */
    void GetOddsRatio(int32 PointNumber, int32& OutNumerator, int32& OutDenominator) const;
};
