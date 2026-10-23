#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FieldNotificationId.h"
#include "SeasonPassWidget.generated.h"

class UBorder;
class UButton;
class UImage;
class UListView;
class UProgressBar;
class UTextBlock;
class USeasonPassViewModel;
class UWidgetAnimation;

/**
 * Parent class for WBP_SeasonPass. Handles the few things MVVM bindings can't express:
 * filling ListViews with ViewModel arrays, playing the level-up animation, and easing the XP bar.
 * No game rules live here.
 */
UCLASS(Abstract)
class SEASONPASSUI_API USeasonPassWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Assigns the ViewModel to this widget's Manual slot and populates the lists. */
	void SetViewModel(USeasonPassViewModel* InViewModel);

protected:
	/** Applies the visual style (see SeasonPassStyle.h). Runs in the Designer too. */
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	/** Horizontal ListView whose entry class is WBP_TierTile. */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidget))
	TObjectPtr<UListView> TierList;

	/** Optional ListView whose entry class is WBP_SkillChip. */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UListView> SkillList;

	/** Optional. If present, C++ drives its Percent with easing, so don't bind Percent in View Bindings. */
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> XPBar;

	/** Optional scale + fade animation for the toast. The toast hides when it ends (or after 2 s without one). */
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> LevelUpAnim;

	// Optional parts that only receive styling.
	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UImage> Background;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Title;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> XPText;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Add100;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UButton> Add1000;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UButton> PremiumButton;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ResetButton;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> LevelUpToast;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelUpTextBlock;

	/** How fast the XP bar catches up to its target. Higher is snappier. */
	UPROPERTY(EditAnywhere, Category = "Season Pass")
	float XPBarInterpSpeed = 6.f;

	/** When the level changes, the tier list glides so the current tier sits this many tiles from the left edge. */
	UPROPERTY(EditAnywhere, Category = "Season Pass")
	int32 AutoScrollLeadTiles = 1;

	UPROPERTY(EditAnywhere, Category = "Season Pass")
	float AutoScrollInterpSpeed = 8.f;

private:
	void HandleShowLevelUpChanged(UObject* Source, UE::FieldNotification::FFieldId FieldId);
	void HandleToastFinished();
	void TickXPBar(float DeltaTime);
	void TickAutoScroll(float DeltaTime);

	/** Level the list last scrolled for. INDEX_NONE forces a scroll on the first tick. */
	int32 ScrolledForLevel = INDEX_NONE;
	float ScrollTarget = 0.f;
	bool bAutoScrolling = false;

	UPROPERTY(Transient)
	TObjectPtr<USeasonPassViewModel> ViewModel;

	FDelegateHandle ShowLevelUpHandle;
	FTimerHandle ToastTimer;
	float DisplayedXP = 0.f;
};
