#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SeasonTypes.generated.h"

SEASONPASSUI_API DECLARE_LOG_CATEGORY_EXTERN(LogSeasonPass, Log, All);

/** Reward rarity. Drives tile color in the ViewModel layer. */
UENUM(BlueprintType)
enum class ESeasonRarity : uint8
{
	Common,
	Rare,
	Epic,
	Legendary
};

/** Claim state of one reward (free or premium) on one tier. */
UENUM(BlueprintType)
enum class ETierState : uint8
{
	Locked,
	Claimable,
	Claimed
};

/** One row of DT_SeasonTiers. Imported from Content/Data/SeasonTiers.csv. */
USTRUCT(BlueprintType)
struct SEASONPASSUI_API FSeasonTierRow : public FTableRowBase
{
	GENERATED_BODY()

	/** 1-based tier number. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Season")
	int32 TierIndex = 0;

	/** Cumulative XP needed to reach this tier. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Season")
	int32 XPRequired = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Season")
	FText FreeRewardName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Season")
	FText PremiumRewardName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Season")
	ESeasonRarity Rarity = ESeasonRarity::Common;

	/** Skill node unlocked by claiming this tier, or NAME_None. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Season")
	FName UnlocksSkillNode = NAME_None;
};
