#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "ExtendedBorder.generated.h"

UCLASS(MinimalAPI)
class UExtendedBorder : public UBorder
{
	GENERATED_UCLASS_BODY()

	void ExtendContextMenu(FMenuBuilder& Builder);

	void Pin();
	bool CanPin();
	void Unpin();
	void LocateInBrowser();
	void SwitchTab(int Index);
	void GenerateTabSubMenu(FMenuBuilder& Builder);

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	FString AssetPath;
};

