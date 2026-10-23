#pragma once

#include "CoreMinimal.h"
#include "UI/SeasonListEntryWidget.h"
#include "SeasonTierTileWidget.generated.h"

class UBorder;
class UButton;
class UTextBlock;

/**
 * Parent class for WBP_TierTile. Applies the tile's visual style.
 * SetRarityColor and SetIsCurrentTier are view functions: MVVM binds the ViewModel's
 * RarityColor and bIsCurrentTier to them, and they decide how those values look.
 */
UCLASS(Abstract)
class SEASONPASSUI_API USeasonTierTileWidget : public USeasonListEntryWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Tier Tile")
	void SetRarityColor(FLinearColor InRarityColor);

	UFUNCTION(BlueprintCallable, Category = "Tier Tile")
	void SetIsCurrentTier(bool bInIsCurrentTier);

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> RarityBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TierNumber;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentLabel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FreeReward;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PremiumReward;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FreeLock;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FreeCheck;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PremiumLock;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PremiumCheck;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClaimFreeButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClaimPremiumButton;

private:
	void ApplyTileBrush();

	FLinearColor RarityColor = FLinearColor(FColor(0x8A, 0x93, 0x9E));
	bool bIsCurrentTier = false;
};
