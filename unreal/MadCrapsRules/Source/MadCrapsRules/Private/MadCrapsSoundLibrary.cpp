#include "MadCrapsSoundLibrary.h"

namespace
{
	static const FMadCrapsSoundSlot* FindSoundSlot(const UMadCrapsSoundProfile* Profile, EMadCrapsSoundCueType CueType)
	{
		if (Profile == nullptr)
		{
			return nullptr;
		}

		for (const FMadCrapsSoundSlot& Slot : Profile->SoundSlots)
		{
			if (Slot.CueType == CueType)
			{
				return &Slot;
			}
		}

		return nullptr;
	}

	static int32 PickWeightedVariantIndex(const TArray<FMadCrapsSoundVariant>& Variants)
	{
		float TotalWeight = 0.0f;
		for (const FMadCrapsSoundVariant& Variant : Variants)
		{
			TotalWeight += FMath::Max(0.0f, Variant.Weight);
		}

		if (TotalWeight <= KINDA_SMALL_NUMBER)
		{
			return INDEX_NONE;
		}

		float Roll = FMath::FRandRange(0.0f, TotalWeight);
		for (int32 Index = 0; Index < Variants.Num(); ++Index)
		{
			Roll -= FMath::Max(0.0f, Variants[Index].Weight);
			if (Roll <= 0.0f)
			{
				return Index;
			}
		}

		return Variants.Num() - 1;
	}
}

bool UMadCrapsSoundLibrary::ResolveSoundCue(const UMadCrapsSoundProfile* Profile, EMadCrapsSoundCueType CueType, FMadCrapsResolvedSound& ResolvedSound)
{
	ResolvedSound = FMadCrapsResolvedSound{};

	const FMadCrapsSoundSlot* Slot = FindSoundSlot(Profile, CueType);
	if (Slot == nullptr || Slot->Variants.IsEmpty())
	{
		return false;
	}

	const int32 VariantIndex = PickWeightedVariantIndex(Slot->Variants);
	if (!Slot->Variants.IsValidIndex(VariantIndex))
	{
		return false;
	}

	const FMadCrapsSoundVariant& Variant = Slot->Variants[VariantIndex];
	if (Variant.Sound == nullptr)
	{
		return false;
	}

	ResolvedSound.Sound = Variant.Sound;
	ResolvedSound.VolumeMultiplier = Variant.VolumeMultiplier;
	ResolvedSound.PitchMultiplier = FMath::FRandRange(Variant.PitchMin, Variant.PitchMax);
	return true;
}
