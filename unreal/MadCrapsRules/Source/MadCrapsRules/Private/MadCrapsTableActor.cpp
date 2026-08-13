#include "MadCrapsTableActor.h"

#include "CollisionProfile.h"
#include "MadCrapsDiceRollCoordinatorComponent.h"
#include "MadCrapsDieActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace MadCrapsTable
{
	static constexpr float BoxUnitSize = 100.0f;
	static constexpr float CylinderHeightUnit = 100.0f;
	static constexpr float CylinderRadiusUnit = 50.0f;
}

AMadCrapsTableActor::AMadCrapsTableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DiceRollCoordinator = CreateDefaultSubobject<UMadCrapsDiceRollCoordinatorComponent>(TEXT("DiceRollCoordinator"));
	RulesWrapper = CreateDefaultSubobject<UMadCrapsRulesWrapper>(TEXT("RulesWrapper"));
	DieActorClass = AMadCrapsDieActor::StaticClass();

	ChipColors = {
		FLinearColor(0.75f, 0.1f, 0.12f),
		FLinearColor(0.97f, 0.97f, 0.97f),
		FLinearColor(0.12f, 0.23f, 0.75f),
		FLinearColor(0.03f, 0.03f, 0.03f)
	};
}

void AMadCrapsTableActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RebuildTable();
	EnsureDiceActors();
}

void AMadCrapsTableActor::RebuildTable()
{
	LoadSharedAssets();
	ClearGeneratedComponents();
	BuildTableShell();
	BuildBettingLayout();
	BuildChipStacks();
}

void AMadCrapsTableActor::StartPreviewDiceRoll(const int32 RollId, const int32 RandomSeedOverride)
{
	EnsureDiceActors();
	if (DiceRollCoordinator)
	{
		DiceRollCoordinator->StartLocalRoll(RollId, RandomSeedOverride);
	}
}

bool AMadCrapsTableActor::ApplyAuthoritativeDiceRoll(const FMadCrapsAuthoritativeRoll& Roll)
{
	EnsureDiceActors();
	return DiceRollCoordinator ? DiceRollCoordinator->SubmitAuthoritativeRoll(Roll) : false;
}

bool AMadCrapsTableActor::PlacePassBet(const double Amount, const FString& Tag)
{
	FMadCrapsBet Bet;
	Bet.Type = EMadCrapsBetType::PassLine;
	Bet.Amount = Amount;
	Bet.Tag = Tag;
	return PlaceBet(Bet);
}

bool AMadCrapsTableActor::PlaceFieldBet(const double Amount, const FString& Tag)
{
	FMadCrapsBet Bet;
	Bet.Type = EMadCrapsBetType::Field;
	Bet.Amount = Amount;
	Bet.Tag = Tag;
	return PlaceBet(Bet);
}

bool AMadCrapsTableActor::PlacePlaceBet(const int32 Number, const double Amount, const FString& Tag)
{
	FMadCrapsBet Bet;
	Bet.Type = EMadCrapsBetType::Place;
	Bet.Target = Number;
	Bet.Amount = Amount;
	Bet.Tag = Tag;
	return PlaceBet(Bet);
}

bool AMadCrapsTableActor::PlaceHardwayBet(const int32 Number, const double Amount, const FString& Tag)
{
	FMadCrapsBet Bet;
	Bet.Type = EMadCrapsBetType::Hardway;
	Bet.Target = Number;
	Bet.Amount = Amount;
	Bet.Tag = Tag;
	return PlaceBet(Bet);
}

bool AMadCrapsTableActor::PlaceBet(const FMadCrapsBet& Bet)
{
	if (!ValidateBet(Bet))
	{
		return false;
	}

	ActiveBets.Add(Bet);
	return true;
}

bool AMadCrapsTableActor::RemoveBetByIndex(const int32 BetIndex)
{
	if (!ActiveBets.IsValidIndex(BetIndex))
	{
		return false;
	}

	ActiveBets.RemoveAt(BetIndex);
	return true;
}

int32 AMadCrapsTableActor::RemoveBetsByTag(const FString& Tag)
{
	if (Tag.IsEmpty())
	{
		return 0;
	}

	const int32 RemovedCount = ActiveBets.RemoveAll([&Tag](const FMadCrapsBet& Bet)
	{
		return Bet.Tag == Tag;
	});

	return RemovedCount;
}

void AMadCrapsTableActor::ClearAllBets()
{
	ActiveBets.Reset();
}

