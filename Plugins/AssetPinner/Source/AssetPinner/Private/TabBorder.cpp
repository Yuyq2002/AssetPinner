#include "TabBorder.h"
#include "TabBorder.h"
#include "TabBorder.h"
#include "ExtendedSlateBorder.h"
#include "Components/BorderSlot.h"
#include "PinnedAssetSlotBase.h"
#include <ContentBrowserModule.h>
#include "IContentBrowserSingleton.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Components/ComboBox.h"
#include "PinnedAssetSubsystem.h"

UTabBorder::UTabBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}


TSharedRef<SWidget> UTabBorder::RebuildWidget()
{
	MyBorder = SNew(SExtendedSlateBorder)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection)
		.ContextMenuExtender(FMenuExtensionDelegate::CreateUObject(this, &UTabBorder::ExtendContextMenu));

	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}

void UTabBorder::ExtendContextMenu(FMenuBuilder& Builder)
{
#define LOCTEXT_NAMESPACE "TabContextMenu"
	Builder.BeginSection("EditSection", LOCTEXT("Heading", "Edit Action"));
	{
		FUIAction RenameTabAction(
			FExecuteAction::CreateUObject(this, &UTabBorder::OpenRenameBox),
			FCanExecuteAction::CreateUObject(this, &UTabBorder::CanEdit)
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "TabRenameLabel", "Rename"),
			NSLOCTEXT("AssetPinner", "TabRenameTooltip", "Rename the tab"),
			FSlateIcon(),
			RenameTabAction
		);

		FUIAction RemoveTabAction(
			FExecuteAction::CreateUObject(this, &UTabBorder::RemoveTab),
			FCanExecuteAction::CreateUObject(this, &UTabBorder::CanEdit)
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "TabRemoveLabel", "Remove"),
			NSLOCTEXT("AssetPinner", "TabRemoveTooltip", "Remove the tab"),
			FSlateIcon(),
			RemoveTabAction
		);
	}
	Builder.EndSection();
#undef LOCTEXT_NAMESPACE
}

void UTabBorder::OpenRenameBox()
{
	OnRenameClickedDelegate.ExecuteIfBound();
}

bool UTabBorder::CanEdit()
{
	if(CheckCanRenameDelegate.IsBound())
		return !CheckCanRenameDelegate.Execute();
	return false;
}

void UTabBorder::RemoveTab()
{
	OnRemoveClickedDelegate.ExecuteIfBound();
}
