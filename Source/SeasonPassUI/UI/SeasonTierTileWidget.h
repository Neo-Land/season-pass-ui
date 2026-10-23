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

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> RarityBorder;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TierNumber;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentLabel;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FreeReward;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PremiumReward;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FreeLock;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FreeCheck;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PremiumLock;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PremiumCheck;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClaimFreeButton;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ClaimPremiumButton;

private:
	void ApplyTileBrush();

	FLinearColor RarityColor = FLinearColor(FColor(0x8A, 0x93, 0x9E));
	bool bIsCurrentTier = false;
};
