// Fill out your copyright notice in the Description page of Project Settings.


#include "SVerticalEditableText.h"
//#include "Framework/Text/TextEditHelper.h"
#include "Framework/Text/PlainTextLayoutMarshaller.h"
#include "Widgets/Text/SlateEditableTextLayout.h"
#include "Types/ReflectionMetadata.h"
#include "Types/TrackedMetaData.h"
#include <Enums.h>

#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateAccessibleWidgets.h"
#endif

SVerticalEditableText::SVerticalEditableText()
{
#if WITH_ACCESSIBILITY
	AccessibleBehavior = EAccessibleBehavior::Auto;
	bCanChildrenBeAccessible = false;
#endif
}

SVerticalEditableText::~SVerticalEditableText()
{
	// Needed to avoid "deletion of pointer to incomplete type 'FSlateEditableTextLayout'; no destructor called" error when using TUniquePtr
}

void SVerticalEditableText::Construct(const FArguments& InArgs)
{
	Rotation = InArgs._Rotation;

	bIsReadOnly = InArgs._IsReadOnly;
	bIsPassword = InArgs._IsPassword;

	bIsCaretMovedWhenGainFocus = InArgs._IsCaretMovedWhenGainFocus;
	bSelectAllTextWhenFocused = InArgs._SelectAllTextWhenFocused;
	bRevertTextOnEscape = InArgs._RevertTextOnEscape;
	bClearKeyboardFocusOnCommit = InArgs._ClearKeyboardFocusOnCommit;
	bAllowContextMenu = InArgs._AllowContextMenu;
	OnContextMenuOpening = InArgs._OnContextMenuOpening;
	OnIsTypedCharValid = InArgs._OnIsTypedCharValid;
	OnTextChangedCallback = InArgs._OnTextChanged;
	OnTextCommittedCallback = InArgs._OnTextCommitted;
	MinDesiredWidth = InArgs._MinDesiredWidth;
	bSelectAllTextOnCommit = InArgs._SelectAllTextOnCommit;
	bSelectWordOnMouseDoubleClick = InArgs._SelectWordOnMouseDoubleClick;
	VirtualKeyboardType = InArgs._VirtualKeyboardType;
	VirtualKeyboardOptions = InArgs._VirtualKeyboardOptions;
	VirtualKeyboardTrigger = InArgs._VirtualKeyboardTrigger;
	VirtualKeyboardDismissAction = InArgs._VirtualKeyboardDismissAction;
	OnKeyCharHandler = InArgs._OnKeyCharHandler;
	OnKeyDownHandler = InArgs._OnKeyDownHandler;
	bEnableIntegratedKeyboard = InArgs._EnableIntegratedKeyboard;

	Font = InArgs._Font;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	BackgroundImageSelected = InArgs._BackgroundImageSelected;

	// We use the given style when creating the text layout as it may not be safe to call the override delegates until we've finished being constructed
	// The first call to SynchronizeTextStyle will apply the correct overrides, and that will happen before the first paint
	check(InArgs._Style);
	FTextBlockStyle TextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	TextStyle.Font = InArgs._Style->Font;
	TextStyle.ColorAndOpacity = InArgs._Style->ColorAndOpacity;
	TextStyle.HighlightShape = InArgs._Style->BackgroundImageSelected;

	PlainTextMarshaller = FPlainTextLayoutMarshaller::Create();
	PlainTextMarshaller->SetIsPassword(bIsPassword);

	// We use a separate marshaller for the hint text, as that should never be displayed as a password
	TSharedRef<FPlainTextLayoutMarshaller> HintTextMarshaller = FPlainTextLayoutMarshaller::Create();

	EditableTextLayout = MakeUnique<FSlateEditableTextLayout>(*this, InArgs._Text, TextStyle, InArgs._TextShapingMethod, InArgs._TextFlowDirection, FCreateSlateTextLayout(), PlainTextMarshaller.ToSharedRef(), HintTextMarshaller);
	EditableTextLayout->SetHintText(InArgs._HintText);
	EditableTextLayout->SetSearchText(InArgs._SearchText);
	EditableTextLayout->SetCursorBrush(InArgs._CaretImage.IsSet() ? InArgs._CaretImage : &InArgs._Style->CaretImage);
	EditableTextLayout->SetCompositionBrush(InArgs._BackgroundImageComposing.IsSet() ? InArgs._BackgroundImageComposing : &InArgs._Style->BackgroundImageComposing);
	EditableTextLayout->SetDebugSourceInfo(TAttribute<FString>::Create(TAttribute<FString>::FGetter::CreateLambda([this] { return FReflectionMetaData::GetWidgetDebugInfo(this); })));
	EditableTextLayout->SetJustification(InArgs._Justification);
	EditableTextLayout->SetOverflowPolicy(InArgs._OverflowPolicy);

	// build context menu extender
	MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuExtension("EditText", EExtensionHook::Before, TSharedPtr<FUICommandList>(), InArgs._ContextMenuExtender);

	AddMetadata(MakeShared<FTrackedMetaData>(this, FName(TEXT("EditableText"))));
}

