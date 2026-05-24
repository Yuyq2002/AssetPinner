// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Tab.generated.h"

class UTabBorder;
class UVerticalTextBlock;
class UVerticalEditableTextBox;
class UPinnedWindowBase;
class UPinnedSectionBase;
class UEditorUtilityButton;

/**
 * 
 */

UCLASS()
class ASSETPINNER_API UTab : public UEditorUtilityWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTabClickedSignature, UTab*, Initiator);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRemoveSignature, UTab*, Initiator);
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnNameChangedSignature, UTab*, Initiator, FText, OldName, FText, NewName);
	
public:
	virtual void NativeConstruct() override;
	void SetInfo(FText InName, UPinnedWindowBase* InParent, UPinnedSectionBase* InWidget, bool InIsPersistent = false);
	void SetInfo(UPinnedWindowBase* InParent, UPinnedSectionBase* InWidget);
	void SetInfo(FText InName);
	void EditName(bool EnableEditing);
	UFUNCTION()
	void ActivateRenameBox();
	UPinnedSectionBase* SetSelected(bool IsSelected = false);
	UPinnedSectionBase* GetSection();
	FString GetName();
	void InitInRenameMode() { bInitInRenameMode = true; }

	UFUNCTION()
	bool GetIsPersistent() { return bIsPersistent; }

	UPROPERTY()
	FOnTabClickedSignature OnTabClickedDelegate;

	UPROPERTY()
	FOnNameChangedSignature OnNameChangedDelegate;

	UPROPERTY()
	FOnRemoveSignature OnRemoveDelegate;

private:
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnRemoveClicked();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UTabBorder* Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UVerticalTextBlock* Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UVerticalEditableTextBox* RenameTextBox;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor HoverColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor SelectedColor;

	UPROPERTY()
	UPinnedSectionBase* Widget;
	bool bIsSelected;
	bool bIsPersistent = false;
	bool bInitInRenameMode = false;

	UPROPERTY()
	UPinnedWindowBase* Parent;
};
