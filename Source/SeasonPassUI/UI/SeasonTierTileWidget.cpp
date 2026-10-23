#include "UI/SeasonTierTileWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/SeasonPassStyle.h"

using namespace SeasonPassStyle;

void USeasonTierTileWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	StyleText(TierNumber, 24.f, TEXT("Bold"), Color::Text);
	StyleText(CurrentLabel, 10.f, TEXT("Bold"), Color::Gold, 120);
	StyleText(FreeReward, 13.f, TEXT("Regular"), Color::Text);
	StyleText(PremiumReward, 13.f, TEXT("Regular"), Color::Text);
	StyleText(FreeLock, 11.f, TEXT("Bold"), Color::TextDim, 80);
	StyleText(PremiumLock, 11.f, TEXT("Bold"), Color::TextDim, 80);
	StyleText(FreeCheck, 11.f, TEXT("Bold"), Color::Success, 40);
	StyleText(PremiumCheck, 11.f, TEXT("Bold"), Color::Success, 40);

	// Free rewards claim with a light button, premium with gold.
	StyleButton(ClaimFreeButton,
		MakeButtonStyle(Color::Text, FLinearColor::White, Color::TextMuted, 8.f, FLinearColor::Transparent, FLinearColor::Transparent, FMargin(10.f, 3.f)),
		12.f, Color::Ink);
	StyleButton(ClaimPremiumButton,
		MakeButtonStyle(Color::Gold, Color::GoldHi, Color::GoldLo, 8.f, FLinearColor::Transparent, FLinearColor::Transparent, FMargin(10.f, 3.f)),
		12.f, Color::Ink);

	if (RarityBorder)
	{
		RarityBorder->SetPadding(FMargin(10.f));
	}
	ApplyTileBrush();
}

void USeasonTierTileWidget::SetRarityColor(FLinearColor InRarityColor)
{
	RarityColor = InRarityColor;
	ApplyTileBrush();
}

void USeasonTierTileWidget::SetIsCurrentTier(bool bInIsCurrentTier)
{
	bIsCurrentTier = bInIsCurrentTier;
	ApplyTileBrush();
}

void USeasonTierTileWidget::ApplyTileBrush()
{
	if (!RarityBorder)
	{
		return;
	}

	// Dark body tinted by rarity, with a rarity outline. The current tier gets a brighter body and a gold outline.
	const float Tint = bIsCurrentTier ? 0.45f : 0.28f;
	const FLinearColor Body = FLinearColor::LerpUsingHSV(Color::Panel, RarityColor, Tint);
	const FLinearColor Outline = bIsCurrentTier ? Color::Gold : RarityColor;
	const float OutlineWidth = bIsCurrentTier ? 3.f : 2.f;

	RarityBorder->SetBrush(RoundedBrush(Body, 12.f, Outline, OutlineWidth));
	RarityBorder->SetBrushColor(FLinearColor::White);
}