void SVerticalEditableText::SetText(const TAttribute< FText >& InNewText)
{
	EditableTextLayout->SetText(InNewText);
}

FText SVerticalEditableText::GetText() const
{
	return EditableTextLayout->GetText();
}

bool SVerticalEditableText::SetEditableText(const FText& InNewText)
{
	return EditableTextLayout->SetEditableText(InNewText);
}

void SVerticalEditableText::SetFont(const TAttribute< FSlateFontInfo >& InNewFont)
{
	Font = InNewFont;

	Invalidate(EInvalidateWidgetReason::Layout);
}

FSlateFontInfo SVerticalEditableText::GetFont() const
{
	return Font.Get();
}

void SVerticalEditableText::SetTextStyle(const FEditableTextStyle& InNewTextStyle)
{
	Font = InNewTextStyle.Font;
	ColorAndOpacity = InNewTextStyle.ColorAndOpacity;
	BackgroundImageSelected = &InNewTextStyle.BackgroundImageSelected;

	Invalidate(EInvalidateWidgetReason::Layout);
}

void SVerticalEditableText::SetTextBlockStyle(const FTextBlockStyle* InTextStyle)
{
	if (InTextStyle)
	{
		EditableTextLayout->SetTextStyle(*InTextStyle);
		Invalidate(EInvalidateWidgetReason::Layout); //Using Layout as changing text block size can affect the size.
	}
}

void SVerticalEditableText::SetRotation(TAttribute<ERotation> InRotation)
{
	Rotation = InRotation;
}

void SVerticalEditableText::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	EditableTextLayout->Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

int32 SVerticalEditableText::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	const FVector2D DesiredHorizontalTextSize = EditableTextLayout->GetSize();
	const FVector2D ActualHorizontalTextSize(FMath::Min(DesiredHorizontalTextSize.X, LocalSize.Y), FMath::Min(DesiredHorizontalTextSize.Y, LocalSize.X));

	// Now determine the center of the vertical text by rotating the dimensions of the horizontal text.
	// The center should align it to the top of the widget.
	const FVector2D VerticalTextSize(ActualHorizontalTextSize.Y, ActualHorizontalTextSize.X);
	const FVector2D VerticalTextCenter = VerticalTextSize / 2.0f;

	// Now determine where the horizontal text should be positioned so that it is centered on the vertical text:
	//      +-+
	//      |v|
	//      |e|
	// [ horizontal ]
	//      |r|
	//      |t|
	//      +-+
	const FVector2D HorizontalTextPosition = VerticalTextCenter - ActualHorizontalTextSize / 2.0f;

	// Define the text's geometry using the horizontal bounds, then rotate it 90/-90 degrees into place to become vertical.
	const FSlateRenderTransform RotationTransform(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(Rotation.Get() == ERotation::Clockwise ? 90 : -90))));
	const FGeometry TextGeometry = AllottedGeometry.MakeChild(ActualHorizontalTextSize, FSlateLayoutTransform(HorizontalTextPosition), RotationTransform, FVector2D(0.5f, 0.5f));

	const FTextBlockStyle& EditableTextStyle = EditableTextLayout->GetTextStyle();
	const FLinearColor ForegroundColor = EditableTextStyle.ColorAndOpacity.GetColor(InWidgetStyle);

	FWidgetStyle TextWidgetStyle = FWidgetStyle(InWidgetStyle)
		.SetForegroundColor(ForegroundColor);

	LayerId = EditableTextLayout->OnPaint(Args, TextGeometry, MyCullingRect, OutDrawElements, LayerId, TextWidgetStyle, ShouldBeEnabled(bParentEnabled));

	return LayerId;
}

