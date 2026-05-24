// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "PinnedSectionBase.generated.h"

class UWrapBox;
class UScrollBox;
class UPinnedAssetSlotBase;

/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinnedSectionBase : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	void ClearPinnedAsset();
	void AddPinnedAsset(UPinnedAssetSlotBase* NewPinnedSlot);
	void SetEnableScrolling(bool IsEnabled);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UScrollBox* ScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UWrapBox* WrapBox;

private:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
