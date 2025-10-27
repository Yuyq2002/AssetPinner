// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Tab.generated.h"

class UBorder;
class UTextBlock;
class UPinnedSectionBase;

/**
 * 
 */
UCLASS()
class ASSETPINNER_API UTab : public UEditorUtilityWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTabClickedSignature, UTab*, Initiator);
	
public:
	void SetInfo(FString Name, UPinnedSectionBase* InParent, int InTabIndex);
	int SetSelected(bool IsSelected = false);

	UPROPERTY()
	FOnTabClickedSignature OnTabClickedDelegate;

private:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UBorder* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* Text;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor HoverColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor SelectedColor;

	int Index;
	bool bIsSelected;

	UPROPERTY()
	UPinnedSectionBase* Parent;
};
