#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SeasonPassHUD.generated.h"

class USeasonPassViewModel;
class USeasonPassWidget;

/**
 * Composition root: wires Model -> ViewModel -> View at BeginPlay.
 * Creates the ViewModel from the game-instance subsystem, creates WBP_SeasonPass,
 * hands it the ViewModel, and switches input to UI.
 */
UCLASS()
class SEASONPASSUI_API ASeasonPassHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASeasonPassHUD();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Season Pass")
	TSoftClassPtr<USeasonPassWidget> SeasonPassWidgetClass;

private:
	UPROPERTY(Transient)
	TObjectPtr<USeasonPassViewModel> ViewModel;

	UPROPERTY(Transient)
	TObjectPtr<USeasonPassWidget> SeasonPassWidget;
};
