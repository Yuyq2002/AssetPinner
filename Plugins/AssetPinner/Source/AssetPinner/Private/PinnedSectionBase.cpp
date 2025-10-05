// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedSectionBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components/WrapBox.h"
#include "EditorUtilityWidgetComponents.h"
#include "PinnedAssetSlotBase.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UPinnedSectionBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!GEditor)
		return;

	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->OnListChangedDelegate.BindDynamic(this, &UPinnedSectionBase::OnListChangedCallback);

	Refresh(PinnedAssetSubsystem->GetAssetPathList(), PinnedAssetSubsystem->GetStatusList());

	ClearButton->OnClicked.AddDynamic(this, &UPinnedSectionBase::OnClearButtonClicked);
}

EditState UPinnedSectionBase::CheckInEditMode()
{
	return EditMode;
}


void UPinnedSectionBase::AddRecheck(UPinnedAssetSlotBase* Caller, FKey Input)
{
	RecallEditAction = Caller;
	MouseInput = Input;
}

void UPinnedSectionBase::OnListChangedCallback(const TArray<FString>& List, const TArray<bool>& StatusList)
{
	Refresh(List, StatusList);
}

void UPinnedSectionBase::Refresh(const TArray<FString>& List, const TArray<bool>& StatusList)
{
	if (!AssetSlotWidget) return;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	PinnedWrapBox->ClearChildren();
	RecentWrapBox->ClearChildren();

	for (int i = 0; i < List.Num(); i++)
	{
		TArray<FAssetData> Assets;
		AssetRegistry.GetAssetsByPackageName(FName(List[i]), Assets);
		if (Assets.Num() <= 0)
			continue;

		UPinnedAssetSlotBase* NewSlot = CreateWidget<UPinnedAssetSlotBase>(this, AssetSlotWidget);
		NewSlot->SetAssetData(List[i], Assets[0]);
		NewSlot->SetParentRef(this);

		if (StatusList[i])
			PinnedWrapBox->AddChildToWrapBox(NewSlot);
		else
			RecentWrapBox->AddChildToWrapBox(NewSlot);
	}

	return;
}

void UPinnedSectionBase::OnClearButtonClicked()
{
	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->ClearRecent();
}


FReply UPinnedSectionBase::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::InEditMode;
		RecallEditAction->RecheckInput(MouseInput);
	}
	return FReply::Handled();
}

FReply UPinnedSectionBase::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
		EditMode = EditState::NotInEditMode;
	return FReply::Handled();
}

void UPinnedSectionBase::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::Unfocused;
}
