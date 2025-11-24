// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Tab.generated.h"

class UBorder;
class UVerticalTextBlock;
class UEditorUtilityEditableTextBox;
class UPinnedWindowBase;
class UEditorUtilityButton;

/**
 * 
 */

UCLASS()
class ASSETPINNER_API UTab : public UEditorUtilityWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTabClickedSignature, UTab*, Initiator);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRemoveClickedSignature, UTab*, Initiator);
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnNameChangedSignature, UTab*, Initiator, FText, OldName, FText, NewName);
	
public:
	virtual void NativeConstruct() override;
	void SetInfo(FText InName, UPinnedWindowBase* InParent, UWidget* InWidget, bool IsPersistent = false);
	void SetInfo(UPinnedWindowBase* InParent, UWidget* InWidget);
	void SetInfo(FText InName);
	void EditName(bool EnableEditing);
	UWidget* SetSelected(bool IsSelected = false);
	UWidget* GetSection();
	FString GetName();

	UPROPERTY()
	FOnTabClickedSignature OnTabClickedDelegate;

	UPROPERTY()
	FOnNameChangedSignature OnNameChangedDelegate;

	UPROPERTY()
	FOnRemoveClickedSignature OnRemoveClickedDelegate;

private:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnRemoveClicked();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UBorder* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UVerticalTextBlock* Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UEditorUtilityEditableTextBox* RenameTextBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UEditorUtilityButton* RemoveButton;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor HoverColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor SelectedColor;

	UPROPERTY()
	UWidget* Widget;
	bool bIsSelected;

	UPROPERTY()
	UPinnedWindowBase* Parent;
};
