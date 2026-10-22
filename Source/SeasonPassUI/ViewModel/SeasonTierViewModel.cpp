#include "ViewModel/SeasonTierViewModel.h"

#include "ViewModel/SeasonPassViewModel.h"

void USeasonTierViewModel::Initialize(USeasonPassViewModel* InOwner, const FSeasonTierRow& Row)
{
	Owner = InOwner;
	UE_MVVM_SET_PROPERTY_VALUE(TierIndex, Row.TierIndex);
	UE_MVVM_SET_PROPERTY_VALUE(TierLabel, FText::AsNumber(Row.TierIndex));
	UE_MVVM_SET_PROPERTY_VALUE(FreeRewardName, Row.FreeRewardName);
	UE_MVVM_SET_PROPERTY_VALUE(PremiumRewardName, Row.PremiumRewardName);
	UE_MVVM_SET_PROPERTY_VALUE(Rarity, Row.Rarity);
	UE_MVVM_SET_PROPERTY_VALUE(RarityColor, GetRarityColor(Row.Rarity));
}

void USeasonTierViewModel::Refresh(ETierState InFreeState, ETierState InPremiumState, bool bInIsCurrentTier)
{
	UE_MVVM_SET_PROPERTY_VALUE(FreeState, InFreeState);
	UE_MVVM_SET_PROPERTY_VALUE(PremiumState, InPremiumState);
	UE_MVVM_SET_PROPERTY_VALUE(bIsCurrentTier, bInIsCurrentTier);

	UE_MVVM_SET_PROPERTY_VALUE(bFreeLocked, InFreeState == ETierState::Locked);
	UE_MVVM_SET_PROPERTY_VALUE(bFreeClaimable, InFreeState == ETierState::Claimable);
	UE_MVVM_SET_PROPERTY_VALUE(bFreeClaimed, InFreeState == ETierState::Claimed);

	UE_MVVM_SET_PROPERTY_VALUE(bPremiumLocked, InPremiumState == ETierState::Locked);
	UE_MVVM_SET_PROPERTY_VALUE(bPremiumClaimable, InPremiumState == ETierState::Claimable);
	UE_MVVM_SET_PROPERTY_VALUE(bPremiumClaimed, InPremiumState == ETierState::Claimed);
}

void USeasonTierViewModel::ClaimFree()
{
	if (USeasonPassViewModel* Pass = Owner.Get())
	{
		Pass->ClaimTier(TierIndex, false);
	}
}

void USeasonTierViewModel::ClaimPremium()
{
	if (USeasonPassViewModel* Pass = Owner.Get())
	{
		Pass->ClaimTier(TierIndex, true);
	}
}

FLinearColor USeasonTierViewModel::GetRarityColor(ESeasonRarity InRarity)
{
	switch (InRarity)
	{
	case ESeasonRarity::Rare:      return FLinearColor(FColor(0x2F, 0x8C, 0xFF));
	case ESeasonRarity::Epic:      return FLinearColor(FColor(0xA3, 0x4D, 0xF5));
	case ESeasonRarity::Legendary: return FLinearColor(FColor(0xF5, 0x9E, 0x2B));
	case ESeasonRarity::Common:
	default:                       return FLinearColor(FColor(0x8A, 0x93, 0x9E));
	}
}
