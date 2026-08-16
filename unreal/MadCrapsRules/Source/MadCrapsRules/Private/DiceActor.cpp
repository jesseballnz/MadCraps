#include "DiceActor.h"
#include "Components/StaticMeshComponent.h"

ADiceActor::ADiceActor()
{
    PrimaryActorTick.bCanEverTick = false;
    DiceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMesh"));
    RootComponent = DiceMesh;
}

void ADiceActor::SnapToFaces(int32 A, int32 B)
{
    // For prototype: simply log the faces. In a real prototype, set mesh orientation or material to show face.
    UE_LOG(LogTemp, Log, TEXT("SnapToFaces called: A=%d B=%d"), A, B);
}

void ADiceActor::PlayRollAnimation()
{
    // Placeholder: you can add a timeline or physics impulse here.
    UE_LOG(LogTemp, Log, TEXT("PlayRollAnimation called"));
}
