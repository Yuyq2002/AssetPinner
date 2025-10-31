// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Enums.h"
#include "PinnedWindowBase.generated.h"

class UWrapBox;
class UEditorUtilityButton;
class UPinnedAssetSlotBase;
class USizeBox;
class UScrollBox;
class UPinnedAssetSubsystem;
class UWidgetSwitcher;
class UHorizontalBox;
class UTab;
class UPinnedSectionBase;
struct FPinnedAssetData;
struct FSection;

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
class ASSETPINNER_API UPinnedWindowBase : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	EditState CheckInEditMode();

	UFUNCTION()
	void SwitchTab(UWidget* Widget);

private:
	UFUNCTION()
	void OnListChangedCallback(const TArray<FPinnedAssetData>& List);
	void Refresh(const TArray<FPinnedAssetData>& List);
	UFUNCTION()
	void OnTabClicked(UTab* Initiator);

	UFUNCTION()
	void OnTabRenamed(UTab* Initiator, FText OldName, FText NewName);

	UFUNCTION()
	void OnTabRemoved(UTab* Initiator);

	UFUNCTION(BlueprintCallable)
	void OnClearButtonClicked();

	UFUNCTION()
	void OnNewTabClicked();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	bool FindSection(FString Name, int& OutIndex);
	bool ContainsSection(FString Name);

private:
	EditState EditMode;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UEditorUtilityWidget> AssetSlotWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UEditorUtilityWidget> TabWidget;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<UEditorUtilityWidget> PinnedSectionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UWidgetSwitcher* TabController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UHorizontalBox* TabList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UPinnedSectionBase* PinnedSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UPinnedSectionBase* RecentSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, BindWidget))
	UEditorUtilityButton* NewTabButton;

	UPROPERTY()
	TArray<UPinnedAssetSlotBase*> Slots;

	float MinSize = 30;
	float Size = 100;
	float Ratio = 1.25;
	FString ConfigPath;

	UPROPERTY()
	UPinnedAssetSubsystem* PinnedAssetSubsystem;

	UPROPERTY()
	UTab* ActiveTab;

	UPROPERTY()
	TArray<FSection> SectionMap;

	int DefaultNameIndex;
};