void SVerticalEditableText::CacheDesiredSize(float LayoutScaleMultiplier)
{
	SynchronizeTextStyle();
	EditableTextLayout->CacheDesiredSize(LayoutScaleMultiplier);
	SWidget::CacheDesiredSize(LayoutScaleMultiplier);
}

FVector2D SVerticalEditableText::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	EditableTextLayout->ComputeDesiredSize(LayoutScaleMultiplier);
	FVector2D TextLayoutSize = EditableTextLayout->GetSize();
	TextLayoutSize = FVector2D(TextLayoutSize.Y, FMath::Max(TextLayoutSize.X, MinDesiredWidth.Get()));
	return TextLayoutSize;
}

FChildren* SVerticalEditableText::GetChildren()
{
	return EditableTextLayout->GetChildren();
}

void SVerticalEditableText::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	EditableTextLayout->OnArrangeChildren(AllottedGeometry, ArrangedChildren);
}

FReply SVerticalEditableText::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FExternalDragOperation> DragDropOp = DragDropEvent.GetOperationAs<FExternalDragOperation>();
	if (DragDropOp.IsValid())
	{
		if (DragDropOp->HasText())
		{
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply SVerticalEditableText::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FExternalDragOperation> DragDropOp = DragDropEvent.GetOperationAs<FExternalDragOperation>();
	if (DragDropOp.IsValid())
	{
		if (DragDropOp->HasText())
		{
			EditableTextLayout->SetText(FText::FromString(DragDropOp->GetText()));
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

bool SVerticalEditableText::SupportsKeyboardFocus() const
{
	return true;
}

FReply SVerticalEditableText::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	EditableTextLayout->HandleFocusReceived(InFocusEvent);
	return FReply::Handled();
}

void SVerticalEditableText::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	EditableTextLayout->HandleFocusLost(InFocusEvent);
}

FReply SVerticalEditableText::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	FReply Reply = FReply::Unhandled();

	// First call the user defined key handler, there might be overrides to normal functionality
	if (OnKeyCharHandler.IsBound())
	{
		Reply = OnKeyCharHandler.Execute(MyGeometry, InCharacterEvent);
	}

	if (!Reply.IsEventHandled())
	{
		Reply = EditableTextLayout->HandleKeyChar(InCharacterEvent);
	}

	return Reply;
}

FReply SVerticalEditableText::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();

	// First call the user defined key handler, there might be overrides to normal functionality
	if (OnKeyDownHandler.IsBound())
	{
		Reply = OnKeyDownHandler.Execute(MyGeometry, InKeyEvent);
	}

	if (!Reply.IsEventHandled())
	{
		Reply = EditableTextLayout->HandleKeyDown(InKeyEvent);

		if (!Reply.IsEventHandled())
		{
			Reply = SWidget::OnKeyDown(MyGeometry, InKeyEvent);
		}
	}

	return Reply;
}

FReply SVerticalEditableText::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	return EditableTextLayout->HandleKeyUp(InKeyEvent);
}

FReply SVerticalEditableText::OnMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return EditableTextLayout->HandleMouseButtonDown(InMyGeometry, InMouseEvent);
}

FReply SVerticalEditableText::OnMouseButtonUp(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return EditableTextLayout->HandleMouseButtonUp(InMyGeometry, InMouseEvent);
}

FReply SVerticalEditableText::OnMouseMove(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return EditableTextLayout->HandleMouseMove(InMyGeometry, InMouseEvent);
}

FReply SVerticalEditableText::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return EditableTextLayout->HandleMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FCursorReply SVerticalEditableText::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	return FCursorReply::Cursor(EMouseCursor::TextEditBeam);
}

const FSlateBrush* SVerticalEditableText::GetFocusBrush() const
{
	return nullptr;
}

bool SVerticalEditableText::IsInteractable() const
{
	return IsEnabled();
}

bool SVerticalEditableText::ComputeVolatility() const
{
	return SWidget::ComputeVolatility()
		|| HasKeyboardFocus()
		|| EditableTextLayout->ComputeVolatility()
		|| Font.IsBound()
		|| ColorAndOpacity.IsBound()
		|| BackgroundImageSelected.IsBound()
		|| bIsReadOnly.IsBound()
		|| bIsPassword.IsBound()
		|| MinDesiredWidth.IsBound();
}

