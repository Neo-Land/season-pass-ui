#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SeasonPassViewModel.generated.h"

class USeasonProgressSubsystem;
class USeasonTierViewModel;
class USkillNodeViewModel;

/**
 * The ViewModel for the whole season pass screen.
 * Translates subsystem state into display-ready FieldNotify properties and forwards
 * user intents (claim, debug buttons) back to the subsystem. Holds no rules of its own.
 *
 * Properties are BlueprintReadOnly: data flows one way (Model -> ViewModel -> View),
 * and the View can only change state by calling the functions below.
 */
UCLASS(BlueprintType)
class SEASONPASSUI_API USeasonPassViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Builds the tier and skill ViewModels, subscribes to the subsystem's events, and refreshes. */
	void Initialize(USeasonProgressSubsystem* InProgress);

	UFUNCTION(BlueprintCallable, Category = "Season Pass")
	void ClaimTier(int32 Tier, bool bPremium);

	UFUNCTION(BlueprintCallable, Category = "Season Pass|Debug")
	void DebugAddXP(int32 Amount);

	/** No-argument versions so buttons can bind to them directly. */
	UFUNCTION(BlueprintCallable, Category = "Season Pass|Debug")
	void DebugAdd100XP() { DebugAddXP(100); }

	UFUNCTION(BlueprintCallable, Category = "Season Pass|Debug")
	void DebugAdd1000XP() { DebugAddXP(1000); }

	UFUNCTION(BlueprintCallable, Category = "Season Pass|Debug")
	void DebugTogglePremium();

	UFUNCTION(BlueprintCallable, Category = "Season Pass|Debug")
	void DebugReset();

	UFUNCTION(BlueprintCallable, Category = "Season Pass")
	void DismissLevelUp();

	const TArray<TObjectPtr<USeasonTierViewModel>>& GetTiers() const { return Tiers; }
	const TArray<TObjectPtr<USkillNodeViewModel>>& GetSkillNodes() const { return SkillNodes; }
	int32 GetCurrentLevel() const { return CurrentLevel; }
	const FText& GetLevelLabel() const { return LevelLabel; }
	float GetXPProgress() const { return XPProgress; }
	const FText& GetXPLabel() const { return XPLabel; }
	bool HasPremium() const { return bHasPremium; }
	const FText& GetPremiumLabel() const { return PremiumLabel; }
	const FText& GetLevelUpText() const { return LevelUpText; }
	bool ShouldShowLevelUp() const { return bShowLevelUp; }

private:
	void RefreshAll();
	void HandleLevelUp(int32 NewLevel);

	TWeakObjectPtr<USeasonProgressSubsystem> Progress;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USeasonTierViewModel>> Tiers;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USkillNodeViewModel>> SkillNodes;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	int32 CurrentLevel = 0;

	/** "LEVEL 7" */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	FText LevelLabel;

	/** 0..1 progress to the next level. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	float XPProgress = 0.f;

	/** "1,250 / 1,380 XP", or "10,260 XP  MAX" at the last tier. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	FText XPLabel;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	bool bHasPremium = false;

	/** "Premium: ON" / "Premium: OFF" for the debug toggle button. */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	FText PremiumLabel;

	/** "LEVEL UP! 8" */
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	FText LevelUpText;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Season Pass", meta = (AllowPrivateAccess = "true"))
	bool bShowLevelUp = false;
};
