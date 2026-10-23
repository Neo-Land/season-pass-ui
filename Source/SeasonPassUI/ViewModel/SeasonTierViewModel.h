#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Model/SeasonTypes.h"
#include "SeasonTierViewModel.generated.h"

class USeasonPassViewModel;

/**
 * One tile on the tier track. Exposes ready-to-bind values so WBP_TierTile has no logic:
 * the Locked/Claimable/Claimed bools feed the built-in "To Visibility (Boolean)" conversion,
 * and RarityColor feeds the border tint directly.
 */
UCLASS(BlueprintType)
class SEASONPASSUI_API USeasonTierViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize(USeasonPassViewModel* InOwner, const FSeasonTierRow& Row);
	void Refresh(ETierState InFreeState, ETierState InPremiumState, bool bInIsCurrentTier);

	UFUNCTION(BlueprintCallable, Category = "Tier")
	void ClaimFree();

	UFUNCTION(BlueprintCallable, Category = "Tier")
	void ClaimPremium();

	int32 GetTierIndex() const { return TierIndex; }
	const FText& GetTierLabel() const { return TierLabel; }
	ETierState GetFreeState() const { return FreeState; }
	ETierState GetPremiumState() const { return PremiumState; }
	bool IsCurrentTier() const { return bIsCurrentTier; }
	bool IsFreeLocked() const { return bFreeLocked; }
	bool IsFreeClaimable() const { return bFreeClaimable; }
	bool IsFreeClaimed() const { return bFreeClaimed; }
	bool IsPremiumLocked() const { return bPremiumLocked; }
	bool IsPremiumClaimable() const { return bPremiumClaimable; }
	bool IsPremiumClaimed() const { return bPremiumClaimed; }
	FLinearColor GetDisplayColor() const { return RarityColor; }

	static FLinearColor GetRarityColor(ESeasonRarity InRarity);

private:
	TWeakObjectPtr<USeasonPassViewModel> Owner;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	int32 TierIndex = 0;

	/** TierIndex as display text, e.g. "12". */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	FText TierLabel;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	FText FreeRewardName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	FText PremiumRewardName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	ESeasonRarity Rarity = ESeasonRarity::Common;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	FLinearColor RarityColor = FLinearColor::Gray;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	ETierState FreeState = ETierState::Locked;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	ETierState PremiumState = ETierState::Locked;

	/** True for the tier matching the player's current level. Highlights the tile. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier", meta = (AllowPrivateAccess = "true"))
	bool bIsCurrentTier = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier|Free", meta = (AllowPrivateAccess = "true"))
	bool bFreeLocked = true;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier|Free", meta = (AllowPrivateAccess = "true"))
	bool bFreeClaimable = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier|Free", meta = (AllowPrivateAccess = "true"))
	bool bFreeClaimed = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier|Premium", meta = (AllowPrivateAccess = "true"))
	bool bPremiumLocked = true;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier|Premium", meta = (AllowPrivateAccess = "true"))
	bool bPremiumClaimable = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Tier|Premium", meta = (AllowPrivateAccess = "true"))
	bool bPremiumClaimed = false;
};
