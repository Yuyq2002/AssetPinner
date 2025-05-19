// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Blueprint/UserWidget.h"
#include "PinnedAssetSlotBase.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinnedAssetSlotBase : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
	FString AssetPath;

public:
	void SetAssetData(const FString& Path);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* Name;

	FReply NativeOnMouseButtonDoubleClick(FGeometry InMyGeometry, const FPointerEvent& InMouseEvent);
};
