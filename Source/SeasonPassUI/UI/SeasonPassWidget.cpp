#include "UI/SeasonPassWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/ListView.h"
#include "Components/ProgressBar.h"
#include "Model/SeasonTypes.h"
#include "MVVMSubsystem.h"
#include "TimerManager.h"
#include "View/MVVMView.h"
#include "ViewModel/SeasonPassViewModel.h"
#include "ViewModel/SeasonTierViewModel.h"
#include "ViewModel/SkillNodeViewModel.h"

void USeasonPassWidget::SetViewModel(USeasonPassViewModel* InViewModel)
{
	if (ViewModel && ShowLevelUpHandle.IsValid())
	{
		ViewModel->RemoveFieldValueChangedDelegate(USeasonPassViewModel::FFieldNotificationClassDescriptor::bShowLevelUp, ShowLevelUpHandle);
		ShowLevelUpHandle.Reset();
	}

	ViewModel = InViewModel;
	if (!ViewModel)
	{
		return;
	}

	UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(this);
	if (!View || !View->SetViewModelByClass(ViewModel))
	{
		UE_LOG(LogSeasonPass, Warning, TEXT("%s: add a SeasonPassViewModel slot (Creation Type: Manual) in View Bindings."), *GetName());
	}

	if (TierList)
	{
		TierList->SetListItems(ViewModel->GetTiers());
	}
	if (SkillList)
	{
		SkillList->SetListItems(ViewModel->GetSkillNodes());
	}

	DisplayedXP = ViewModel->GetXPProgress();

	ShowLevelUpHandle = ViewModel->AddFieldValueChangedDelegate(
		USeasonPassViewModel::FFieldNotificationClassDescriptor::bShowLevelUp,
		INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &ThisClass::HandleShowLevelUpChanged));
}

void USeasonPassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!XPBar || !ViewModel)
	{
		return;
	}

	const float Target = ViewModel->GetXPProgress();
	if (Target < DisplayedXP)
	{
		// Wrapped to a new level (or reset): refill from empty instead of draining backwards.
		DisplayedXP = 0.f;
	}
	DisplayedXP = FMath::FInterpTo(DisplayedXP, Target, InDeltaTime, XPBarInterpSpeed);
	XPBar->SetPercent(DisplayedXP);
}

void USeasonPassWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ToastTimer);
	}
	SetViewModel(nullptr);
	Super::NativeDestruct();
}

void USeasonPassWidget::HandleShowLevelUpChanged(UObject* Source, UE::FieldNotification::FFieldId FieldId)
{
	UWorld* World = GetWorld();
	if (!ViewModel || !World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(ToastTimer);

	// The ViewModel re-broadcasts while already showing to replay the toast for a new level.
	const bool bShowing = ViewModel->ShouldShowLevelUp();
	if (!bShowing)
	{
		if (LevelUpAnim)
		{
			StopAnimation(LevelUpAnim);
		}
		return;
	}

	float Duration = 2.f;
	if (LevelUpAnim)
	{
		PlayAnimation(LevelUpAnim);
		Duration = FMath::Max(LevelUpAnim->GetEndTime(), 0.1f);
	}
	World->GetTimerManager().SetTimer(ToastTimer, this, &ThisClass::HandleToastFinished, Duration, false);
}

void USeasonPassWidget::HandleToastFinished()
{
	if (ViewModel)
	{
		ViewModel->DismissLevelUp();
	}
}
