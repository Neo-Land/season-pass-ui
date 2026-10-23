#pragma once

#include "CoreMinimal.h"
#include "UI/SeasonListEntryWidget.h"
#include "SkillChipWidget.generated.h"

class UBorder;
class UTextBlock;

/** Parent class for WBP_SkillChip. Draws the chip and its locked shade as matching pills. */
UCLASS(Abstract)
class SEASONPASSUI_API USkillChipWidget : public USeasonListEntryWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Chip;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> LockedShade;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NodeName;
};
