#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SeasonPassGameMode.generated.h"

/** Default game mode (set in DefaultEngine.ini). Uses ASeasonPassHUD to show the season pass screen. */
UCLASS()
class SEASONPASSUI_API ASeasonPassGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASeasonPassGameMode();
};