void SVerticalEditableText::SetHintText(const TAttribute< FText >& InHintText)
{
	EditableTextLayout->SetHintText(InHintText);
}

FText SVerticalEditableText::GetHintText() const
{
	return EditableTextLayout->GetHintText();
}

void SVerticalEditableText::SetSearchText(const TAttribute<FText>& InSearchText)
{
	EditableTextLayout->SetSearchText(InSearchText);
}

FText SVerticalEditableText::GetSearchText() const
{
	return EditableTextLayout->GetSearchText();
}

void SVerticalEditableText::SetIsReadOnly(TAttribute< bool > InIsReadOnly)
{
	bIsReadOnly = InIsReadOnly;
}

void SVerticalEditableText::SetIsPassword(TAttribute< bool > InIsPassword)
{
	bIsPassword = InIsPassword;
	PlainTextMarshaller->SetIsPassword(bIsPassword);
}

void SVerticalEditableText::SetColorAndOpacity(TAttribute<FSlateColor> Color)
{
	ColorAndOpacity = Color;
}

void SVerticalEditableText::SetMinDesiredWidth(const TAttribute<float>& InMinDesiredWidth)
{
	MinDesiredWidth = InMinDesiredWidth;
}

void SVerticalEditableText::SetIsCaretMovedWhenGainFocus(const TAttribute<bool>& InIsCaretMovedWhenGainFocus)
{
	bIsCaretMovedWhenGainFocus = InIsCaretMovedWhenGainFocus;
}

void SVerticalEditableText::SetSelectAllTextWhenFocused(const TAttribute<bool>& InSelectAllTextWhenFocused)
{
	bSelectAllTextWhenFocused = InSelectAllTextWhenFocused;
}

void SVerticalEditableText::SetRevertTextOnEscape(const TAttribute<bool>& InRevertTextOnEscape)
{
	bRevertTextOnEscape = InRevertTextOnEscape;
}

void SVerticalEditableText::SetClearKeyboardFocusOnCommit(const TAttribute<bool>& InClearKeyboardFocusOnCommit)
{
	bClearKeyboardFocusOnCommit = InClearKeyboardFocusOnCommit;
}

void SVerticalEditableText::SetSelectAllTextOnCommit(const TAttribute<bool>& InSelectAllTextOnCommit)
{
	bSelectAllTextOnCommit = InSelectAllTextOnCommit;
}

void SVerticalEditableText::SetSelectWordOnMouseDoubleClick(const TAttribute<bool>& InSelectWordOnMouseDoubleClick)
{
	bSelectWordOnMouseDoubleClick = InSelectWordOnMouseDoubleClick;
}

void SVerticalEditableText::SetJustification(const TAttribute<ETextJustify::Type>& InJustification)
{
	EditableTextLayout->SetJustification(InJustification);
}

void SVerticalEditableText::SetAllowContextMenu(const TAttribute< bool >& InAllowContextMenu)
{
	bAllowContextMenu = InAllowContextMenu;
}

void SVerticalEditableText::SetEnableIntegratedKeyboard(const TAttribute<bool>& InEnableIntegratedKeyboard)
{
	bEnableIntegratedKeyboard = InEnableIntegratedKeyboard;
}

void SVerticalEditableText::SetVirtualKeyboardDismissAction(TAttribute< EVirtualKeyboardDismissAction > InVirtualKeyboardDismissAction)
{
	VirtualKeyboardDismissAction = InVirtualKeyboardDismissAction;
}

void SVerticalEditableText::SetTextShapingMethod(const TOptional<ETextShapingMethod>& InTextShapingMethod)
{
	EditableTextLayout->SetTextShapingMethod(InTextShapingMethod);
}

void SVerticalEditableText::SetTextFlowDirection(const TOptional<ETextFlowDirection>& InTextFlowDirection)
{
	EditableTextLayout->SetTextFlowDirection(InTextFlowDirection);
}

void SVerticalEditableText::SetOverflowPolicy(TOptional<ETextOverflowPolicy> InOverflowPolicy)
{
	EditableTextLayout->SetOverflowPolicy(InOverflowPolicy);
}

bool SVerticalEditableText::AnyTextSelected() const
{
	return EditableTextLayout->AnyTextSelected();
}

