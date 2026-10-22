#include "UI/SeasonPassHUD.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Model/SeasonProgressSubsystem.h"
#include "UI/SeasonPassWidget.h"
#include "ViewModel/SeasonPassViewModel.h"

ASeasonPassHUD::ASeasonPassHUD()
{
	SeasonPassWidgetClass = TSoftClassPtr<USeasonPassWidget>(FSoftObjectPath(TEXT("/Game/UI/WBP_SeasonPass.WBP_SeasonPass_C")));
}

void ASeasonPassHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	USeasonProgressSubsystem* Progress = GetGameInstance() ? GetGameInstance()->GetSubsystem<USeasonProgressSubsystem>() : nullptr;
	if (!PC || !Progress)
	{
		return;
	}

	TSubclassOf<USeasonPassWidget> WidgetClass = SeasonPassWidgetClass.LoadSynchronous();
	if (!WidgetClass)
	{
		UE_LOG(LogSeasonPass, Warning, TEXT("Season pass widget not found at %s. Create WBP_SeasonPass in /Game/UI with parent class SeasonPassWidget."),
			*SeasonPassWidgetClass.ToString());
		return;
	}

	ViewModel = NewObject<USeasonPassViewModel>(this);
	ViewModel->Initialize(Progress);

	SeasonPassWidget = CreateWidget<USeasonPassWidget>(PC, WidgetClass);
	SeasonPassWidget->SetViewModel(ViewModel);
	SeasonPassWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(SeasonPassWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}
