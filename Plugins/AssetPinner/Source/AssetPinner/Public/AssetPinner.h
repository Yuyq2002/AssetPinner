// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

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
	static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);
	static void ExecutePinAsset(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets);

private:
	FDelegateHandle ContentBrowserExtenderDelegateHandle;

};
