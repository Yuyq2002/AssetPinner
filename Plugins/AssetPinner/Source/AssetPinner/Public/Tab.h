// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Tab.generated.h"

class UBorder;
class UTextBlock;
class UEditorUtilityEditableTextBox;
class UPinnedWindowBase;

/**
 * 
 */

UCLASS()
class ASSETPINNER_API UTab : public UEditorUtilityWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTabClickedSignature, UTab*, Initiator);
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnNameChangedSignature, UTab*, Initiator, FText, OldName, FText, NewName);
	
public:
	virtual void NativeConstruct() override;
	void SetInfo(FText InName, UPinnedWindowBase* InParent, int InTabIndex);
	void SetInfo(UPinnedWindowBase* InParent, int InTabIndex);
	void SetInfo(FText InName);
	void EditName(bool EnableEditing);
	int SetSelected(bool IsSelected = false);

	UPROPERTY()
	FOnTabClickedSignature OnTabClickedDelegate;

	UPROPERTY()
	FOnNameChangedSignature OnNameChangedDelegate;

private:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UBorder* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTextBlock* Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UEditorUtilityEditableTextBox* RenameTextBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor HoverColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor SelectedColor;

	int Index;
	bool bIsSelected;

	UPROPERTY()
	UPinnedWindowBase* Parent;
};
