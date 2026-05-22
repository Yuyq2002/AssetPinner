#include "ExtendedSlateBorder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
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

	ContextMenuExtender = InArgs._ContextMenuExtender;
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
				static const bool bFocusImmediately = true;

				TSharedPtr<IMenu> ContextMenu;

					ContextMenu = FSlateApplication::Get().PushMenu(
						MouseEvent.GetWindow(),
						WidgetPath,
						MenuContentWidget.ToSharedRef(),
						MouseEvent.GetScreenSpacePosition(),
						FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
						bFocusImmediately
					);
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
		ContextMenuExtender.ExecuteIfBound(MenuBuilder);
	}

	return MenuBuilder.MakeWidget();
#undef LOCTEXT_NAMESPACE
}

