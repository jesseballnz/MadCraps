#pragma once

#include "CoreMinimal.h"
#include "MadCrapsRulesWrapper.generated.h"

/**
 * UMadCrapsRulesWrapper
 * Thin Blueprint-exposed wrapper around the native MadCraps rules engine.
 * The actual rules logic lives in the native C++ library (rules/src).
 */
UCLASS(BlueprintType)
class MADCRAPSRULES_API UMadCrapsRulesWrapper : public UObject
{
	GENERATED_BODY()

public:
	UMadCrapsRulesWrapper();

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	void SetTableConfigFromJSON(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	void Seed(int64 Seed);

	// Returns a struct or tuple for dice result + resolved payouts
	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	void ResolveRoll(int32 Die1, int32 Die2, int32 CurrentPoint);

private:
	// Native engine instance (forward declared or linked statically)
};