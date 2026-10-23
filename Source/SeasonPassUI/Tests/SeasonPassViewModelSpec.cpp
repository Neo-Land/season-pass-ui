#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "Model/SeasonProgressSubsystem.h"
#include "Tests/SeasonTestData.h"
#include "UObject/StrongObjectPtr.h"
#include "ViewModel/SeasonPassViewModel.h"
#include "ViewModel/SeasonTierViewModel.h"
#include "ViewModel/SkillNodeViewModel.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * Tests the ViewModel layer: that model state is translated into the right display values,
 * that intents are forwarded, and that FieldNotify only fires when a value actually changes.
 */
BEGIN_DEFINE_SPEC(FSeasonPassViewModelSpec, "SeasonPass.ViewModel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

	TStrongObjectPtr<UGameInstance> GameInstance;
	TStrongObjectPtr<USeasonProgressSubsystem> Progress;
	TStrongObjectPtr<USeasonPassViewModel> ViewModel;

	USeasonTierViewModel* Tier(int32 TierIndex) const { return ViewModel->GetTiers()[TierIndex - 1]; }

	/** Counts FieldNotify broadcasts for one field. */
	int32* CountNotifications(UE::FieldNotification::FFieldId FieldId)
	{
		int32* Count = new int32(0);
		Counters.Emplace(Count);
		ViewModel->AddFieldValueChangedDelegate(FieldId,
			INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateLambda([Count](UObject*, UE::FieldNotification::FFieldId) { ++*Count; }));
		return Count;
	}
	TArray<TUniquePtr<int32>> Counters;

END_DEFINE_SPEC(FSeasonPassViewModelSpec)

void FSeasonPassViewModelSpec::Define()
{
	BeforeEach([this]()
	{
		GameInstance.Reset(NewObject<UGameInstance>());
		Progress.Reset(NewObject<USeasonProgressSubsystem>(GameInstance.Get()));
		Progress->SetTierRowsForTest(SeasonTest::MakeTiers());
		ViewModel.Reset(NewObject<USeasonPassViewModel>());
		ViewModel->Initialize(Progress.Get());
	});

	AfterEach([this]()
	{
		ViewModel.Reset();
		Progress.Reset();
		GameInstance.Reset();
		Counters.Reset();
	});

	Describe("Initialization", [this]()
	{
		It("builds one tier ViewModel per tier and one skill ViewModel per skill node", [this]()
		{
			TestEqual("Tiers", ViewModel->GetTiers().Num(), 5);
			TestEqual("Skill nodes", ViewModel->GetSkillNodes().Num(), 1);
			TestEqual("Skill name", ViewModel->GetSkillNodes()[0]->GetDisplayName().ToString(), FString(TEXT("Cooking II")));
		});

		It("exposes display-ready labels for a fresh season", [this]()
		{
			TestEqual("Level", ViewModel->GetCurrentLevel(), 1);
			TestEqual("Level label", ViewModel->GetLevelLabel().ToString(), FString(TEXT("LEVEL 1")));
			TestEqual("XP label", ViewModel->GetXPLabel().ToString(),
				FString::Printf(TEXT("0 / %s XP"), *FText::AsNumber(500).ToString()));
			TestEqual("Premium label", ViewModel->GetPremiumLabel().ToString(), FString(TEXT("Premium: OFF")));
			TestFalse("Toast hidden", ViewModel->ShouldShowLevelUp());
		});

		It("maps tier state to exactly one visible state per track", [this]()
		{
			TestTrue("Tier 1 free claimable", Tier(1)->IsFreeClaimable());
			TestFalse("Tier 1 free locked", Tier(1)->IsFreeLocked());
			TestFalse("Tier 1 free claimed", Tier(1)->IsFreeClaimed());
			TestTrue("Tier 1 premium locked", Tier(1)->IsPremiumLocked());
			TestTrue("Tier 2 free locked", Tier(2)->IsFreeLocked());
		});

		It("marks only the current tier", [this]()
		{
			TestTrue("Tier 1 current", Tier(1)->IsCurrentTier());
			TestFalse("Tier 2 current", Tier(2)->IsCurrentTier());
		});

		It("computes rarity color in the ViewModel", [this]()
		{
			TestEqual("Legendary color", Tier(5)->GetDisplayColor(), USeasonTierViewModel::GetRarityColor(ESeasonRarity::Legendary));
		});
	});

	Describe("Reacting to the model", [this]()
	{
		It("updates level, labels and the current tier when XP is added", [this]()
		{
			ViewModel->DebugAddXP(1000);

			TestEqual("Level", ViewModel->GetCurrentLevel(), 3);
			TestEqual("Level label", ViewModel->GetLevelLabel().ToString(), FString(TEXT("LEVEL 3")));
			TestEqual("XP label", ViewModel->GetXPLabel().ToString(),
				FString::Printf(TEXT("%s / %s XP"), *FText::AsNumber(1000).ToString(), *FText::AsNumber(1500).ToString()));
			TestFalse("Tier 1 no longer current", Tier(1)->IsCurrentTier());
			TestTrue("Tier 3 current", Tier(3)->IsCurrentTier());
			TestTrue("Tier 3 free claimable", Tier(3)->IsFreeClaimable());
		});

		It("shows MAX in the XP label at the last tier", [this]()
		{
			ViewModel->DebugAddXP(5000);
			TestTrue("XP label says MAX", ViewModel->GetXPLabel().ToString().Contains(TEXT("MAX")));
			TestEqual("Progress full", ViewModel->GetXPProgress(), 1.f);
		});

		It("claims through the tier ViewModel and updates its state", [this]()
		{
			Tier(1)->ClaimFree();
			TestTrue("Tier 1 free claimed", Tier(1)->IsFreeClaimed());
			TestFalse("Tier 1 free claimable", Tier(1)->IsFreeClaimable());
		});

		It("unlocks the skill node ViewModel when its tier is claimed", [this]()
		{
			ViewModel->DebugAddXP(1000);
			TestFalse("Locked before", ViewModel->GetSkillNodes()[0]->IsUnlocked());
			Tier(3)->ClaimFree();
			TestTrue("Unlocked after", ViewModel->GetSkillNodes()[0]->IsUnlocked());
		});

		It("opens premium rewards when premium is toggled", [this]()
		{
			ViewModel->DebugTogglePremium();
			TestTrue("Has premium", ViewModel->HasPremium());
			TestEqual("Premium label", ViewModel->GetPremiumLabel().ToString(), FString(TEXT("Premium: ON")));
			TestTrue("Tier 1 premium claimable", Tier(1)->IsPremiumClaimable());
		});

		It("resets everything through DebugReset", [this]()
		{
			ViewModel->DebugAddXP(1000);
			ViewModel->DebugTogglePremium();
			Tier(1)->ClaimFree();
			ViewModel->DebugReset();

			TestEqual("Level", ViewModel->GetCurrentLevel(), 1);
			TestFalse("Premium", ViewModel->HasPremium());
			TestTrue("Tier 1 free claimable again", Tier(1)->IsFreeClaimable());
			TestFalse("Toast hidden", ViewModel->ShouldShowLevelUp());
		});
	});

	Describe("Level-up toast", [this]()
	{
		It("shows the toast with the new level", [this]()
		{
			ViewModel->DebugAddXP(1000);
			TestTrue("Shown", ViewModel->ShouldShowLevelUp());
			TestEqual("Text", ViewModel->GetLevelUpText().ToString(), FString(TEXT("LEVEL UP! 3")));
		});

		It("hides when dismissed", [this]()
		{
			ViewModel->DebugAddXP(1000);
			ViewModel->DismissLevelUp();
			TestFalse("Hidden", ViewModel->ShouldShowLevelUp());
		});

		It("re-broadcasts when leveling up again while the toast is showing", [this]()
		{
			int32* ShowNotifications = CountNotifications(USeasonPassViewModel::FFieldNotificationClassDescriptor::bShowLevelUp);
			ViewModel->DebugAddXP(500);
			ViewModel->DebugAddXP(500);
			TestEqual("bShowLevelUp notifications", *ShowNotifications, 2);
			TestEqual("Text", ViewModel->GetLevelUpText().ToString(), FString(TEXT("LEVEL UP! 3")));
		});
	});

	Describe("FieldNotify", [this]()
	{
		It("only broadcasts fields whose values changed", [this]()
		{
			int32* LevelNotifications = CountNotifications(USeasonPassViewModel::FFieldNotificationClassDescriptor::CurrentLevel);
			int32* ProgressNotifications = CountNotifications(USeasonPassViewModel::FFieldNotificationClassDescriptor::XPProgress);

			ViewModel->DebugAddXP(100);

			TestEqual("CurrentLevel notifications", *LevelNotifications, 0);
			TestEqual("XPProgress notifications", *ProgressNotifications, 1);
		});
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS
