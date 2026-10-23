#include "UI/SeasonPassWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Model/SeasonTypes.h"
#include "MVVMSubsystem.h"
#include "TimerManager.h"
#include "UI/SeasonPassStyle.h"
#include "View/MVVMView.h"
#include "ViewModel/SeasonPassViewModel.h"
#include "ViewModel/SeasonTierViewModel.h"
#include "ViewModel/SkillNodeViewModel.h"

void USeasonPassWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	using namespace SeasonPassStyle;

	if (Background)
	{
		Background->SetColorAndOpacity(Color::Backdrop);
	}

	StyleText(Title, 40.f, TEXT("Bold"), Color::Text, 150);
	StyleText(LevelText, 24.f, TEXT("Bold"), Color::Gold, 60);
	StyleText(XPText, 16.f, TEXT("Bold"), Color::TextMuted);

	if (XPBar)
	{
		FProgressBarStyle BarStyle;
		BarStyle.SetBackgroundImage(RoundedBrush(Color::RaisedLo, 12.f, Color::Stroke, 1.f));
		BarStyle.SetFillImage(RoundedBrush(Color::Gold, 12.f));
		BarStyle.SetMarqueeImage(RoundedBrush(Color::Gold, 12.f));
		XPBar->SetWidgetStyle(BarStyle);
		XPBar->SetFillColorAndOpacity(FLinearColor::White);
	}

	const FButtonStyle DebugButton = MakeButtonStyle(Color::Raised, Color::RaisedHi, Color::RaisedLo, 8.f, Color::Stroke, Color::Gold);
	for (UButton* Button : { Add100.Get(), Add1000.Get(), PremiumButton.Get(), ResetButton.Get() })
	{
		StyleButton(Button, DebugButton, 14.f, Color::Text);
	}

	if (LevelUpToast)
	{
		LevelUpToast->SetBrush(RoundedBrush(Color::Gold, 18.f, Color::GoldPale, 3.f));
		LevelUpToast->SetBrushColor(FLinearColor::White);
		LevelUpToast->SetPadding(FMargin(36.f, 18.f));
	}
	StyleText(LevelUpTextBlock, 44.f, TEXT("Bold"), Color::Ink, 100);
}

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
	ScrolledForLevel = INDEX_NONE;

	ShowLevelUpHandle = ViewModel->AddFieldValueChangedDelegate(
		USeasonPassViewModel::FFieldNotificationClassDescriptor::bShowLevelUp,
		INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &ThisClass::HandleShowLevelUpChanged));
}

void USeasonPassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ViewModel)
	{
		TickXPBar(InDeltaTime);
		TickAutoScroll(InDeltaTime);
	}
}

void USeasonPassWidget::TickAutoScroll(float DeltaTime)
{
	if (!TierList)
	{
		return;
	}

	// Only glide when the level changes, so manual scrolling isn't fought every frame.
	const int32 Level = ViewModel->GetCurrentLevel();
	if (Level != ScrolledForLevel)
	{
		ScrolledForLevel = Level;
		ScrollTarget = FMath::Max(0.f, float(Level - 1 - AutoScrollLeadTiles));
		bAutoScrolling = true;
	}

	if (bAutoScrolling)
	{
		const float Offset = FMath::FInterpTo(TierList->GetScrollOffset(), ScrollTarget, DeltaTime, AutoScrollInterpSpeed);
		TierList->SetScrollOffset(Offset);
		bAutoScrolling = !FMath::IsNearlyEqual(Offset, ScrollTarget, 0.01f);
	}
}

void USeasonPassWidget::TickXPBar(float DeltaTime)
{
	if (!XPBar)
	{
		return;
	}

	const float Target = ViewModel->GetXPProgress();
	if (Target < DisplayedXP)
	{
		// Wrapped to a new level (or reset): refill from empty instead of draining backwards.
		DisplayedXP = 0.f;
	}
	DisplayedXP = FMath::FInterpTo(DisplayedXP, Target, DeltaTime, XPBarInterpSpeed);
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
