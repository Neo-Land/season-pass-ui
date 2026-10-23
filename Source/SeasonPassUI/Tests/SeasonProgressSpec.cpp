#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "Model/SeasonProgressSubsystem.h"
#include "Model/SeasonSaveGame.h"
#include "Tests/SeasonTestData.h"
#include "UObject/StrongObjectPtr.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FSeasonProgressSpec, "SeasonPass.Progress",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

	// Game-instance subsystems must live inside a UGameInstance, so the tests make a bare one as the outer.
	TStrongObjectPtr<UGameInstance> GameInstance;
	TStrongObjectPtr<USeasonProgressSubsystem> Progress;

	/** Compares tier states by name so failures read "Claimed" instead of "2". */
	void TestState(const FString& What, ETierState Actual, ETierState Expected)
	{
		TestEqual(What, UEnum::GetValueAsString(Actual), UEnum::GetValueAsString(Expected));
	}

END_DEFINE_SPEC(FSeasonProgressSpec)

void FSeasonProgressSpec::Define()
{
	BeforeEach([this]()
	{
		GameInstance.Reset(NewObject<UGameInstance>());
		Progress.Reset(NewObject<USeasonProgressSubsystem>(GameInstance.Get()));
		Progress->SetTierRowsForTest(SeasonTest::MakeTiers());
	});

	AfterEach([this]()
	{
		Progress.Reset();
		GameInstance.Reset();
	});

	Describe("Level math", [this]()
	{
		It("starts at level 1 with 0 XP", [this]()
		{
			TestEqual("Level", Progress->GetCurrentLevel(), 1);
			TestEqual("Progress", Progress->GetProgressToNext(), 0.f);
		});

		It("stays on level 1 one XP below the tier 2 boundary", [this]()
		{
			Progress->AddXP(499);
			TestEqual("Level", Progress->GetCurrentLevel(), 1);
			TestEqual("Progress", Progress->GetProgressToNext(), 499.f / 500.f, KINDA_SMALL_NUMBER);
		});

		It("reaches level 2 exactly at the boundary", [this]()
		{
			Progress->AddXP(500);
			TestEqual("Level", Progress->GetCurrentLevel(), 2);
			TestEqual("Progress", Progress->GetProgressToNext(), 0.f);
		});

		It("caps at the max level with full progress", [this]()
		{
			Progress->AddXP(1000000);
			TestEqual("Level", Progress->GetCurrentLevel(), 5);
			TestEqual("Progress", Progress->GetProgressToNext(), 1.f);
		});

		It("ignores zero and negative XP", [this]()
		{
			Progress->AddXP(0);
			Progress->AddXP(-100);
			TestEqual("TotalXP", Progress->GetTotalXP(), 0);
		});

		It("broadcasts one level-up with the new level on a multi-level jump", [this]()
		{
			int32 Broadcasts = 0;
			int32 ReportedLevel = 0;
			Progress->OnLevelUp.AddLambda([&](int32 NewLevel) { ++Broadcasts; ReportedLevel = NewLevel; });

			Progress->AddXP(1200);
			TestEqual("Broadcasts", Broadcasts, 1);
			TestEqual("Reported level", ReportedLevel, 3);
		});
	});

	Describe("Claiming", [this]()
	{
		It("can't claim a locked tier", [this]()
		{
			TestState("State", Progress->GetTierState(3, false), ETierState::Locked);
			TestFalse("Claim", Progress->ClaimTier(3, false));
		});

		It("can claim a reached free tier", [this]()
		{
			TestState("State before", Progress->GetTierState(1, false), ETierState::Claimable);
			TestTrue("Claim", Progress->ClaimTier(1, false));
			TestState("State after", Progress->GetTierState(1, false), ETierState::Claimed);
		});

		It("can't claim a tier twice", [this]()
		{
			TestTrue("First claim", Progress->ClaimTier(1, false));
			TestFalse("Second claim", Progress->ClaimTier(1, false));
		});

		It("needs premium for a premium tier", [this]()
		{
			TestState("State without premium", Progress->GetTierState(1, true), ETierState::Locked);
			TestFalse("Claim without premium", Progress->ClaimTier(1, true));

			Progress->SetPremium(true);
			TestState("State with premium", Progress->GetTierState(1, true), ETierState::Claimable);
			TestTrue("Claim with premium", Progress->ClaimTier(1, true));
		});

		It("tracks free and premium claims separately", [this]()
		{
			Progress->SetPremium(true);
			TestTrue("Claim free", Progress->ClaimTier(1, false));
			TestState("Premium still claimable", Progress->GetTierState(1, true), ETierState::Claimable);
		});

		It("unlocks the tier's skill node when claimed", [this]()
		{
			Progress->AddXP(1000);
			TestFalse("Unlocked before claim", Progress->IsSkillUnlocked(TEXT("Cooking II")));
			TestTrue("Claim", Progress->ClaimTier(3, false));
			TestTrue("Unlocked after claim", Progress->IsSkillUnlocked(TEXT("Cooking II")));
		});

		It("rejects tiers that don't exist", [this]()
		{
			Progress->AddXP(1000000);
			TestFalse("Tier 0", Progress->ClaimTier(0, false));
			TestFalse("Tier 6", Progress->ClaimTier(6, false));
		});
	});

	Describe("Reset", [this]()
	{
		It("clears XP, premium, claims and skills", [this]()
		{
			Progress->AddXP(1000);
			Progress->SetPremium(true);
			Progress->ClaimTier(3, false);
			Progress->ResetProgress();

			TestEqual("TotalXP", Progress->GetTotalXP(), 0);
			TestFalse("Premium", Progress->HasPremium());
			TestState("Tier 1 claimable again", Progress->GetTierState(1, false), ETierState::Claimable);
			TestFalse("Skill", Progress->IsSkillUnlocked(TEXT("Cooking II")));
		});
	});

	Describe("Save data", [this]()
	{
		It("round-trips XP, premium, claims and skills", [this]()
		{
			Progress->AddXP(1200);
			Progress->SetPremium(true);
			Progress->ClaimTier(1, false);
			Progress->ClaimTier(3, true);

			USeasonSaveGame* Save = NewObject<USeasonSaveGame>();
			Progress->WriteTo(*Save);

			USeasonProgressSubsystem* Restored = NewObject<USeasonProgressSubsystem>(GameInstance.Get());
			Restored->SetTierRowsForTest(SeasonTest::MakeTiers());
			Restored->ReadFrom(*Save);

			TestEqual("TotalXP", Restored->GetTotalXP(), 1200);
			TestEqual("Level", Restored->GetCurrentLevel(), 3);
			TestTrue("Premium", Restored->HasPremium());
			TestState("Tier 1 free", Restored->GetTierState(1, false), ETierState::Claimed);
			TestState("Tier 3 premium", Restored->GetTierState(3, true), ETierState::Claimed);
			TestState("Tier 3 free still claimable", Restored->GetTierState(3, false), ETierState::Claimable);
			TestTrue("Skill", Restored->IsSkillUnlocked(TEXT("Cooking II")));
		});

		It("notifies listeners when progress is loaded", [this]()
		{
			int32 Broadcasts = 0;
			Progress->OnProgressChanged.AddLambda([&]() { ++Broadcasts; });

			USeasonSaveGame* Save = NewObject<USeasonSaveGame>();
			Save->TotalXP = 600;
			Progress->ReadFrom(*Save);

			TestEqual("Broadcasts", Broadcasts, 1);
			TestEqual("Level", Progress->GetCurrentLevel(), 2);
		});
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS
