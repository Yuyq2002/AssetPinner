// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSlotBase.h"
#include "Components/TextBlock.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UPinnedAssetSlotBase::SetAssetData(const FString& Path)
{
	AssetPath = Path;

	Name->SetText(FText::FromString(FPackageName::GetShortName(*Path)));
}

FReply UPinnedAssetSlotBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
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
