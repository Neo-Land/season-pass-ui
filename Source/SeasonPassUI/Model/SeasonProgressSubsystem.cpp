#include "Model/SeasonProgressSubsystem.h"

#include "Engine/DataTable.h"

DEFINE_LOG_CATEGORY(LogSeasonPass);

const TCHAR* USeasonProgressSubsystem::TierTablePath = TEXT("/Game/Data/DT_SeasonTiers.DT_SeasonTiers");

void USeasonProgressSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TierTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TierTablePath)).LoadSynchronous();
	if (!TierTable)
	{
		UE_LOG(LogSeasonPass, Warning, TEXT("Tier table not found at %s. Import Content/Data/SeasonTiers.csv as DT_SeasonTiers."), TierTablePath);
		return;
	}

	TArray<FSeasonTierRow*> RowPtrs;
	TierTable->GetAllRows<FSeasonTierRow>(TEXT("USeasonProgressSubsystem::Initialize"), RowPtrs);

	TArray<FSeasonTierRow> Rows;
	Rows.Reserve(RowPtrs.Num());
	for (const FSeasonTierRow* Row : RowPtrs)
	{
		Rows.Add(*Row);
	}
	SetTierRows(MoveTemp(Rows));

	UE_LOG(LogSeasonPass, Log, TEXT("Loaded %d season tiers."), Tiers.Num());
}

void USeasonProgressSubsystem::SetTierRows(TArray<FSeasonTierRow> Rows)
{
	Rows.Sort([](const FSeasonTierRow& A, const FSeasonTierRow& B) { return A.TierIndex < B.TierIndex; });
	Tiers = MoveTemp(Rows);
}

void USeasonProgressSubsystem::SetTierRowsForTest(TArray<FSeasonTierRow> Rows)
{
	SetTierRows(MoveTemp(Rows));
	ResetProgress();
}

void USeasonProgressSubsystem::AddXP(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	const int32 OldLevel = GetCurrentLevel();
	TotalXP += Amount;
	const int32 NewLevel = GetCurrentLevel();

	OnProgressChanged.Broadcast();
	if (NewLevel > OldLevel)
	{
		OnLevelUp.Broadcast(NewLevel);
	}
}

int32 USeasonProgressSubsystem::GetCurrentLevel() const
{
	int32 Level = 0;
	for (const FSeasonTierRow& Row : Tiers)
	{
		if (Row.XPRequired > TotalXP)
		{
			break;
		}
		Level = Row.TierIndex;
	}
	return Level;
}

float USeasonProgressSubsystem::GetProgressToNext() const
{
	const int32 Level = GetCurrentLevel();
	const FSeasonTierRow* Next = FindTier(Level + 1);
	if (!Next)
	{
		return Level > 0 ? 1.f : 0.f;
	}

	const FSeasonTierRow* Current = FindTier(Level);
	const int32 Floor = Current ? Current->XPRequired : 0;
	const int32 Span = Next->XPRequired - Floor;
	return Span > 0 ? FMath::Clamp(float(TotalXP - Floor) / float(Span), 0.f, 1.f) : 1.f;
}

ETierState USeasonProgressSubsystem::GetTierState(int32 Tier, bool bPremium) const
{
	if (!FindTier(Tier))
	{
		return ETierState::Locked;
	}

	const TSet<int32>& Claimed = bPremium ? ClaimedPremium : ClaimedFree;
	if (Claimed.Contains(Tier))
	{
		return ETierState::Claimed;
	}

	const bool bLevelReached = GetCurrentLevel() >= Tier;
	const bool bOwnsTrack = !bPremium || bHasPremium;
	return (bLevelReached && bOwnsTrack) ? ETierState::Claimable : ETierState::Locked;
}

bool USeasonProgressSubsystem::ClaimTier(int32 Tier, bool bPremium)
{
	if (GetTierState(Tier, bPremium) != ETierState::Claimable)
	{
		return false;
	}

	(bPremium ? ClaimedPremium : ClaimedFree).Add(Tier);

	const FSeasonTierRow* Row = FindTier(Tier);
	if (Row && !Row->UnlocksSkillNode.IsNone())
	{
		UnlockedSkills.Add(Row->UnlocksSkillNode);
	}

	OnTierClaimed.Broadcast(Tier, bPremium);
	return true;
}

void USeasonProgressSubsystem::SetPremium(bool bInHasPremium)
{
	if (bHasPremium == bInHasPremium)
	{
		return;
	}
	bHasPremium = bInHasPremium;
	OnProgressChanged.Broadcast();
}

void USeasonProgressSubsystem::ResetProgress()
{
	TotalXP = 0;
	bHasPremium = false;
	ClaimedFree.Reset();
	ClaimedPremium.Reset();
	UnlockedSkills.Reset();
	OnProgressChanged.Broadcast();
}

int32 USeasonProgressSubsystem::GetMaxLevel() const
{
	return Tiers.Num() > 0 ? Tiers.Last().TierIndex : 0;
}

int32 USeasonProgressSubsystem::GetXPRequiredForTier(int32 Tier) const
{
	const FSeasonTierRow* Row = FindTier(Tier);
	return Row ? Row->XPRequired : INDEX_NONE;
}

const FSeasonTierRow* USeasonProgressSubsystem::FindTier(int32 Tier) const
{
	return Tiers.FindByPredicate([Tier](const FSeasonTierRow& Row) { return Row.TierIndex == Tier; });
}
