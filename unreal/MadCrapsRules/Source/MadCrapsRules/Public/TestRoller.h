#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestRoller.generated.h"

UCLASS()
class ATestRoller : public AActor
{
    GENERATED_BODY()

public:
    ATestRoller();

    // Server base URL (e.g., http://127.0.0.1:3000)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Network")
    FString ServerBaseUrl;

    // Dice actor to drive when a roll result is verified
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Gameplay")
    AActor* DiceActor;

    virtual void BeginPlay() override;

    // Trigger a roll request to the server (Blueprint-callable)
    UFUNCTION(BlueprintCallable, Category = "MadCraps|Network")
    void StartRoll(const FString& Nonce);

private:
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