void AMadCrapsTableActor::SetCurrentPoint(const int32 NewPoint)
{
	CurrentPoint = IsValidPlaceNumber(NewPoint) ? NewPoint : 0;
}

TArray<FMadCrapsResolvedBet> AMadCrapsTableActor::ResolveActiveBetsForRoll(const FMadCrapsRollResult& Roll)
{
	TArray<FMadCrapsResolvedBet> Results;
	if (!RulesWrapper || ActiveBets.IsEmpty())
	{
		CurrentPoint = GetNextPointForRoll(Roll);
		return Results;
	}

	const TArray<FMadCrapsPayout> Payouts = RulesWrapper->ResolveBets(ActiveBets, Roll, CurrentPoint);
	Results.Reserve(ActiveBets.Num());

	TArray<FMadCrapsBet> RemainingBets;
	RemainingBets.Reserve(ActiveBets.Num());

	for (int32 BetIndex = 0; BetIndex < ActiveBets.Num(); ++BetIndex)
	{
		const FMadCrapsBet& Bet = ActiveBets[BetIndex];
		const FMadCrapsPayout Payout = Payouts.IsValidIndex(BetIndex) ? Payouts[BetIndex] : FMadCrapsPayout{};
		const bool bKeepBet = ShouldKeepBetAfterRoll(Bet, Payout);

		FMadCrapsResolvedBet& Result = Results.AddDefaulted_GetRef();
		Result.Bet = Bet;
		Result.Payout = Payout;
		Result.bRemainsActive = bKeepBet;

		if (bKeepBet)
		{
			RemainingBets.Add(Bet);
		}
	}

	ActiveBets = MoveTemp(RemainingBets);
	CurrentPoint = GetNextPointForRoll(Roll);
	return Results;
}

TArray<FMadCrapsResolvedBet> AMadCrapsTableActor::ResolveActiveBetsForDice(const int32 Die1, const int32 Die2)
{
	FMadCrapsRollResult Roll;
	Roll.Die1 = Die1;
	Roll.Die2 = Die2;
	Roll.Total = Die1 + Die2;
	return ResolveActiveBetsForRoll(Roll);
}

void AMadCrapsTableActor::ClearGeneratedComponents()
{
	for (UStaticMeshComponent* MeshComponent : GeneratedMeshes)
	{
		if (IsValid(MeshComponent))
		{
			MeshComponent->DestroyComponent();
		}
	}

	for (UTextRenderComponent* TextComponent : GeneratedTexts)
	{
		if (IsValid(TextComponent))
		{
			TextComponent->DestroyComponent();
		}
	}

	GeneratedMeshes.Reset();
	GeneratedTexts.Reset();
}

void AMadCrapsTableActor::LoadSharedAssets()
{
	if (!CubeMesh)
	{
		CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	}

	if (!CylinderMesh)
	{
		CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	}

	if (!BaseMaterial)
	{
		BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}
}

UStaticMeshComponent* AMadCrapsTableActor::AddBox(const FString& Name, const FVector& Location, const FVector& Size, const FLinearColor& Color)
{
	if (!CubeMesh)
	{
		return nullptr;
	}

	UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this, *Name);
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetStaticMesh(CubeMesh);
	MeshComponent->SetRelativeLocation(Location);
	MeshComponent->SetRelativeScale3D(FVector(Size.X / MadCrapsTable::BoxUnitSize, Size.Y / MadCrapsTable::BoxUnitSize, Size.Z / MadCrapsTable::BoxUnitSize));
	MeshComponent->SetMobility(EComponentMobility::Static);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	MeshComponent->RegisterComponent();
	CreateColorMaterial(MeshComponent, Color);
	GeneratedMeshes.Add(MeshComponent);
	return MeshComponent;
}

UStaticMeshComponent* AMadCrapsTableActor::AddCylinder(const FString& Name, const FVector& Location, float Radius, float Height, const FLinearColor& Color)
{
	if (!CylinderMesh)
	{
		return nullptr;
	}

	UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this, *Name);
	MeshComponent->SetupAttachment(SceneRoot);
	MeshComponent->SetStaticMesh(CylinderMesh);
	MeshComponent->SetRelativeLocation(Location);
	MeshComponent->SetRelativeScale3D(FVector(Radius / MadCrapsTable::CylinderRadiusUnit, Radius / MadCrapsTable::CylinderRadiusUnit, Height / MadCrapsTable::CylinderHeightUnit));
	MeshComponent->SetMobility(EComponentMobility::Static);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->RegisterComponent();
	CreateColorMaterial(MeshComponent, Color);
	GeneratedMeshes.Add(MeshComponent);
	return MeshComponent;
}

