#pragma once

#include "CoreMinimal.h"
#include "Model/SeasonTypes.h"

namespace SeasonTest
{
	/** Five tiers, 500 XP apart (0, 500, 1000, 1500, 2000). Tier 3 unlocks "Cooking II". */
	inline TArray<FSeasonTierRow> MakeTiers()
	{
		TArray<FSeasonTierRow> Rows;
		for (int32 Tier = 1; Tier <= 5; ++Tier)
		{
			FSeasonTierRow& Row = Rows.AddDefaulted_GetRef();
			Row.TierIndex = Tier;
			Row.XPRequired = (Tier - 1) * 500;
			Row.FreeRewardName = FText::FromString(FString::Printf(TEXT("Free %d"), Tier));
			Row.PremiumRewardName = FText::FromString(FString::Printf(TEXT("Premium %d"), Tier));
		}
		Rows[2].UnlocksSkillNode = TEXT("Cooking II");
		Rows[4].Rarity = ESeasonRarity::Legendary;
		return Rows;
	}
}
