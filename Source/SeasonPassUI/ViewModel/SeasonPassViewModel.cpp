#include "ViewModel/SeasonPassViewModel.h"

#include "Model/SeasonProgressSubsystem.h"
#include "ViewModel/SeasonTierViewModel.h"
#include "ViewModel/SkillNodeViewModel.h"

#define LOCTEXT_NAMESPACE "SeasonPass"

void USeasonPassViewModel::Initialize(USeasonProgressSubsystem* InProgress)
{
	if (!ensure(InProgress))
	{
		return;
	}
	Progress = InProgress;

	Tiers.Reset();
	SkillNodes.Reset();
	for (const FSeasonTierRow& Row : InProgress->GetTierRows())
	{
		USeasonTierViewModel* TierVM = NewObject<USeasonTierViewModel>(this);
		TierVM->Initialize(this, Row);
		Tiers.Add(TierVM);

		if (!Row.UnlocksSkillNode.IsNone())
		{
			USkillNodeViewModel* SkillVM = NewObject<USkillNodeViewModel>(this);
			SkillVM->Initialize(Row.UnlocksSkillNode);
			SkillNodes.Add(SkillVM);
		}
	}

	InProgress->OnProgressChanged.AddUObject(this, &ThisClass::RefreshAll);
	InProgress->OnTierClaimed.AddWeakLambda(this, [this](int32, bool) { RefreshAll(); });
	InProgress->OnLevelUp.AddUObject(this, &ThisClass::HandleLevelUp);

	RefreshAll();
}

void USeasonPassViewModel::RefreshAll()
{
	const USeasonProgressSubsystem* Model = Progress.Get();
	if (!Model)
	{
		return;
	}

	const int32 Level = Model->GetCurrentLevel();
	const int32 NextXP = Model->GetXPRequiredForTier(Level + 1);

	UE_MVVM_SET_PROPERTY_VALUE(CurrentLevel, Level);
	UE_MVVM_SET_PROPERTY_VALUE(LevelLabel, FText::Format(LOCTEXT("LevelLabel", "LEVEL {0}"), Level));
	UE_MVVM_SET_PROPERTY_VALUE(XPProgress, Model->GetProgressToNext());
	UE_MVVM_SET_PROPERTY_VALUE(XPLabel, NextXP == INDEX_NONE
		? FText::Format(LOCTEXT("XPLabelMax", "{0} XP  MAX"), FText::AsNumber(Model->GetTotalXP()))
		: FText::Format(LOCTEXT("XPLabel", "{0} / {1} XP"), FText::AsNumber(Model->GetTotalXP()), FText::AsNumber(NextXP)));
	UE_MVVM_SET_PROPERTY_VALUE(bHasPremium, Model->HasPremium());
	UE_MVVM_SET_PROPERTY_VALUE(PremiumLabel, Model->HasPremium()
		? LOCTEXT("PremiumOn", "Premium: ON")
		: LOCTEXT("PremiumOff", "Premium: OFF"));

	for (USeasonTierViewModel* TierVM : Tiers)
	{
		const int32 Tier = TierVM->GetTierIndex();
		TierVM->Refresh(Model->GetTierState(Tier, false), Model->GetTierState(Tier, true), Tier == Level);
	}

	for (USkillNodeViewModel* SkillVM : SkillNodes)
	{
		SkillVM->SetUnlocked(Model->IsSkillUnlocked(SkillVM->GetNodeId()));
	}
}

void USeasonPassViewModel::HandleLevelUp(int32 NewLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(LevelUpText, FText::Format(LOCTEXT("LevelUp", "LEVEL UP! {0}"), NewLevel));

	// If the toast is already showing, re-broadcast so the view replays it for the new level.
	if (!UE_MVVM_SET_PROPERTY_VALUE(bShowLevelUp, true))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(bShowLevelUp);
	}
}

void USeasonPassViewModel::ClaimTier(int32 Tier, bool bPremium)
{
	if (USeasonProgressSubsystem* Model = Progress.Get())
	{
		Model->ClaimTier(Tier, bPremium);
	}
}

void USeasonPassViewModel::DebugAddXP(int32 Amount)
{
	if (USeasonProgressSubsystem* Model = Progress.Get())
	{
		Model->AddXP(Amount);
	}
}

void USeasonPassViewModel::DebugTogglePremium()
{
	if (USeasonProgressSubsystem* Model = Progress.Get())
	{
		Model->SetPremium(!Model->HasPremium());
	}
}

void USeasonPassViewModel::DebugReset()
{
	DismissLevelUp();
	if (USeasonProgressSubsystem* Model = Progress.Get())
	{
		Model->ResetProgress();
	}
}

void USeasonPassViewModel::DismissLevelUp()
{
	UE_MVVM_SET_PROPERTY_VALUE(bShowLevelUp, false);
}

#undef LOCTEXT_NAMESPACE
