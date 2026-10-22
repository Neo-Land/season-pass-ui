#include "SeasonPassGameMode.h"

#include "GameFramework/SpectatorPawn.h"
#include "UI/SeasonPassHUD.h"

ASeasonPassGameMode::ASeasonPassGameMode()
{
	HUDClass = ASeasonPassHUD::StaticClass();
	DefaultPawnClass = ASpectatorPawn::StaticClass();
}
