// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/UserWidget.h"
#include "PinnedAssetSlotBase.generated.h"

class UTextBlock;
class UAssetThumbnailWidget;
class UPinnedSectionBase;
/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinnedAssetSlotBase : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
	FString AssetPath;
	UPinnedSectionBase* ParentRef;

public:
	void SetAssetData(const FString& Path, const FAssetData& Asset);
	void SetParentRef(UPinnedSectionBase* ParentReference);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UAssetThumbnailWidget* Thumbnail;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
