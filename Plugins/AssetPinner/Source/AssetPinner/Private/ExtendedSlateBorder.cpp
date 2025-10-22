#include "ExtendedSlateBorder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
#include "PinnedAssetSubsystem.h"
#include <ContentBrowserModule.h>
#include "IContentBrowserSingleton.h"
#include <AssetRegistry/AssetRegistryModule.h>
#include "Internationalization/Text.h"

void SExtendedSlateBorder::Construct(const FArguments& InArgs)
{
	SBorder::Construct(SBorder::FArguments()
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.Padding(InArgs._Padding)
		.BorderImage(InArgs._BorderImage)
		.ContentScale(InArgs._ContentScale)
		.DesiredSizeScale(InArgs._DesiredSizeScale)
		.ColorAndOpacity(InArgs._ColorAndOpacity)
		.BorderBackgroundColor(InArgs._BorderBackgroundColor)
		.ForegroundColor(InArgs._ForegroundColor)
		.ShowEffectWhenDisabled(InArgs._ShowEffectWhenDisabled)
		.FlipForRightToLeftFlowDirection(InArgs._FlipForRightToLeftFlowDirection)
	);

	AssetPath = InArgs._AssetPath.Get();
}

FReply SExtendedSlateBorder::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition()))
		{
			// Right clicked, so summon a context menu if the cursor is within the widget
			FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();

			TSharedPtr<SWidget> MenuContentWidget = BuildContextMenuContent();
			if (MenuContentWidget.IsValid())
			{
				ActiveContextMenu.PrepareToSummon();
				ParentWidget = MouseEvent.GetWindow();
				static const bool bFocusImmediately = true;

				TSharedPtr<IMenu> ContextMenu;
				if (ParentWidget.IsValid())
				{
					ContextMenu = FSlateApplication::Get().PushMenu(
						ParentWidget.Pin().ToSharedRef(),
						WidgetPath,
						MenuContentWidget.ToSharedRef(),
						MouseEvent.GetScreenSpacePosition(),
						FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
						bFocusImmediately
					);
				}

				// Make sure the window is valid. It's possible for the parent to already be in the destroy queue, for example if the editable text was configured to dismiss it's window during OnTextCommitted.
				if (!ContextMenu.IsValid())
				{
					ActiveContextMenu.SummonFailed();
				}
				else
				{
					ContextMenuPosition = MouseEvent.GetScreenSpacePosition();
				}
			}
		}

		// Release mouse capture
		Reply = FReply::Handled();
	}

	return Reply;
}

TSharedPtr<SWidget> SExtendedSlateBorder::BuildContextMenuContent()
{
#define LOCTEXT_NAMESPACE "AssetPinnerContextMenu"
	// Set the menu to automatically close when the user commits to a choice
	const bool bShouldCloseWindowAfterMenuSelection = true;

	// This is a context menu which could be summoned from within another menu if this text block is in a menu
	// it should not close the menu it is inside
	bool bCloseSelfOnly = true;
	FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, TSharedPtr< const FUICommandList >(), nullptr, bCloseSelfOnly, &FCoreStyle::Get());
	{
		MenuBuilder.BeginSection("EditSection", LOCTEXT("Heading", "Edit Action"));
		{
			FUIAction PinAssetAction(
				FExecuteAction::CreateRaw(this, &SExtendedSlateBorder::Pin),
				FCanExecuteAction::CreateSP(this, &SExtendedSlateBorder::CanPin)
			);

			FUIAction UnpinAssetAction(
				FExecuteAction::CreateRaw(this, &SExtendedSlateBorder::Unpin)
			);

			FUIAction SetAlternativeNameAction(
				FExecuteAction::CreateRaw(this, &SExtendedSlateBorder::SetAlternativeName)
			);

			MenuBuilder.AddMenuEntry(
				NSLOCTEXT("AssetPinner", "PinAssetLabel", "Pin Asset"),
				NSLOCTEXT("AssetPinner", "PinAssetTooltip", "Move the asset to pinned section"),
				FSlateIcon(),
				PinAssetAction
			);

			MenuBuilder.AddMenuEntry(
				NSLOCTEXT("AssetPinner", "UnpinAssetLabel", "Unpin Asset"),
				NSLOCTEXT("AssetPinner", "UnpinAssetTooltip", "Remove the asset from pinned section"),
				FSlateIcon(),
				UnpinAssetAction
			);

			MenuBuilder.AddMenuEntry(
				NSLOCTEXT("AssetPinner", "RenameLabel", "Set Alternative Name"),
				NSLOCTEXT("AssetPinner", "RenameTooltip", "Set an alternative name to display"),
				FSlateIcon(),
				SetAlternativeNameAction
			);
		}
		MenuBuilder.EndSection();

		MenuBuilder.BeginSection("SearchSection", LOCTEXT("Heading", "Search Action"));
		{
			FUIAction LocateAssetAction(
				FExecuteAction::CreateRaw(this, &SExtendedSlateBorder::LocateInBrowser)
			);

			MenuBuilder.AddMenuEntry(
				NSLOCTEXT("AssetPinner", "LocateAssetLabel", "Locate Asset"),
				NSLOCTEXT("AssetPinner", "LocateAssetTooltip", "Locate the asset in the content browser"),
				FSlateIcon(),
				LocateAssetAction
			);
		}
		MenuBuilder.EndSection();
	}

	return MenuBuilder.MakeWidget();