UMaterialInstanceDynamic* AMadCrapsTableActor::CreateColorMaterial(UPrimitiveComponent* Component, const FLinearColor& Color) const
{
	if (!BaseMaterial || !Component)
	{
		return nullptr;
	}

	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, Component);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
		Component->SetMaterial(0, DynamicMaterial);
	}
	return DynamicMaterial;
}

void AMadCrapsTableActor::AddLabel(const FMadCrapsLabelSpec& LabelSpec)
{
	UTextRenderComponent* TextComponent = NewObject<UTextRenderComponent>(this);
	TextComponent->SetupAttachment(SceneRoot);
	TextComponent->SetRelativeLocation(LabelSpec.Location);
	TextComponent->SetRelativeRotation(LabelSpec.Rotation);
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextComponent->SetWorldSize(LabelSpec.Scale);
	TextComponent->SetText(FText::FromString(LabelSpec.Text));
	TextComponent->SetTextRenderColor(LayoutLineColor.ToFColor(true));
	TextComponent->SetMobility(EComponentMobility::Static);
	TextComponent->RegisterComponent();
	GeneratedTexts.Add(TextComponent);
}

void AMadCrapsTableActor::BuildTableShell()
{
	const float TableTopZ = TableSize.Z;
	const float InnerX = TableSize.X - (RailWidth * 2.0f);
	const float InnerY = TableSize.Y - (RailWidth * 2.0f);
	const float RailZ = TableTopZ + (RailHeight * 0.5f);

	AddBox(TEXT("TableBase"), FVector(0.0f, 0.0f, TableSize.Z * 0.5f), TableSize, WoodColor);
	AddBox(TEXT("Felt"), FVector(0.0f, 0.0f, TableTopZ + (FeltThickness * 0.5f)), FVector(InnerX, InnerY, FeltThickness), FeltColor);
	AddBox(TEXT("RailNorth"), FVector(0.0f, (InnerY + RailWidth) * 0.5f, RailZ), FVector(TableSize.X, RailWidth, RailHeight), RailColor);
	AddBox(TEXT("RailSouth"), FVector(0.0f, -(InnerY + RailWidth) * 0.5f, RailZ), FVector(TableSize.X, RailWidth, RailHeight), RailColor);
	AddBox(TEXT("RailEast"), FVector((InnerX + RailWidth) * 0.5f, 0.0f, RailZ), FVector(RailWidth, InnerY, RailHeight), RailColor);
	AddBox(TEXT("RailWest"), FVector(-(InnerX + RailWidth) * 0.5f, 0.0f, RailZ), FVector(RailWidth, InnerY, RailHeight), RailColor);
	AddBox(TEXT("DealerWall"), FVector(TableSize.X * 0.37f, 0.0f, TableTopZ + 32.0f), FVector(12.0f, InnerY * 0.82f, 46.0f), RailColor);
	AddBox(TEXT("Apron"), FVector(-TableSize.X * 0.46f, 0.0f, TableTopZ + 6.0f), FVector(30.0f, InnerY * 0.92f, 12.0f), RailColor);
}

