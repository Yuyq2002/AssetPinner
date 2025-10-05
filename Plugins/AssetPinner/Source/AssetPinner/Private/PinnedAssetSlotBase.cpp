// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSlotBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PinnedSectionBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components\AssetThumbnailWidget.h"

void UPinnedAssetSlotBase::SetAssetData(const FString& Path, const FAssetData& Asset)
{
	AssetPath = Path;

	if(Name)
		Name->SetText(FText::FromString(FPackageName::GetShortName(*Path)));

	Thumbnail->SetAsset(Asset);
}

void UPinnedAssetSlotBase::SetParentRef(UPinnedSectionBase* ParentReference)
{
	ParentRef = ParentReference;
}

void UPinnedAssetSlotBase::RecheckInput(FKey Input)
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	if (Input == EKeys::LeftMouseButton)
		Subsystem->MoveAssetPath(AssetPath);
	else if (Input == EKeys::RightMouseButton)
		Subsystem->RemoveAssetPath(AssetPath);
}

FReply UPinnedAssetSlotBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		return FReply::Unhandled();

	TArray<FAssetData> Assets;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.GetAssetsByPackageName(FName(AssetPath), Assets);

	if (Assets.Num() <= 0)
		return FReply::Handled();

	UObject* Asset = Assets[0].GetAsset();

	if (!Asset)
	{
		UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed - Asset is not valid"));
		return FReply::Handled();
	}

	UAssetEditorSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr;
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed - Asset Editor Subsystem is not valid"));
		return FReply::Handled();
	}

	bool success = Subsystem->OpenEditorForAsset(Asset);
	if (success)
	{
		UE_LOG(LogTemp, Log, TEXT("Open Asset Window Succeeded"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed"));

	return FReply::Handled();
}

FReply UPinnedAssetSlotBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EditState State = ParentRef->CheckInEditMode();

	if (!GEditor || State == EditState::NotInEditMode)
		return FReply::Handled();

	if (State == EditState::Unfocused)
	{
		ParentRef->AddRecheck(this, InMouseEvent.GetEffectingButton());
		return FReply::Handled();
	}

	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return FReply::Handled();

	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		Subsystem->MoveAssetPath(AssetPath);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
		Subsystem->RemoveAssetPath(AssetPath);

	return FReply::Handled();
}
