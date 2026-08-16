#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestLevelBootstrap.generated.h"

UCLASS()
class ATestLevelBootstrap : public AActor
{
    GENERATED_BODY()

public:
    ATestLevelBootstrap();

    virtual void BeginPlay() override;

    // Server base URL to use for the test roller
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Test")
    FString ServerBaseUrl = TEXT("http://127.0.0.1:3000");

    // Whether to auto-trigger a test roll at BeginPlay
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Test")
    bool bAutoTrigger = true;
};
