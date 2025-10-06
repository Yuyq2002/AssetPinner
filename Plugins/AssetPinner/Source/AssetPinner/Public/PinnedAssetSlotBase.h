// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
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
	void RecheckInput(FKey Input);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	USizeBox* SizeBox;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UBorder* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UAssetThumbnailWidget* Thumbnail;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UEditorUtilityWidget> ContextMenuWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor HoverColor;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
