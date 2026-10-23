#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SeasonSaveGame.generated.h"

/** Persisted season progress. Written by USeasonProgressSubsystem after every change. */
UCLASS()
class SEASONPASSUI_API USeasonSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 TotalXP = 0;

	UPROPERTY()
	bool bHasPremium = false;

	UPROPERTY()
	TSet<int32> ClaimedFree;

	UPROPERTY()
	TSet<int32> ClaimedPremium;

	UPROPERTY()
	TSet<FName> UnlockedSkills;
};
