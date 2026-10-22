#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SkillNodeViewModel.generated.h"

/** One Gamify.Life skill node chip (e.g. "Cooking II"). */
UCLASS(BlueprintType)
class SEASONPASSUI_API USkillNodeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize(FName InNodeId);
	void SetUnlocked(bool bInUnlocked);

	FName GetNodeId() const { return NodeId; }

private:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	FName NodeId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Category = "Skill", meta = (AllowPrivateAccess = "true"))
	bool bUnlocked = false;
};
