#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "SeasonListEntryWidget.generated.h"

/**
 * Base class for ListView entries whose item is a ViewModel (WBP_TierTile, WBP_SkillChip).
 * When the ListView hands this entry an item, it is assigned to the entry's matching
 * ViewModel slot, so the Blueprint needs no "On List Item Object Set" graph.
 */
UCLASS(Abstract)
class SEASONPASSUI_API USeasonListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