void AMadCrapsTableActor::BuildBettingLayout()
{
	const float LayoutZ = TableSize.Z + FeltThickness + 0.6f;
	const float LineThickness = 1.5f;
	const float InnerX = TableSize.X - (RailWidth * 2.0f);
	const float InnerY = TableSize.Y - (RailWidth * 2.0f);
	const float PlayerEdgeX = -InnerX * 0.24f;
	const float CenterX = -InnerX * 0.02f;
	const float PropositionX = InnerX * 0.19f;
	const float NumberBandX = InnerX * 0.07f;

	AddBox(TEXT("PassLine"), FVector(PlayerEdgeX, 0.0f, LayoutZ), FVector(InnerX * 0.9f, InnerY * 0.88f, LineThickness), LayoutLineColor);
	AddBox(TEXT("PassInset"), FVector(PlayerEdgeX + 14.0f, 0.0f, LayoutZ + 0.5f), FVector(InnerX * 0.78f, InnerY * 0.74f, LineThickness), FeltColor);
	AddBox(TEXT("ComeBox"), FVector(CenterX, 0.0f, LayoutZ), FVector(InnerX * 0.34f, InnerY * 0.6f, LineThickness), LayoutLineColor);
	AddBox(TEXT("ComeInset"), FVector(CenterX, 0.0f, LayoutZ + 0.5f), FVector(InnerX * 0.31f, InnerY * 0.57f, LineThickness), FeltColor);

	const TArray<FString> Numbers = { TEXT("4"), TEXT("5"), TEXT("6"), TEXT("8"), TEXT("9"), TEXT("10") };
	for (int32 Index = 0; Index < Numbers.Num(); ++Index)
	{
		const float Y = -InnerY * 0.35f + (Index * (InnerY * 0.14f));
		AddBox(*FString::Printf(TEXT("NumberBox_%d"), Index), FVector(NumberBandX, Y, LayoutZ), FVector(InnerX * 0.16f, InnerY * 0.11f, LineThickness), LayoutLineColor);
		AddBox(*FString::Printf(TEXT("NumberInset_%d"), Index), FVector(NumberBandX, Y, LayoutZ + 0.5f), FVector(InnerX * 0.14f, InnerY * 0.09f, LineThickness), FeltColor);

		FMadCrapsLabelSpec NumberLabel;
		NumberLabel.Text = Numbers[Index];
		NumberLabel.Location = FVector(NumberBandX, Y, LayoutZ + 2.0f);
		NumberLabel.Rotation = FRotator(90.0f, 0.0f, 0.0f);
		NumberLabel.Scale = 32.0f;
		AddLabel(NumberLabel);
	}

	AddBox(TEXT("FieldBox"), FVector(-InnerX * 0.08f, 0.0f, LayoutZ), FVector(InnerX * 0.24f, InnerY * 0.84f, LineThickness), LayoutLineColor);
	AddBox(TEXT("FieldInset"), FVector(-InnerX * 0.08f, 0.0f, LayoutZ + 0.5f), FVector(InnerX * 0.21f, InnerY * 0.81f, LineThickness), FeltColor);
	AddBox(TEXT("PropBox"), FVector(PropositionX, 0.0f, LayoutZ), FVector(InnerX * 0.16f, InnerY * 0.84f, LineThickness), LayoutLineColor);
	AddBox(TEXT("PropInset"), FVector(PropositionX, 0.0f, LayoutZ + 0.5f), FVector(InnerX * 0.13f, InnerY * 0.81f, LineThickness), FeltColor);

	FMadCrapsLabelSpec Label;
	Label.Rotation = FRotator(90.0f, 0.0f, 0.0f);

	Label.Text = TEXT("PASS LINE");
	Label.Location = FVector(-InnerX * 0.28f, 0.0f, LayoutZ + 2.0f);
	Label.Scale = 34.0f;
	AddLabel(Label);

	Label.Text = TEXT("COME");
	Label.Location = FVector(CenterX, 0.0f, LayoutZ + 2.0f);
	Label.Scale = 32.0f;
	AddLabel(Label);

	Label.Text = TEXT("FIELD");
	Label.Location = FVector(-InnerX * 0.08f, 0.0f, LayoutZ + 2.0f);
	Label.Scale = 28.0f;
	AddLabel(Label);

	Label.Text = TEXT("ANY 7");
	Label.Location = FVector(PropositionX, -InnerY * 0.22f, LayoutZ + 2.0f);
	Label.Scale = 24.0f;
	AddLabel(Label);

	Label.Text = TEXT("HARDWAYS");
	Label.Location = FVector(PropositionX, InnerY * 0.22f, LayoutZ + 2.0f);
	Label.Scale = 24.0f;
	AddLabel(Label);

	Label.Text = TEXT("DON'T PASS BAR");
	Label.Location = FVector(-InnerX * 0.38f, InnerY * 0.33f, LayoutZ + 2.0f);
	Label.Scale = 18.0f;
	AddLabel(Label);
}

void AMadCrapsTableActor::BuildChipStacks()
{
	const float InnerX = TableSize.X - (RailWidth * 2.0f);
	const float InnerY = TableSize.Y - (RailWidth * 2.0f);
	const float StartX = -InnerX * 0.42f;
	const float SideY = InnerY * 0.43f;
	const float ChipBaseZ = TableSize.Z + FeltThickness + (ChipHeight * 0.5f);
	const float SpacingX = InnerX * 0.16f;

	for (int32 SideIndex = 0; SideIndex < 2; ++SideIndex)
	{
		const float Y = SideIndex == 0 ? -SideY : SideY;
		for (int32 StackIndex = 0; StackIndex < ChipStacksPerSide; ++StackIndex)
		{
			const float X = StartX + (StackIndex * SpacingX);
			const FLinearColor StackColor = ChipColors.IsEmpty() ? FLinearColor::White : ChipColors[StackIndex % ChipColors.Num()];
			for (int32 ChipIndex = 0; ChipIndex < ChipsPerStack; ++ChipIndex)
			{
				const float Z = ChipBaseZ + (ChipIndex * ChipHeight);
				AddCylinder(FString::Printf(TEXT("Chip_%d_%d_%d"), SideIndex, StackIndex, ChipIndex), FVector(X, Y, Z), ChipRadius, ChipHeight, StackColor);
			}
		}
	}
}

