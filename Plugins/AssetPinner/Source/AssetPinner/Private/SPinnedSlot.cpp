#include "SPinnedSlot.h"

#include "ExtendedSlateBorder.h"
#include <PinnedAssetSubsystem.h>
#include <ContentBrowserModule.h>
#include "Components/BorderSlot.h"
#include "IContentBrowserSingleton.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Components/ComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "AssetThumbnail.h"
#include "ThumbnailRendering/ThumbnailManager.h"

SPinnedSlot::SPinnedSlot()
{
}

void SPinnedSlot::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SAssignNew(SizeBox, SBox)
			.WidthOverride(InArgs._Size)
			.HeightOverride(InArgs._Size * 1.25f)
			[
				SAssignNew(Background, SExtendedSlateBorder)
					.BorderBackgroundColor(BaseColor)
					.ContextMenuExtender(this, &SPinnedSlot::BuildSlotContextMenu)
					[
						SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.FillHeight(10)
							[
								SAssignNew(ThumbnailHolder, SBox)
									.WidthOverride(InArgs._Size)
									.HeightOverride(InArgs._Size)
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
							]
							+SVerticalBox::Slot()
							.FillHeight(2.5)
							[
								SAssignNew(Name, STextBlock)
									.Text(FText::FromString("None"))
									.ColorAndOpacity(FLinearColor::White)
									.ShadowColorAndOpacity(FLinearColor::Black)
									.ShadowOffset(FVector2D(1.f, 1.f))
									.Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
							]
					]
			]
	];

	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	if (PathType == EPathType::Folder)
	{
		ThumbnailHolder->SetContent(
			SNew(SImage)
			.Image(Subsystem->FolderIconBrush.Get())
		);
	}
	else if (PathType == EPathType::Asset)
	{
		Thumbnail = MakeShared<FAssetThumbnail>(InArgs._AssetData, 100, 100, UThumbnailManager::Get().GetSharedThumbnailPool());
		ThumbnailHolder->SetContent(
			Thumbnail->MakeThumbnailWidget()
		);
	}

	AssetPath = InArgs._Data.AssetPath;

	if (Name)
		Name->SetText(FText::FromString(FPackageName::GetShortName(*InArgs._Data.AssetPath)));

	PathType = InArgs._Data.PathType;
}

void SPinnedSlot::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

}

FReply SPinnedSlot::OnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Unhandled();
}

void SPinnedSlot::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

}

void SPinnedSlot::OnMouseLeave(const FPointerEvent& InMouseEvent)
{

}

FReply SPinnedSlot::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Unhandled();
}

FReply SPinnedSlot::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}

void SPinnedSlot::SetSize(int Width, int Height)
{
	SizeBox->SetHeightOverride(Height);
	SizeBox->SetWidthOverride(Width);

	if (Thumbnail->GetSize().X != Width || Thumbnail->GetSize().Y != Height)
	{
		Thumbnail = MakeShared<FAssetThumbnail>(Thumbnail->GetAssetData(), 100, 100, UThumbnailManager::Get().GetSharedThumbnailPool());
		ThumbnailHolder->SetContent(
			Thumbnail->MakeThumbnailWidget()
		);
	}
}

void SPinnedSlot::BuildSlotContextMenu(FMenuBuilder& Builder)
{
#define LOCTEXT_NAMESPACE "AssetPinnerContextMenu"
	Builder.BeginSection("EditSection", LOCTEXT("Heading", "Edit Action"));
	{
		FUIAction PinAssetAction(
			FExecuteAction::CreateRaw(this, &SPinnedSlot::Pin),
			FCanExecuteAction::CreateRaw(this, &SPinnedSlot::CanPin)
		);

		FUIAction UnpinAssetAction(
			FExecuteAction::CreateRaw(this, &SPinnedSlot::Unpin)
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
			FNewMenuDelegate::CreateRaw(this, &SPinnedSlot::GenerateTabSubMenu)
		);
	}
	Builder.EndSection();

	Builder.BeginSection("SearchSection", LOCTEXT("Heading", "Search Action"));
	{
		FUIAction LocateAssetAction(
			FExecuteAction::CreateRaw(this, &SPinnedSlot::LocateInBrowser)
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

void SPinnedSlot::Pin()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->MoveAssetPath(AssetPath, 0);
}


bool SPinnedSlot::CanPin()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return false;

	// False mean unpinned, and we only want this option available then
	return !Subsystem->GetStatus(AssetPath);
}

void SPinnedSlot::Unpin()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	// False mean unpinned, and we only want this option available then
	Subsystem->RemoveAssetPath(AssetPath);
}

void SPinnedSlot::LocateInBrowser()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
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

void SPinnedSlot::SwitchTab(int Index)
{
	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->MoveAssetPath(AssetPath, Index);
}

void SPinnedSlot::GenerateTabSubMenu(FMenuBuilder& Builder)
{
	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	TArray<FString> Names = PinnedAssetSubsystem->GetTabNames();

	for (int i = 0; i < Names.Num(); i++)
	{
		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "SwitchTabLabel", "Move To " + Names[i]),
			NSLOCTEXT("AssetPinner", "SwitchTabTooltip", "Move Asset To " + Names[i] + " Tab"),
			FSlateIcon(),
			FExecuteAction::CreateRaw(this, &SPinnedSlot::SwitchTab, i)
		);
	}
}