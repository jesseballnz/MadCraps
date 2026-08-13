#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MadCrapsDiceTypes.h"
#include "MadCrapsRulesWrapper.h"
#include "MadCrapsTableActor.generated.h"

class AMadCrapsDieActor;
class UMadCrapsDiceRollCoordinatorComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UPrimitiveComponent;
class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UTextRenderComponent;

USTRUCT()
struct FMadCrapsLabelSpec
{
	GENERATED_BODY()

	UPROPERTY()
	FString Text;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY()
	float Scale = 36.0f;
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsResolvedBet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Bets")
	FMadCrapsBet Bet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Bets")
	FMadCrapsPayout Payout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Bets")
	bool bRemainsActive = false;
};

UCLASS(Blueprintable)
class MADCRAPSRULES_API AMadCrapsTableActor : public AActor
{
	GENERATED_BODY()

public:
	AMadCrapsTableActor();

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Dice")
	void StartPreviewDiceRoll(int32 RollId, int32 RandomSeedOverride = INDEX_NONE);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Dice")
	bool ApplyAuthoritativeDiceRoll(const FMadCrapsAuthoritativeRoll& Roll);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	bool PlacePassBet(double Amount, const FString& Tag = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	bool PlaceFieldBet(double Amount, const FString& Tag = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	bool PlacePlaceBet(int32 Number, double Amount, const FString& Tag = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	bool PlaceHardwayBet(int32 Number, double Amount, const FString& Tag = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	bool PlaceBet(const FMadCrapsBet& Bet);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	bool RemoveBetByIndex(int32 BetIndex);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	int32 RemoveBetsByTag(const FString& Tag);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	void ClearAllBets();

	UFUNCTION(BlueprintPure, Category = "MadCraps|Bets")
	TArray<FMadCrapsBet> GetActiveBets() const { return ActiveBets; }

	UFUNCTION(BlueprintPure, Category = "MadCraps|Bets")
	int32 GetCurrentPoint() const { return CurrentPoint; }

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	void SetCurrentPoint(int32 NewPoint);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	TArray<FMadCrapsResolvedBet> ResolveActiveBetsForRoll(const FMadCrapsRollResult& Roll);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Bets")
	TArray<FMadCrapsResolvedBet> ResolveActiveBetsForDice(int32 Die1, int32 Die2);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "MadCraps")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Table")
	FVector TableSize = FVector(320.0f, 180.0f, 100.0f);

	UPROPERTY(EditAnywhere, Category = "MadCraps|Table")
	float FeltThickness = 4.0f;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Table")
	float RailWidth = 18.0f;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Table")
	float RailHeight = 14.0f;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Style")
	FLinearColor FeltColor = FLinearColor(0.0f, 0.33f, 0.14f);

	UPROPERTY(EditAnywhere, Category = "MadCraps|Style")
	FLinearColor LayoutLineColor = FLinearColor(0.96f, 0.9f, 0.67f);

	UPROPERTY(EditAnywhere, Category = "MadCraps|Style")
	FLinearColor RailColor = FLinearColor(0.24f, 0.08f, 0.03f);

	UPROPERTY(EditAnywhere, Category = "MadCraps|Style")
	FLinearColor WoodColor = FLinearColor(0.19f, 0.12f, 0.07f);

	UPROPERTY(EditAnywhere, Category = "MadCraps|Chips")
	int32 ChipStacksPerSide = 4;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Chips")
	int32 ChipsPerStack = 8;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Chips")
	float ChipRadius = 6.5f;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Chips")
	float ChipHeight = 1.2f;

	UPROPERTY(EditAnywhere, Category = "MadCraps|Chips")
	TArray<FLinearColor> ChipColors;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> GeneratedMeshes;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextRenderComponent>> GeneratedTexts;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> CylinderMesh;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MadCraps|Dice")
	TObjectPtr<UMadCrapsDiceRollCoordinatorComponent> DiceRollCoordinator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MadCraps|Bets")
	TObjectPtr<UMadCrapsRulesWrapper> RulesWrapper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	TSubclassOf<AMadCrapsDieActor> DieActorClass;

	UPROPERTY(Transient)
	TObjectPtr<AMadCrapsDieActor> DieOneActor;

	UPROPERTY(Transient)
	TObjectPtr<AMadCrapsDieActor> DieTwoActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MadCraps|Bets")
	TArray<FMadCrapsBet> ActiveBets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Bets", meta = (ClampMin = "0"))
	int32 CurrentPoint = 0;

	void RebuildTable();
	void ClearGeneratedComponents();
	void LoadSharedAssets();
	UStaticMeshComponent* AddBox(const FString& Name, const FVector& Location, const FVector& Size, const FLinearColor& Color);
	UStaticMeshComponent* AddCylinder(const FString& Name, const FVector& Location, float Radius, float Height, const FLinearColor& Color);
	void AddLabel(const FMadCrapsLabelSpec& LabelSpec);
	void BuildTableShell();
	void BuildBettingLayout();
	void BuildChipStacks();
	void EnsureDiceActors();
	FTransform GetDieSpawnTransform(int32 DieIndex) const;
	UMaterialInstanceDynamic* CreateColorMaterial(UPrimitiveComponent* Component, const FLinearColor& Color) const;
	bool ValidateBet(const FMadCrapsBet& Bet) const;
	bool IsValidPlaceNumber(int32 Number) const;
	bool IsValidHardwayNumber(int32 Number) const;
	bool ShouldKeepBetAfterRoll(const FMadCrapsBet& Bet, const FMadCrapsPayout& Payout) const;
	int32 GetNextPointForRoll(const FMadCrapsRollResult& Roll) const;
};
