#include "UI/SkillChipWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "UI/SeasonPassStyle.h"

using namespace SeasonPassStyle;

void USkillChipWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	constexpr float PillRadius = 22.f;

	if (Chip)
	{
		Chip->SetBrush(RoundedBrush(Color::Skill, PillRadius, Color::SkillHi, 1.5f));
		Chip->SetBrushColor(FLinearColor::White);
		Chip->SetContentColorAndOpacity(FLinearColor::White);
	}
	if (LockedShade)
	{
		LockedShade->SetBrush(RoundedBrush(FLinearColor(0.f, 0.f, 0.f, 0.68f), PillRadius));
		LockedShade->SetBrushColor(FLinearColor::White);
	}
	StyleText(NodeName, 13.f, TEXT("Bold"), FLinearColor::White, 40);
}
