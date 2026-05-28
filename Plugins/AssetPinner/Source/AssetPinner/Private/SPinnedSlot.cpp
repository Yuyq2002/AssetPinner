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
#include "SlotDragOperation.h"

SPinnedSlot::SPinnedSlot()
{
	BaseColor = FLinearColor(0.018f, 0.018f, 0.018f, 1);
	HoverColor = FLinearColor(0.527f, 0.527f, 0.527f, 1);
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
							.HAlign(HAlign_Center)
							[
								SAssignNew(Name, STextBlock)
									.Text(FText::FromString(FPackageName::GetShortName(*InArgs._Data.AssetPath)))
									.ColorAndOpacity(FLinearColor::White)
									.ShadowColorAndOpacity(FLinearColor::Black)
									.ShadowOffset(FVector2D(1.f, 1.f))
									.Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
							]
					]
			]
	];

	AssetPath = InArgs._Data.AssetPath;
	AssetData = InArgs._AssetData;
	OuterWidget = InArgs._Outer;

	PathType = InArgs._Data.PathType;

	ThumbnailHolder->SetContent(
		MakeThumbnailWidget(InArgs._Size).ToSharedRef()
	);
}

FReply SPinnedSlot::OnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		return FReply::Unhandled();

	if (PathType == EPathType::Folder)
	{
		TArray<FString> Assets{ AssetPath };

		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		ContentBrowserModule.Get().SyncBrowserToFolders(Assets);

		return FReply::Handled();
	}

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

void SPinnedSlot::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Background->SetBorderBackgroundColor(HoverColor);
}

void SPinnedSlot::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Background->SetBorderBackgroundColor(BaseColor);
}

FReply SPinnedSlot::OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), InMouseEvent.GetEffectingButton());
	}

	return FReply::Unhandled();
}

FReply SPinnedSlot::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		TSharedRef<FSlotDragOperation> DragOp = FSlotDragOperation::New(AssetData, nullptr, SharedThis(this).ToWeakPtr(), OuterWidget);
		DragOp->DragOffset = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

		SetVisibility(EVisibility::Collapsed);

		return FReply::Handled().BeginDragDrop(DragOp);
	}
	else
	{
		return FReply::Handled();
	}
}

void SPinnedSlot::SetSize(int Width, int Height)
{
	SizeBox->SetHeightOverride(Height);
	SizeBox->SetWidthOverride(Width);

	if (Thumbnail->GetSize().X != Width || Thumbnail->GetSize().Y != Width)
	{
		Thumbnail = MakeShared<FAssetThumbnail>(Thumbnail->GetAssetData(), Width, Width, UThumbnailManager::Get().GetSharedThumbnailPool());
		ThumbnailHolder->SetContent(
			Thumbnail->MakeThumbnailWidget()
		);
	}
}

TSharedPtr<SWidget> SPinnedSlot::MakeThumbnailWidget(float Size)
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return nullptr;

	if (PathType == EPathType::Folder)
	{
		return
			SNew(SImage)
			.Image(Subsystem->FolderIconBrush.Get());
	}
	else if (PathType == EPathType::Asset)
	{
		if(!Thumbnail.IsValid() || Thumbnail->GetSize().X != Size || Thumbnail->GetSize().Y != Size)
			Thumbnail = MakeShared<FAssetThumbnail>(AssetData, Size, Size, UThumbnailManager::Get().GetSharedThumbnailPool());
		
		return
			Thumbnail->MakeThumbnailWidget();
	}

	return SNew(STextBlock)
		.Text(FText::FromString("Unknown Type"));
}

FText SPinnedSlot::GetAssetName() const
{
	return FText::FromString(FPackageName::GetShortName(*AssetPath));
}

void SPinnedSlot::SetSlotState(int State)
{
	switch (State)
	{
		case 0: // Normal
			Background->SetBorderBackgroundColor(BaseColor);
			break;
		case 1: // Hovered
			Background->SetBorderBackgroundColor(HoverColor);
			break;
	default:
		break;
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