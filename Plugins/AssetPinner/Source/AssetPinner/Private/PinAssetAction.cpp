// Fill out your copyright notice in the Description page of Project Settings.


#include "PinAssetAction.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "UnrealEdGlobals.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "PinnedAssetSubsystem.h"

UPinAssetAction::UPinAssetAction()
{
	bIsActionForBlueprints = true;
	SupportedClasses.Add(UBlueprint::StaticClass());
}

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
}

void UPinAssetAction::GetSelectedAssets(TArray<FAssetData>& SelectedAssets)
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();

	ContentBrowserSingleton.GetSelectedAssets(SelectedAssets);
}