void SVerticalEditableText::SelectAllText()
{
	EditableTextLayout->SelectAllText();
}

void SVerticalEditableText::ClearSelection()
{
	EditableTextLayout->ClearSelection();
}

FText SVerticalEditableText::GetSelectedText() const
{
	return EditableTextLayout->GetSelectedText();
}

void SVerticalEditableText::GoTo(const FTextLocation& NewLocation)
{
	EditableTextLayout->GoTo(NewLocation);
}

void SVerticalEditableText::GoTo(const ETextLocation NewLocation)
{
	EditableTextLayout->GoTo(NewLocation);
}

void SVerticalEditableText::ScrollTo(const FTextLocation& NewLocation)
{
	EditableTextLayout->ScrollTo(NewLocation);
}

void SVerticalEditableText::ScrollTo(const ETextLocation NewLocation)
{
	EditableTextLayout->ScrollTo(NewLocation);
}

void SVerticalEditableText::BeginSearch(const FText& InSearchText, const ESearchCase::Type InSearchCase, const bool InReverse)
{
	EditableTextLayout->BeginSearch(InSearchText, InSearchCase, InReverse);
}

void SVerticalEditableText::AdvanceSearch(const bool InReverse)
{
	EditableTextLayout->AdvanceSearch(InReverse);
}

void SVerticalEditableText::EnableTextInputMethodContext()
{
	EditableTextLayout->EnableTextInputMethodContext();
}

FTextSelection SVerticalEditableText::GetSelection() const
{
	return EditableTextLayout->GetSelection();
}

void SVerticalEditableText::SelectText(const FTextLocation& InSelectionStart, const FTextLocation& InCursorLocation)
{
	EditableTextLayout->SelectText(InSelectionStart, InCursorLocation);
}

void SVerticalEditableText::SynchronizeTextStyle()
{
	// Has the style used for this editable text changed?
	bool bTextStyleChanged = false;
	FTextBlockStyle NewTextStyle = EditableTextLayout->GetTextStyle();

	// Sync from the font override
	if (Font.IsSet())
	{
		const FSlateFontInfo& NewFontInfo = Font.Get();
		if (!NewTextStyle.Font.IsIdenticalTo(NewFontInfo))
		{
			NewTextStyle.Font = NewFontInfo;
			bTextStyleChanged = true;
		}
	}

	// Sync from the color override
	if (ColorAndOpacity.IsSet())
	{
		const FSlateColor& NewColorAndOpacity = ColorAndOpacity.Get();
		if (NewTextStyle.ColorAndOpacity != NewColorAndOpacity)
		{
			NewTextStyle.ColorAndOpacity = NewColorAndOpacity;
			bTextStyleChanged = true;
		}
	}

	// Sync from the highlight shape override
	if (BackgroundImageSelected.IsSet())
	{
		const FSlateBrush* NewSelectionBrush = BackgroundImageSelected.Get();
		if (NewSelectionBrush && NewTextStyle.HighlightShape != *NewSelectionBrush)
		{
			NewTextStyle.HighlightShape = *NewSelectionBrush;
			bTextStyleChanged = true;
		}
	}

	if (bTextStyleChanged)
	{
		EditableTextLayout->SetTextStyle(NewTextStyle);
		EditableTextLayout->ForceRefreshTextLayout(EditableTextLayout->GetEditableText());
	}
}

bool SVerticalEditableText::IsTextReadOnly() const
{
	return bIsReadOnly.Get(false);
}

bool SVerticalEditableText::IsTextPassword() const
{
	return bIsPassword.Get(false);
}

bool SVerticalEditableText::IsMultiLineTextEdit() const
{
	return false;
}

bool SVerticalEditableText::IsIntegratedKeyboardEnabled() const
{
	return bEnableIntegratedKeyboard.Get(false);
}

bool SVerticalEditableText::ShouldJumpCursorToEndWhenFocused() const
{
	return bIsCaretMovedWhenGainFocus.Get(false);
}

bool SVerticalEditableText::ShouldSelectAllTextWhenFocused() const
{
	return bSelectAllTextWhenFocused.Get(false);
}

bool SVerticalEditableText::ShouldClearTextSelectionOnFocusLoss() const
{
	return true;
}

bool SVerticalEditableText::ShouldRevertTextOnEscape() const
{
	return bRevertTextOnEscape.Get(false);
}

