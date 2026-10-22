#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Model/SeasonTypes.h"
#include "SeasonProgressSubsystem.generated.h"

class UDataTable;

DECLARE_MULTICAST_DELEGATE(FOnSeasonProgressChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSeasonLevelUp, int32 /*NewLevel*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSeasonTierClaimed, int32 /*Tier*/, bool /*bPremium*/);

/**
 * The Model. Owns all season-pass state and rules; knows nothing about UI.
 * The ViewModel layer reads from it and listens to its events.
 */
UCLASS()
class SEASONPASSUI_API USeasonProgressSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Asset the tier rows are loaded from at startup. */
	static const TCHAR* TierTablePath;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Season Pass")
	void AddXP(int32 Amount);

	/** Highest tier whose XPRequired has been reached (0 if no tiers are loaded). */
	UFUNCTION(BlueprintPure, Category = "Season Pass")
	int32 GetCurrentLevel() const;

	/** 0..1 progress from the current level to the next. 1 at max level. */
	UFUNCTION(BlueprintPure, Category = "Season Pass")
	float GetProgressToNext() const;

	UFUNCTION(BlueprintPure, Category = "Season Pass")
	ETierState GetTierState(int32 Tier, bool bPremium) const;

	/** Returns false if the reward is not currently claimable. */
	UFUNCTION(BlueprintCallable, Category = "Season Pass")
	bool ClaimTier(int32 Tier, bool bPremium);

	UFUNCTION(BlueprintCallable, Category = "Season Pass")
	void SetPremium(bool bInHasPremium);

	UFUNCTION(BlueprintCallable, Category = "Season Pass")
	void ResetProgress();

	UFUNCTION(BlueprintPure, Category = "Season Pass")
	int32 GetTotalXP() const { return TotalXP; }

	UFUNCTION(BlueprintPure, Category = "Season Pass")
	bool HasPremium() const { return bHasPremium; }

	UFUNCTION(BlueprintPure, Category = "Season Pass")
	int32 GetMaxLevel() const;

	/** Cumulative XP needed to reach Tier, or INDEX_NONE if the tier doesn't exist. */
	UFUNCTION(BlueprintPure, Category = "Season Pass")
	int32 GetXPRequiredForTier(int32 Tier) const;

	UFUNCTION(BlueprintPure, Category = "Season Pass")
	bool IsSkillUnlocked(FName NodeId) const { return UnlockedSkills.Contains(NodeId); }

	/** Tier rows sorted by TierIndex. */
	const TArray<FSeasonTierRow>& GetTierRows() const { return Tiers; }

	const FSeasonTierRow* FindTier(int32 Tier) const;

	/** Replaces the tier data and resets progress. Lets tests run without the DataTable asset. */
	void SetTierRowsForTest(TArray<FSeasonTierRow> Rows);

	FOnSeasonProgressChanged OnProgressChanged;
	FOnSeasonLevelUp OnLevelUp;
	FOnSeasonTierClaimed OnTierClaimed;

private:
	void SetTierRows(TArray<FSeasonTierRow> Rows);

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> TierTable;

	TArray<FSeasonTierRow> Tiers;

	int32 TotalXP = 0;
	bool bHasPremium = false;
	TSet<int32> ClaimedFree;
	TSet<int32> ClaimedPremium;
	TSet<FName> UnlockedSkills;
};
