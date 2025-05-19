// Fill out your copyright notice in the Description page of Project Settings.


#include "PinAssetAction.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "UnrealEdGlobals.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "PinnedAssetSubsystem.h"


void UPinAssetAction::PinAssets()
{
	TArray<FAssetData> SelectedAssets;

	GetSelectedAssets(SelectedAssets);

	for (auto& AssetData : SelectedAssets)
	{
		FString AssetPath = AssetData.PackageName.ToString();

		UPinnedAssetSubsystem* Subsystem = nullptr;
		if (GEngine)
			Subsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();

		if (Subsystem)
			Subsystem->AddAssetPath(AssetPath);
	}

		//if (!Asset)
		//{
		//	UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed - Asset is not valid"));
		//}

		//UAssetEditorSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr;
		//if (!Subsystem)
		//{
		//	UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed - Asset Editor Subsystem is not valid"));
		//}

		//bool success = Subsystem->OpenEditorForAsset(Asset);
		//if (success)
		//{
		//	UE_LOG(LogTemp, Log, TEXT("Open Asset Window Succeeded"));
		//}
		//else 
		//	UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed"));
}

void UPinAssetAction::GetSelectedAssets(TArray<FAssetData>& SelectedAssets)
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();

	ContentBrowserSingleton.GetSelectedAssets(SelectedAssets);
}