#undef LOCTEXT_NAMESPACE
}

void SExtendedSlateBorder::OnContextMenuClosed(TSharedRef<IMenu> Menu)
{
	TSharedPtr<SWidget> OwnerSlateWidget(this);
	if (OwnerSlateWidget.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(OwnerSlateWidget, EFocusCause::OtherWidgetLostFocus);
	}
}

void SExtendedSlateBorder::OnRenamed(const FText& Text, ETextCommit::Type CommitType)
{
	AltNameHolder = Text.ToString().Replace(TEXT(" "), TEXT("_"));

	if (CommitType != ETextCommit::Type::OnEnter)
		return;

	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->RenamePinnedAsset(AssetPath, AltNameHolder);
}

void SExtendedSlateBorder::Pin()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->MoveAssetPath(AssetPath);
}


bool SExtendedSlateBorder::CanPin()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return false;

	// False mean unpinned, and we only want this option available then
	return !Subsystem->GetStatus(AssetPath);
}

void SExtendedSlateBorder::Unpin()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	// False mean unpinned, and we only want this option available then
	Subsystem->RemoveAssetPath(AssetPath);
}

void SExtendedSlateBorder::LocateInBrowser()
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

void SExtendedSlateBorder::SetAlternativeName()
{
#define LOCTEXT_NAMESPACE "AssetPinnerRenameMenu"
	// Set the menu to automatically close when the user commits to a choice
	const bool bShouldCloseWindowAfterMenuSelection = true;

	// This is a context menu which could be summoned from within another menu if this text block is in a menu
	// it should not close the menu it is inside
	bool bCloseSelfOnly = true;
	FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, TSharedPtr< const FUICommandList >(), nullptr, bCloseSelfOnly, &FCoreStyle::Get());
	{
		MenuBuilder.BeginSection("SetAltName", LOCTEXT("Heading", "Set Alternative Name"));

		MenuBuilder.EndSection();

		MenuBuilder.AddEditableText(
			NSLOCTEXT("AssetPinner", "RenameAssetLabel", "New Display Name"),
			NSLOCTEXT("AssetPinner", "RenameAssetToolTip", "Enter a new display name for the pinned asset"),
			FSlateIcon(),
			NSLOCTEXT("AssetPinner", "DefaultEditableText", ""),
			FOnTextCommitted::CreateRaw(this, &SExtendedSlateBorder::OnRenamed)
		);

		MenuBuilder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "ConfirmRenameLabel", "Confirm"),
			NSLOCTEXT("AssetPinner", "ConfirmRenameTooltip", "Confirm setting alternative name"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &SExtendedSlateBorder::ConfirmName))
		);

		MenuBuilder.AddSeparator();

		MenuBuilder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "CancelRenameLabel", "Cancel"),
			NSLOCTEXT("AssetPinner", "CancelRenameTooltip", "Cancel setting alternative name"),
			FSlateIcon(),
			FUIAction()
		);
	}

	TSharedPtr<SWidget> MenuContentWidget = MenuBuilder.MakeWidget();
	if (MenuContentWidget.IsValid() && ParentWidget.IsValid())
	{
		static const bool bFocusImmediately = true;
		TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
			ParentWidget.Pin().ToSharedRef(),
			FWidgetPath(),
			MenuContentWidget.ToSharedRef(),
			ContextMenuPosition,
			FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
			bFocusImmediately
		);
	}
#undef LOCTEXT_NAMESPACE
}

void SExtendedSlateBorder::ConfirmName()
{
	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->RenamePinnedAsset(AssetPath, AltNameHolder);
}
