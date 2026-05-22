// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetPinner.h"
#include "ToolMenus.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EditorUtilitySubsystem.h"
#include <ContentBrowserModule.h>
#include "PinAssetAction.h"
#include <LevelEditor.h>
#include "PinnedAssetSubsystem.h"
#include "Enums.h"
#define LOCTEXT_NAMESPACE "FAssetPinnerModule"

void FAssetPinnerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetPinnerModule::RegisterMenuExtension));
    AddContentBrowserContextMenuExtender();
}

void FAssetPinnerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

        // Unregister the startup function
    UToolMenus::UnRegisterStartupCallback(this);

    // Unregister all our menu extensions
    UToolMenus::UnregisterOwner(this);

    RemoveContentBrowserContextMenuExtender();
}

void FAssetPinnerModule::RegisterMenuExtension()
{
    // Use the current object as the owner of the menus
    // This allows us to remove all our custom menus when the
    // module is unloaded (see ShutdownModule below)
    FToolMenuOwnerScoped OwnerScoped(this);

    // Extend the "File" section of the main toolbar
    UToolMenu* AssetsToolBar = UToolMenus::Get()->ExtendMenu(
        "LevelEditor.LevelEditorToolBar.ModesToolBar");
    FToolMenuSection& ToolbarSection = AssetsToolBar->FindOrAddSection("File");

    ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(
        TEXT("AssetPinner"),
        FExecuteAction::CreateLambda([]()
            {
                UObject* WidgetObj = LoadObject<UObject>(nullptr, TEXT("/AssetPinner/BPW_PinnedAssetWindow"));
                UEditorUtilityWidgetBlueprint* WidgetBP = Cast<UEditorUtilityWidgetBlueprint>(WidgetObj);

                if (WidgetBP)
                {
                    UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
                    FName ID;
                    EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(WidgetBP, ID);

                    //FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
                    //if (TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager())
                    //{
                    //    TSharedPtr<SDockTab> Tab = LevelEditorTabManager->FindExistingLiveTab(FTabId(ID));
                    //    Tab->context
                    //}
                }
            }),
        INVTEXT("Asset Pinner"),
        INVTEXT("Open the asset pinner"),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "ViewportActorPreview.Pinned")
    ));

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
    MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateRaw(this, &FAssetPinnerModule::AddMenuExtention));

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FAssetPinnerModule::AddContentBrowserContextMenuExtender()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetExtenderDelegates = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
    TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuPathExtenderDelegates = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

    CBMenuAssetExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&OnExtendContentBrowserAssetSelectionMenu));
    ContentBrowserAssetExtenderDelegateHandle = CBMenuAssetExtenderDelegates.Last().GetHandle();
    CBMenuPathExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedPaths::CreateStatic(&OnExtendContentBrowserPathSelectionMenu));
    ContentBrowserPathExtenderDelegateHandle = CBMenuPathExtenderDelegates.Last().GetHandle();
}

void FAssetPinnerModule::RemoveContentBrowserContextMenuExtender()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

    TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuExtenderDelegates = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
    CBMenuExtenderDelegates.RemoveAll([this](const FContentBrowserMenuExtender_SelectedAssets& Delegate) { return Delegate.GetHandle() == ContentBrowserAssetExtenderDelegateHandle; });

    TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuPathExtenderDelegates = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
    CBMenuPathExtenderDelegates.RemoveAll([this](const FContentBrowserMenuExtender_SelectedPaths& Delegate) { return Delegate.GetHandle() == ContentBrowserPathExtenderDelegateHandle; });
}

void FAssetPinnerModule::AddMenuExtention(FMenuBuilder& MenuBuilder)
{
    MenuBuilder.BeginSection("Test section", LOCTEXT("ASSET_CONTEXT", "Pin Asset"));
    {
        // Add Menu Entry Here
        MenuBuilder.AddMenuEntry(
            LOCTEXT("ButtonName", "Test"),
            LOCTEXT("Button ToolTip", "A test button"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "ViewportActorPreview.Pinned"),
            FUIAction(FExecuteAction::CreateLambda([]()
                {
                    FAssetPinnerModule::PrintString();
                })),
            NAME_None,
            EUserInterfaceActionType::Button);
    }
    MenuBuilder.EndSection();
}

TSharedRef<FExtender> FAssetPinnerModule::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
{
    TSharedRef<FExtender> Extender = MakeShared<FExtender>();
    Extender->AddMenuExtension(
        "AssetContextCollections",
        EExtensionHook::After,
        nullptr,
        FMenuExtensionDelegate::CreateStatic(&ExecutePinAsset, SelectedAssets)
    );
    return Extender;
}

TSharedRef<FExtender> FAssetPinnerModule::OnExtendContentBrowserPathSelectionMenu(const TArray<FString>& SelectedAssets)
{
    TSharedRef<FExtender> Extender = MakeShared<FExtender>();
    Extender->AddMenuExtension(
        "PathContextBulkOperations",
        EExtensionHook::After,
        nullptr,
        FMenuExtensionDelegate::CreateStatic(&ExecutePinPath, SelectedAssets)
    );
    return Extender;
}

void FAssetPinnerModule::ExecutePinAsset(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets)
{
	MenuBuilder.BeginSection("Pin Asset", LOCTEXT("ASSET_CONTEXT", "Pin Asset"));
	{
		// Add Menu Entry Here
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ButtonName", "Pin"),
			LOCTEXT("Button ToolTip", "Pin Asset"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "ViewportActorPreview.Pinned"),
			FUIAction(FExecuteAction::CreateLambda([SelectedAssets]()
				{
                    for (auto& AssetData : SelectedAssets)
                    {
                        FString AssetPath = AssetData.PackageName.ToString();

                        UPinnedAssetSubsystem* Subsystem = nullptr;
                        if (GEditor)
                            Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();

                        if (Subsystem)
                            Subsystem->AddAssetPath(AssetPath);
                    }
				})),
			NAME_None,
			EUserInterfaceActionType::Button);
	}
	MenuBuilder.EndSection();
}

void FAssetPinnerModule::ExecutePinPath(FMenuBuilder& MenuBuilder, const TArray<FString> SelectedAssets)
{
    MenuBuilder.BeginSection("Pin Asset", LOCTEXT("ASSET_CONTEXT", "Pin Asset"));
    {
        // Add Menu Entry Here
        MenuBuilder.AddMenuEntry(
            LOCTEXT("ButtonName", "Pin Path"),
            LOCTEXT("Button ToolTip", "Pin Path"),
            FSlateIcon(FAppStyle::GetAppStyleSetName(), "ViewportActorPreview.Pinned"),
            FUIAction(FExecuteAction::CreateLambda([SelectedAssets]()
                {
                    UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
                    if (Subsystem)
                        Subsystem->AddAssetPath(SelectedAssets[0], EPathType::Folder);
                })),
            NAME_None,
            EUserInterfaceActionType::Button);
    }
    MenuBuilder.EndSection();
}

void FAssetPinnerModule::PrintString()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssetPinnerModule, AssetPinner)