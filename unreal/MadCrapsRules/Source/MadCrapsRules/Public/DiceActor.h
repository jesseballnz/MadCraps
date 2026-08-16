#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiceActor.generated.h"

UCLASS()
class ADiceActor : public AActor
{
    GENERATED_BODY()

public:
    ADiceActor();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice")
    UStaticMeshComponent* DiceMesh;

    // Snap the dice to faces (A and B range 1-6)
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void SnapToFaces(int32 A, int32 B);

    // Simple play animation (placeholder)
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void PlayRollAnimation();
};
