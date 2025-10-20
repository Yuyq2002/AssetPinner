// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Enums.h"
#include "PinnedSectionBase.generated.h"

class UWrapBox;
class UEditorUtilityButton;
class UPinnedAssetSlotBase;
class USizeBox;
class UScrollBox;
class UPinnedAssetSubsystem;

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
	virtual void NativeDestruct();
	EditState CheckInEditMode();
	void AddRecheck(UPinnedAssetSlotBase* Caller, FKey Input);

private:
	UFUNCTION()
	void OnListChangedCallback(const TArray<FString>& List, const TArray<bool>& StatusList, const TArray<EPathType>& PathTypes);
	void Refresh(const TArray<FString>& List, const TArray<bool>& StatusList, const TArray<EPathType>& PathTypes);

	UFUNCTION()
	void OnClearButtonClicked();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	UTexture2D* GetObjectThumbnailAsTexture2D(const FAssetData& AssetData);

private:
	EditState EditMode;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UEditorUtilityWidget> AssetSlotWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UScrollBox* PinnedScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UWrapBox* PinnedWrapBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UScrollBox* RecentScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UWrapBox* RecentWrapBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UEditorUtilityButton* ClearButton;

	UPROPERTY()
	UPinnedAssetSlotBase* RecallEditAction;

	UPROPERTY()
	FKey MouseInput;

	UPROPERTY()
	TArray<UPinnedAssetSlotBase*> Slots;

	float MinSize = 30;
	float Size = 100;
	float Ratio = 1.25;
	FString ConfigPath;

	UPROPERTY()
	UPinnedAssetSubsystem* PinnedAssetSubsystem;
};
