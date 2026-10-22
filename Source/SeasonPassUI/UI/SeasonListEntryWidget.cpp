#include "UI/SeasonListEntryWidget.h"

#include "Model/SeasonTypes.h"
#include "MVVMSubsystem.h"
#include "View/MVVMView.h"

void USeasonListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(this);
	if (!View || !View->SetViewModelByClass(ListItemObject))
	{
		UE_LOG(LogSeasonPass, Warning, TEXT("%s: no Manual ViewModel slot accepts %s. Add one in View Bindings."),
			*GetName(), *GetNameSafe(ListItemObject));
	}
}