void AMadCrapsTableActor::EnsureDiceActors()
{
	UWorld* World = GetWorld();
	if (!World || !DieActorClass)
	{
		return;
	}

	auto SpawnOrMoveDie = [&](TObjectPtr<AMadCrapsDieActor>& DieActor, const int32 DieIndex, const TCHAR* Name)
	{
		const FTransform SpawnTransform = GetDieSpawnTransform(DieIndex);
		if (!IsValid(DieActor))
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.Name = MakeUniqueObjectName(World, DieActorClass, Name);
			DieActor = World->SpawnActor<AMadCrapsDieActor>(DieActorClass, SpawnTransform, SpawnParameters);
			if (DieActor)
			{
				DieActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
		else
		{
			DieActor->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
		}
	};

	SpawnOrMoveDie(DieOneActor, 0, TEXT("DieOne"));
	SpawnOrMoveDie(DieTwoActor, 1, TEXT("DieTwo"));

	if (DiceRollCoordinator)
	{
		DiceRollCoordinator->DieOneActor = DieOneActor;
		DiceRollCoordinator->DieTwoActor = DieTwoActor;
	}
}

FTransform AMadCrapsTableActor::GetDieSpawnTransform(const int32 DieIndex) const
{
	const float InnerX = TableSize.X - (RailWidth * 2.0f);
	const float InnerY = TableSize.Y - (RailWidth * 2.0f);
	const FVector LocalLocation(
		InnerX * 0.16f,
		(DieIndex == 0 ? -1.0f : 1.0f) * InnerY * 0.08f,
		TableSize.Z + FeltThickness + 9.0f);

	return FTransform(FRotator::ZeroRotator, GetActorTransform().TransformPosition(LocalLocation));
}

bool AMadCrapsTableActor::ValidateBet(const FMadCrapsBet& Bet) const
{
	if (Bet.Amount <= 0.0)
	{
		return false;
	}

	switch (Bet.Type)
	{
	case EMadCrapsBetType::PassLine:
	case EMadCrapsBetType::Field:
		return Bet.Target == 0;

	case EMadCrapsBetType::Place:
		return IsValidPlaceNumber(Bet.Target);

	case EMadCrapsBetType::Hardway:
		return IsValidHardwayNumber(Bet.Target);

	default:
		return false;
	}
}

bool AMadCrapsTableActor::IsValidPlaceNumber(const int32 Number) const
{
	return Number == 4 || Number == 5 || Number == 6 || Number == 8 || Number == 9 || Number == 10;
}

bool AMadCrapsTableActor::IsValidHardwayNumber(const int32 Number) const
{
	return Number == 4 || Number == 6 || Number == 8 || Number == 10;
}

bool AMadCrapsTableActor::ShouldKeepBetAfterRoll(const FMadCrapsBet& Bet, const FMadCrapsPayout& Payout) const
{
	switch (Bet.Type)
	{
	case EMadCrapsBetType::Field:
		return false;

	case EMadCrapsBetType::PassLine:
		return Payout.Description == TEXT("PassLine point established") || Payout.Description == TEXT("No resolution");

	case EMadCrapsBetType::Place:
		return Payout.Description == TEXT("Place win") || Payout.Description == TEXT("No resolution");

	case EMadCrapsBetType::Hardway:
		return Payout.Description == TEXT("Hardway win") || Payout.Description == TEXT("No resolution");

	default:
		return false;
	}
}

int32 AMadCrapsTableActor::GetNextPointForRoll(const FMadCrapsRollResult& Roll) const
{
	const int32 Total = Roll.Die1 + Roll.Die2;
	if (CurrentPoint == 0)
	{
		return IsValidPlaceNumber(Total) ? Total : 0;
	}

	if (Total == CurrentPoint || Total == 7)
	{
		return 0;
	}

	return CurrentPoint;
}
