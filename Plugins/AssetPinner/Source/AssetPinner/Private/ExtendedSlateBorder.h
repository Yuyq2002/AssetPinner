#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UPinnedWindowBase;

class SExtendedSlateBorder : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SExtendedSlateBorder)
		: _Content()
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _Padding(FMargin(2.0f))
		, _OnMouseButtonDown()
		, _OnMouseButtonUp()
		, _OnMouseMove()
		, _OnMouseDoubleClick()
		, _BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		, _ContentScale(FVector2D(1, 1))
		, _DesiredSizeScale(FVector2D(1, 1))
		, _ColorAndOpacity(FLinearColor(1, 1, 1, 1))
		, _BorderBackgroundColor(FLinearColor::Black)
		, _ForegroundColor(FSlateColor::UseForeground())
		, _ShowEffectWhenDisabled(true)
		, _FlipForRightToLeftFlowDirection(false)
		{
		}

		SLATE_DEFAULT_SLOT(FArguments, Content)

		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)
		SLATE_ATTRIBUTE(FMargin, Padding)

		SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)
		SLATE_EVENT(FPointerEventHandler, OnMouseButtonUp)
		SLATE_EVENT(FPointerEventHandler, OnMouseMove)
		SLATE_EVENT(FPointerEventHandler, OnMouseDoubleClick)

		SLATE_ATTRIBUTE(const FSlateBrush*, BorderImage)

		SLATE_ATTRIBUTE(FVector2D, ContentScale)

		SLATE_ATTRIBUTE(FVector2D, DesiredSizeScale)

		/** ColorAndOpacity is the color and opacity of content in the border */
		SLATE_ATTRIBUTE(FLinearColor, ColorAndOpacity)
		/** BorderBackgroundColor refers to the actual color and opacity of the supplied border image.*/
		SLATE_ATTRIBUTE(FSlateColor, BorderBackgroundColor)
		/** The foreground color of text and some glyphs that appear as the border's content. */
		SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)
		/** Whether or not to show the disabled effect when this border is disabled */
		SLATE_ATTRIBUTE(bool, ShowEffectWhenDisabled)

		/** Flips the background image if the localization's flow direction is RightToLeft */
		SLATE_ARGUMENT(bool, FlipForRightToLeftFlowDirection)

		/** Whether the context menu can be opened  */
		SLATE_ATTRIBUTE(bool, AllowContextMenu)

		SLATE_EVENT(FMenuExtensionDelegate, ContextMenuExtender)
	SLATE_END_ARGS()

	/** See the AllowContextMenu attribute */
	void SetAllowContextMenu(const TAttribute< bool >& InAllowContextMenu);

	void Construct(const FArguments& InArgs);

protected:
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	TSharedPtr<SWidget> BuildContextMenuContent();

protected:
	/** Whether to disable the context menu */
	TAttribute< bool > AllowContextMenu;

	FMenuExtensionDelegate ContextMenuExtender;
};