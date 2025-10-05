// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "PinnedSectionBase.generated.h"

class UWrapBox;
class UEditorUtilityButton;
class UPinnedAssetSlotBase;

enum class EditState
{
	Unfocused,
	NotInEditMode,
	InEditMode
};

/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinnedSectionBase : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct();
	EditState CheckInEditMode();
	void AddRecheck(UPinnedAssetSlotBase* Caller, FKey Input);

private:
	UFUNCTION()
	void OnListChangedCallback(const TArray<FString>& List, const TArray<bool>& StatusList);
	void Refresh(const TArray<FString>& List, const TArray<bool>& StatusList);

	UFUNCTION()
	void OnClearButtonClicked();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

private:
	EditState EditMode;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UEditorUtilityWidget> AssetSlotWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UWrapBox* PinnedWrapBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UWrapBox* RecentWrapBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UEditorUtilityButton* ClearButton;

	UPROPERTY()
	UPinnedAssetSlotBase* RecallEditAction;

	UPROPERTY()
	FKey MouseInput;
};
