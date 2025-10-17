#include "ExtendedSlateBorder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"

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
}

FReply SExtendedSlateBorder::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	//// If the mouse is already captured, then don't allow a new action to be taken
	//if (!HasMouseCapture())
	//{
	//	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	//	{
	//		// Right clicking to summon context menu, but we'll do that on mouse-up.
	//		Reply = FReply::Handled();
	//		Reply.CaptureMouse(TSharedRef<SWidget>(this));
	//	}
	//}

	return Reply;
}

FReply SExtendedSlateBorder::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	//if (HasMouseCapture())
	{
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

				//	static const bool bFocusImmediately = true;
				//	TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
				//		MouseEvent.GetWindow(),
				//		WidgetPath,
				//		MenuContentWidget.ToSharedRef(),
				//		MouseEvent.GetScreenSpacePosition(),
				//		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
				//		bFocusImmediately
				//	);

				//	// Make sure the window is valid. It's possible for the parent to already be in the destroy queue, for example if the editable text was configured to dismiss it's window during OnTextCommitted.
				//	if (ContextMenu.IsValid())
				//	{
				//		ContextMenu->GetOnMenuDismissed().AddRaw(this, &SExtendedSlateBorder::OnContextMenuClosed);
				//		ActiveContextMenu.SummonSucceeded(ContextMenu.ToSharedRef());
				//	}
				//	else
				//	{
				//		ActiveContextMenu.SummonFailed();
				//	}
				}
			}

			// Release mouse capture
			Reply = FReply::Handled();
			//Reply.ReleaseMouseCapture();
		}
	}

	return Reply;
}

TSharedPtr<SWidget> SExtendedSlateBorder::BuildContextMenuContent()
{
#define LOCTEXT_NAMESPACE "EditableTextContextMenu"
	// Set the menu to automatically close when the user commits to a choice
	const bool bShouldCloseWindowAfterMenuSelection = true;

	// This is a context menu which could be summoned from within another menu if this text block is in a menu
	// it should not close the menu it is inside
	bool bCloseSelfOnly = true;
	FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, TSharedPtr< const FUICommandList >(), nullptr, bCloseSelfOnly, &FCoreStyle::Get());
	{
		MenuBuilder.BeginSection("Test", LOCTEXT("Heading", "TestSection"));
		{
			// Undo
			FUIAction ClearOutputLogAction(
				FExecuteAction::CreateRaw(this, &SExtendedSlateBorder::Test),
				FCanExecuteAction::CreateSP(this, &SExtendedSlateBorder::CanTest)
			);

			MenuBuilder.AddMenuEntry(
				NSLOCTEXT("OutputLog", "ClearLogLabel", "Clear Log"),
				NSLOCTEXT("OutputLog", "ClearLogTooltip", "Clears all log messages"),
				FSlateIcon(),
				ClearOutputLogAction
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

void SExtendedSlateBorder::Test()
{

}


bool SExtendedSlateBorder::CanTest()
{
	return true;
}
