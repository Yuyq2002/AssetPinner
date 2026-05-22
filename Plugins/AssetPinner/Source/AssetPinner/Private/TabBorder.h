#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "TabBorder.generated.h"

UCLASS(MinimalAPI)
class UTabBorder : public UBorder
{
	DECLARE_DYNAMIC_DELEGATE(FTriggerEventSignature);
	DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FCheckCanRenameSignature);

	GENERATED_UCLASS_BODY()

	void ExtendContextMenu(FMenuBuilder& Builder);

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	FTriggerEventSignature OnRenameClickedDelegate;
	FTriggerEventSignature OnRemoveClickedDelegate;
	FCheckCanRenameSignature CheckCanRenameDelegate;

private:
	void OpenRenameBox();
	bool CanEdit();
	void RemoveTab();

private:
};

