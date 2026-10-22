#include "ViewModel/SkillNodeViewModel.h"

void USkillNodeViewModel::Initialize(FName InNodeId)
{
	UE_MVVM_SET_PROPERTY_VALUE(NodeId, InNodeId);
	UE_MVVM_SET_PROPERTY_VALUE(DisplayName, FText::FromName(InNodeId));
}

void USkillNodeViewModel::SetUnlocked(bool bInUnlocked)
{
	UE_MVVM_SET_PROPERTY_VALUE(bUnlocked, bInUnlocked);
}
