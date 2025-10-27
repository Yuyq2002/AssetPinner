// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "ExtendedBorder.h"
#include <Components/Image.h>
#include "Enums.h"
#include "PinnedAssetSlotBase.generated.h"

class UTextBlock;
class UAssetThumbnailWidget;
class UPinnedSectionBase;
struct FPinnedAssetData;
class UPinnedAssetSubsystem;
/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinnedAssetSlotBase : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
	FString AssetPath;

	EPathType PathType;

public:
	void SetAssetData(const FPinnedAssetData& Data);
	void SetThumbnail(const FAssetData& AssetData, const UPinnedAssetSubsystem* PinnedAssetSubsystem);
	FString GetAssetPath();
	void SetSize(int Width, int Height);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UExtendedBorder* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	USizeBox* ThumbnailHolder;

	UPROPERTY()
	UAssetThumbnailWidget* Thumbnail;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor HoverColor;

	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

};