bool SVerticalEditableText::ShouldClearKeyboardFocusOnCommit() const
{
	return bClearKeyboardFocusOnCommit.Get(false);
}

bool SVerticalEditableText::ShouldSelectAllTextOnCommit() const
{
	return bSelectAllTextOnCommit.Get(false);
}

bool SVerticalEditableText::ShouldSelectWordOnMouseDoubleClick() const
{
	return bSelectWordOnMouseDoubleClick.Get(true);
}

bool SVerticalEditableText::CanInsertCarriageReturn() const
{
	return false;
}

bool SVerticalEditableText::CanTypeCharacter(const TCHAR InChar) const
{
	if (OnIsTypedCharValid.IsBound())
	{
		return OnIsTypedCharValid.Execute(InChar);
	}

	return InChar != TEXT('\t');
}

void SVerticalEditableText::EnsureActiveTick()
{
	TSharedPtr<FActiveTimerHandle> ActiveTickTimerPin = ActiveTickTimer.Pin();
	if (ActiveTickTimerPin.IsValid())
	{
		return;
	}

	auto DoActiveTick = [this](double InCurrentTime, float InDeltaTime) -> EActiveTimerReturnType
		{
			// Continue if we still have focus, otherwise treat as a fire-and-forget Tick() request
			const bool bShouldAppearFocused = HasKeyboardFocus() || EditableTextLayout->HasActiveContextMenu();
			return (bShouldAppearFocused) ? EActiveTimerReturnType::Continue : EActiveTimerReturnType::Stop;
		};

	const float TickPeriod = 0.5f;
	ActiveTickTimer = RegisterActiveTimer(TickPeriod, FWidgetActiveTimerDelegate::CreateLambda(DoActiveTick));
}

EKeyboardType SVerticalEditableText::GetVirtualKeyboardType() const
{
	return VirtualKeyboardType.Get();
}

FVirtualKeyboardOptions SVerticalEditableText::GetVirtualKeyboardOptions() const
{
	return VirtualKeyboardOptions;
}

EVirtualKeyboardTrigger SVerticalEditableText::GetVirtualKeyboardTrigger() const
{
	return VirtualKeyboardTrigger.Get();
}

EVirtualKeyboardDismissAction SVerticalEditableText::GetVirtualKeyboardDismissAction() const
{
	return VirtualKeyboardDismissAction.Get();
}

TSharedRef<SWidget> SVerticalEditableText::GetSlateWidget()
{
	return AsShared();
}

TSharedPtr<SWidget> SVerticalEditableText::GetSlateWidgetPtr()
{
	if (DoesSharedInstanceExist())
	{
		return AsShared();
	}
	return nullptr;
}

TSharedPtr<SWidget> SVerticalEditableText::BuildContextMenuContent() const
{
	if (!bAllowContextMenu.Get())
	{
		return nullptr;
	}

	if (OnContextMenuOpening.IsBound())
	{
		return OnContextMenuOpening.Execute();
	}

	return EditableTextLayout->BuildDefaultContextMenu(MenuExtender);
}

void SVerticalEditableText::OnTextChanged(const FText& InText)
{
	OnTextChangedCallback.ExecuteIfBound(InText);
}

void SVerticalEditableText::OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	OnTextCommittedCallback.ExecuteIfBound(InText, InTextAction);
}

void SVerticalEditableText::OnCursorMoved(const FTextLocation& InLocation)
{
	Invalidate(EInvalidateWidgetReason::Layout);
}

float SVerticalEditableText::UpdateAndClampHorizontalScrollBar(const float InViewOffset, const float InViewFraction, const EVisibility InVisiblityOverride)
{
	return EditableTextLayout->GetScrollOffset().X;
}

float SVerticalEditableText::UpdateAndClampVerticalScrollBar(const float InViewOffset, const float InViewFraction, const EVisibility InVisiblityOverride)
{
	return 0.0f;
}

#if WITH_ACCESSIBILITY
TSharedRef<FSlateAccessibleWidget> SVerticalEditableText::CreateAccessibleWidget()
{
	return MakeShareable<FSlateAccessibleWidget>(new FSlateAccessibleEditableText(SharedThis(this)));
}

TOptional<FText> SVerticalEditableText::GetDefaultAccessibleText(EAccessibleType AccessibleType) const
{
	return GetHintText();
}
#endif

