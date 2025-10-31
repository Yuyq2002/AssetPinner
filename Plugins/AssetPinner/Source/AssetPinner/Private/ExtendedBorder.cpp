#include "ExtendedBorder.h"
#include "ExtendedSlateBorder.h"
#include "Components/BorderSlot.h"
#include "PinnedAssetSlotBase.h"
#include <ContentBrowserModule.h>
#include "IContentBrowserSingleton.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Components/ComboBox.h"

UExtendedBorder::UExtendedBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{

}


TSharedRef<SWidget> UExtendedBorder::RebuildWidget()
{
	MyBorder = SNew(SExtendedSlateBorder)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection)
		.ContextMenuExtender(FMenuExtensionDelegate::CreateUObject(this, &UExtendedBorder::ExtendContextMenu));

	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}

void UExtendedBorder::ExtendContextMenu(FMenuBuilder& Builder)
{
#define LOCTEXT_NAMESPACE "AssetPinnerContextMenu"
	Builder.BeginSection("EditSection", LOCTEXT("Heading", "Edit Action"));
	{
		FUIAction PinAssetAction(
			FExecuteAction::CreateUObject(this, &UExtendedBorder::Pin),
			FCanExecuteAction::CreateUObject(this, &UExtendedBorder::CanPin)
		);

		FUIAction UnpinAssetAction(
			FExecuteAction::CreateUObject(this, &UExtendedBorder::Unpin)
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "PinAssetLabel", "Pin Asset"),
			NSLOCTEXT("AssetPinner", "PinAssetTooltip", "Move the asset to pinned section"),
			FSlateIcon(),
			PinAssetAction
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "UnpinAssetLabel", "Unpin Asset"),
			NSLOCTEXT("AssetPinner", "UnpinAssetTooltip", "Remove the asset from pinned section"),
			FSlateIcon(),
			UnpinAssetAction
		);

		FMenuBuilder TabMenuBuilder(true, TSharedPtr< const FUICommandList >(), nullptr, false, &FCoreStyle::Get());

		Builder.AddSubMenu(
			NSLOCTEXT("AssetPinner", "MoveAssetLabel", "Move Asset"),
			NSLOCTEXT("AssetPinner", "MoveAssetTooltip", "Move the asset to another tab"),
			FNewMenuDelegate::CreateUObject(this, &UExtendedBorder::GenerateTabSubMenu)
		);
	}
	Builder.EndSection();

	Builder.BeginSection("SearchSection", LOCTEXT("Heading", "Search Action"));
	{
		FUIAction LocateAssetAction(
			FExecuteAction::CreateUObject(this, &UExtendedBorder::LocateInBrowser)
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "LocateAssetLabel", "Locate Asset"),
			NSLOCTEXT("AssetPinner", "LocateAssetTooltip", "Locate the asset in the content browser"),
			FSlateIcon(),
			LocateAssetAction
		);
	}
	Builder.EndSection();
#undef LOCTEXT_NAMESPACE
}

void UExtendedBorder::Pin()
{
	UPinnedAssetSubsystem* Subsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->MoveAssetPath(AssetPath, 0);
}


bool UExtendedBorder::CanPin()
{
	UPinnedAssetSubsystem* Subsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return false;

	// False mean unpinned, and we only want this option available then
	return !Subsystem->GetStatus(AssetPath);
}

void UExtendedBorder::Unpin()
{
	UPinnedAssetSubsystem* Subsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	// False mean unpinned, and we only want this option available then
	Subsystem->RemoveAssetPath(AssetPath);
}

void UExtendedBorder::LocateInBrowser()
{
	UPinnedAssetSubsystem* Subsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	if (Subsystem->GetPathType(AssetPath) == EPathType::Folder)
	{
		TArray<FString> Path{ AssetPath };
		ContentBrowserModule.Get().SyncBrowserToFolders(Path);
	}
	else
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		TArray<FAssetData> Assets;
		AssetRegistry.GetAssetsByPackageName(FName(AssetPath), Assets);
		if (Assets.Num() <= 0)
			return;

		ContentBrowserModule.Get().SyncBrowserToAssets(Assets);
	}
}

void UExtendedBorder::SwitchTab(int Index)
{
	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->MoveAssetPath(AssetPath, Index);
}

void UExtendedBorder::GenerateTabSubMenu(FMenuBuilder& Builder)
{
	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	TArray<FString> Names = PinnedAssetSubsystem->GetTabNames();

	for (int i = 0; i < Names.Num(); i++)
	{
		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "SwitchTabLabel", "Move To " + Names[i]),
			NSLOCTEXT("AssetPinner", "SwitchTabTooltip", "Move Asset To " + Names[i] + " Tab"),
			FSlateIcon(),
			FExecuteAction::CreateUObject(this, &UExtendedBorder::SwitchTab, i)
		);
	}
}
