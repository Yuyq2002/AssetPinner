// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

class FAssetPinnerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenuExtension();

	void AddContentBrowserContextMenuExtender();
	void RemoveContentBrowserContextMenuExtender();
	void AddMenuExtention(FMenuBuilder& MenuBuilder);
	static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);
	static TSharedRef<FExtender> OnExtendContentBrowserPathSelectionMenu(const TArray<FString>& SelectedAssets);
	static void ExecutePinAsset(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets);
	static void ExecutePinPath(FMenuBuilder& MenuBuilder, const TArray<FString> SelectedAssets);
	static void PrintString();
	TSharedRef<class SDockTab> OnSpawnAssetPinner(const class FSpawnTabArgs& SpawnTabArgs);

private:
	FDelegateHandle ContentBrowserPathExtenderDelegateHandle;
	FDelegateHandle ContentBrowserAssetExtenderDelegateHandle;
};
