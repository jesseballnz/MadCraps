#include "TestLevelBootstrap.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TestRoller.h"
#include "DiceActor.h"

ATestLevelBootstrap::ATestLevelBootstrap()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATestLevelBootstrap::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    // Spawn a DiceActor
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ADiceActor* Dice = World->SpawnActor<ADiceActor>(ADiceActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);

    // Spawn a TestRoller and wire it to the dice
    ATestRoller* Roller = World->SpawnActor<ATestRoller>(ATestRoller::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if (Roller && Dice)
    {
        Roller->DiceActor = Dice;
        Roller->ServerBaseUrl = ServerBaseUrl;
    }

    if (bAutoTrigger && Roller)
    {
        // trigger a test roll
        Roller->StartRoll(TEXT("test-auto-1"));
    }
}
