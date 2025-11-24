// Fill out your copyright notice in the Description page of Project Settings.


#include "SVerticalEditableTextBox.h"
//#include "Framework/Text/TextEditHelper.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SPopUpErrorText.h"

#if WITH_ACCESSIBILITY
#include "Widgets/Accessibility/SlateAccessibleWidgets.h"
#endif

SVerticalEditableTextBox::SVerticalEditableTextBox()
{
#if WITH_ACCESSIBILITY
	AccessibleBehavior = EAccessibleBehavior::Auto;
	bCanChildrenBeAccessible = false;
#endif
}

SVerticalEditableTextBox::~SVerticalEditableTextBox() = default;

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
void SVerticalEditableTextBox::Construct(const FArguments& InArgs)
{
	check(InArgs._Style);
	SetStyle(InArgs._Style);

	PaddingOverride = InArgs._Padding;
	FontOverride = InArgs._Font;
	ForegroundColorOverride = InArgs._ForegroundColor;
	BackgroundColorOverride = InArgs._BackgroundColor;
	ReadOnlyForegroundColorOverride = InArgs._ReadOnlyForegroundColor;
	FocusedForegroundColorOverride = InArgs._FocusedForegroundColor;
	MaximumLength = InArgs._MaximumLength;
	OnTextChanged = InArgs._OnTextChanged;
	OnVerifyTextChanged = InArgs._OnVerifyTextChanged;
	OnTextCommitted = InArgs._OnTextCommitted;

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SVerticalEditableTextBox::GetBorderImage)
		.BorderBackgroundColor(this, &SVerticalEditableTextBox::DetermineBackgroundColor)
		.ForegroundColor(this, &SVerticalEditableTextBox::DetermineForegroundColor)
		.Padding(0.f)
		[
			SAssignNew(Box, SVerticalBox)

				+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.FillHeight(1)
				[
					SAssignNew(PaddingBox, SBox)
						.Padding(this, &SVerticalEditableTextBox::DeterminePadding)
						.VAlign(VAlign_Center)
						[
							SAssignNew(VerticalEditableText, SVerticalEditableText)
								.Text(InArgs._Text)
								.HintText(InArgs._HintText)
								.SearchText(InArgs._SearchText)
								.Font(this, &SVerticalEditableTextBox::DetermineFont)
								.IsReadOnly(InArgs._IsReadOnly)
								.IsPassword(InArgs._IsPassword)
								.IsCaretMovedWhenGainFocus(InArgs._IsCaretMovedWhenGainFocus)
								.SelectAllTextWhenFocused(InArgs._SelectAllTextWhenFocused)
								.RevertTextOnEscape(InArgs._RevertTextOnEscape)
								.ClearKeyboardFocusOnCommit(InArgs._ClearKeyboardFocusOnCommit)
								.Justification(InArgs._Justification)
								.AllowContextMenu(InArgs._AllowContextMenu)
								.OnContextMenuOpening(InArgs._OnContextMenuOpening)
								.ContextMenuExtender(InArgs._ContextMenuExtender)
								.OnTextChanged(this, &SVerticalEditableTextBox::OnEditableTextChanged)
								.OnTextCommitted(this, &SVerticalEditableTextBox::OnEditableTextCommitted)
								.MinDesiredWidth(InArgs._MinDesiredWidth)
								.SelectAllTextOnCommit(InArgs._SelectAllTextOnCommit)
								.SelectWordOnMouseDoubleClick(InArgs._SelectWordOnMouseDoubleClick)
								.OnKeyCharHandler(InArgs._OnKeyCharHandler)
								.OnKeyDownHandler(InArgs._OnKeyDownHandler)
								.VirtualKeyboardType(InArgs._VirtualKeyboardType)
								.VirtualKeyboardOptions(InArgs._VirtualKeyboardOptions)
								.VirtualKeyboardTrigger(InArgs._VirtualKeyboardTrigger)
								.VirtualKeyboardDismissAction(InArgs._VirtualKeyboardDismissAction)
								.TextShapingMethod(InArgs._TextShapingMethod)
								.TextFlowDirection(InArgs._TextFlowDirection)
								.OverflowPolicy(InArgs._OverflowPolicy)
						]
				]
		]
	);

	ErrorReporting = InArgs._ErrorReporting;
	if (ErrorReporting.IsValid())
	{
		Box->AddSlot()
			.AutoHeight()
			.Padding(3, 0)
			[
				ErrorReporting->AsWidget()
			];
	}
	else
	{
		// this also creates a default widget
		// if we don't create the widget in Construct() 
		// it will get created in OnEditableTextChanged()
		// create it now so that the default size of the textbox
		// won't grow after user use it once
		SetError(FText::GetEmpty());
	}
}

void SVerticalEditableTextBox::SetStyle(const FEditableTextBoxStyle* InStyle)
{
	Style = InStyle;

	if (Style == nullptr)
	{
		FArguments Defaults;
		Style = Defaults._Style;
	}

	check(Style);

	BorderImageNormal = &Style->BackgroundImageNormal;
	BorderImageHovered = &Style->BackgroundImageHovered;
	BorderImageFocused = &Style->BackgroundImageFocused;
	BorderImageReadOnly = &Style->BackgroundImageReadOnly;

	SetTextBlockStyle(&Style->TextStyle);
}

void SVerticalEditableTextBox::SetTextBlockStyle(const FTextBlockStyle* InTextStyle)
{
	// The Construct() function will call this before EditableText exists,
	// so we need a guard here to ignore that function call.
	if (VerticalEditableText.IsValid())
	{
		VerticalEditableText->SetTextBlockStyle(InTextStyle);
	}
}

void SVerticalEditableTextBox::SetText(const TAttribute< FText >& InNewText)
{
	VerticalEditableText->SetText(InNewText);
}


void SVerticalEditableTextBox::SetError(const FText& InError)
{
	SetError(InError.ToString());
}


void SVerticalEditableTextBox::SetError(const FString& InError)
{
	const bool bHaveError = !InError.IsEmpty();

	if (!ErrorReporting.IsValid())
	{
		// No error reporting was specified; make a default one
		TSharedPtr<SPopupErrorText> ErrorTextWidget;
		Box->AddSlot()
			.AutoHeight()
			.Padding(3, 0)
			[
				SAssignNew(ErrorTextWidget, SPopupErrorText)
			];
		ErrorReporting = ErrorTextWidget;
	}

	ErrorReporting->SetError(InError);
}


void SVerticalEditableTextBox::SetOnKeyCharHandler(FOnKeyChar InOnKeyCharHandler)
{
	VerticalEditableText->SetOnKeyCharHandler(InOnKeyCharHandler);
}


void SVerticalEditableTextBox::SetOnKeyDownHandler(FOnKeyDown InOnKeyDownHandler)
{
	VerticalEditableText->SetOnKeyDownHandler(InOnKeyDownHandler);
}


void SVerticalEditableTextBox::SetTextShapingMethod(const TOptional<ETextShapingMethod>& InTextShapingMethod)
{
	VerticalEditableText->SetTextShapingMethod(InTextShapingMethod);
}


void SVerticalEditableTextBox::SetTextFlowDirection(const TOptional<ETextFlowDirection>& InTextFlowDirection)
{
	VerticalEditableText->SetTextFlowDirection(InTextFlowDirection);
}


void SVerticalEditableTextBox::SetOverflowPolicy(TOptional<ETextOverflowPolicy> InOverflowPolicy)
{
	VerticalEditableText->SetOverflowPolicy(InOverflowPolicy);
}

bool SVerticalEditableTextBox::AnyTextSelected() const
{
	return VerticalEditableText->AnyTextSelected();
}


void SVerticalEditableTextBox::SelectAllText()
{
	VerticalEditableText->SelectAllText();
}


void SVerticalEditableTextBox::ClearSelection()
{
	VerticalEditableText->ClearSelection();
}


FText SVerticalEditableTextBox::GetSelectedText() const
{
	return VerticalEditableText->GetSelectedText();
}

void SVerticalEditableTextBox::GoTo(const FTextLocation& NewLocation)
{
	VerticalEditableText->GoTo(NewLocation);
}

void SVerticalEditableTextBox::ScrollTo(const FTextLocation& NewLocation)
{
	VerticalEditableText->ScrollTo(NewLocation);
}

void SVerticalEditableTextBox::BeginSearch(const FText& InSearchText, const ESearchCase::Type InSearchCase, const bool InReverse)
{
	VerticalEditableText->BeginSearch(InSearchText, InSearchCase, InReverse);
}

void SVerticalEditableTextBox::AdvanceSearch(const bool InReverse)
{
	VerticalEditableText->AdvanceSearch(InReverse);
}

bool SVerticalEditableTextBox::HasError() const
{
	return ErrorReporting.IsValid() && ErrorReporting->HasError();
}

const FSlateBrush* SVerticalEditableTextBox::GetBorderImage() const
{
	if (VerticalEditableText->IsTextReadOnly())
	{
		return BorderImageReadOnly;
	}
	else if (VerticalEditableText->HasKeyboardFocus())
	{
		return BorderImageFocused;
	}
	else
	{
		if (VerticalEditableText->IsHovered())
		{
			return BorderImageHovered;
		}
		else
		{
			return BorderImageNormal;
		}
	}
}


bool SVerticalEditableTextBox::SupportsKeyboardFocus() const
{
	return StaticCastSharedPtr<SWidget>(VerticalEditableText)->SupportsKeyboardFocus();
}


bool SVerticalEditableTextBox::HasKeyboardFocus() const
{
	// Since keyboard focus is forwarded to our editable text, we will test it instead
	return SBorder::HasKeyboardFocus() || VerticalEditableText->HasKeyboardFocus();
}


FReply SVerticalEditableTextBox::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	FReply Reply = FReply::Handled();

	if (InFocusEvent.GetCause() != EFocusCause::Cleared)
	{
		// Forward keyboard focus to our editable text widget
		Reply.SetUserFocus(VerticalEditableText.ToSharedRef(), InFocusEvent.GetCause());
	}

	return Reply;
}


FReply SVerticalEditableTextBox::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Escape && VerticalEditableText->HasKeyboardFocus())
	{
		// Clear focus
		return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::Cleared);
	}

	return FReply::Unhandled();
}

FMargin SVerticalEditableTextBox::DeterminePadding() const
{
	check(Style);
	return PaddingOverride.IsSet() ? PaddingOverride.Get() : Style->Padding;
}

FSlateFontInfo SVerticalEditableTextBox::DetermineFont() const
{
	check(Style);
	return FontOverride.IsSet() ? FontOverride.Get() : Style->TextStyle.Font;
}

FSlateColor SVerticalEditableTextBox::DetermineBackgroundColor() const
{
	check(Style);
	return BackgroundColorOverride.IsSet() ? BackgroundColorOverride.Get() : Style->BackgroundColor;
}

FSlateColor SVerticalEditableTextBox::DetermineForegroundColor() const
{
	check(Style);

	if (VerticalEditableText->IsTextReadOnly())
	{
		if (ReadOnlyForegroundColorOverride.IsSet())
		{
			return ReadOnlyForegroundColorOverride.Get();
		}
		if (ForegroundColorOverride.IsSet())
		{
			return ForegroundColorOverride.Get();
		}

		return Style->ReadOnlyForegroundColor;
	}
	else if (HasKeyboardFocus())
	{
		return FocusedForegroundColorOverride.IsSet() ? FocusedForegroundColorOverride.Get() : Style->FocusedForegroundColor;
	}
	else
	{
		return ForegroundColorOverride.IsSet() ? ForegroundColorOverride.Get() : Style->ForegroundColor;
	}
}

void SVerticalEditableTextBox::SetHintText(const TAttribute< FText >& InHintText)
{
	VerticalEditableText->SetHintText(InHintText);
}


void SVerticalEditableTextBox::SetSearchText(const TAttribute<FText>& InSearchText)
{
	VerticalEditableText->SetSearchText(InSearchText);
}


FText SVerticalEditableTextBox::GetSearchText() const
{
	return VerticalEditableText->GetSearchText();
}


void SVerticalEditableTextBox::SetIsReadOnly(TAttribute< bool > InIsReadOnly)
{
	VerticalEditableText->SetIsReadOnly(InIsReadOnly);
}


void SVerticalEditableTextBox::SetIsPassword(TAttribute< bool > InIsPassword)
{
	VerticalEditableText->SetIsPassword(InIsPassword);
}


void SVerticalEditableTextBox::SetFont(const TAttribute<FSlateFontInfo>& InFont)
{
	FontOverride = InFont;
}

void SVerticalEditableTextBox::SetTextBoxForegroundColor(const TAttribute<FSlateColor>& InForegroundColor)
{
	ForegroundColorOverride = InForegroundColor;
}

void SVerticalEditableTextBox::SetTextBoxBackgroundColor(const TAttribute<FSlateColor>& InBackgroundColor)
{
	BackgroundColorOverride = InBackgroundColor;
}

void SVerticalEditableTextBox::SetReadOnlyForegroundColor(const TAttribute<FSlateColor>& InReadOnlyForegroundColor)
{
	ReadOnlyForegroundColorOverride = InReadOnlyForegroundColor;
}

void SVerticalEditableTextBox::SetFocusedForegroundColor(const TAttribute<FSlateColor>& InFocusedForegroundColor)
{
	FocusedForegroundColorOverride = InFocusedForegroundColor;
}

void SVerticalEditableTextBox::SetMaximumLength(const TAttribute<int32>& InMaximumLength)
{
	MaximumLength = InMaximumLength;
}

void SVerticalEditableTextBox::SetMinimumDesiredWidth(const TAttribute<float>& InMinimumDesiredWidth)
{
	VerticalEditableText->SetMinDesiredWidth(InMinimumDesiredWidth);
}


void SVerticalEditableTextBox::SetIsCaretMovedWhenGainFocus(const TAttribute<bool>& InIsCaretMovedWhenGainFocus)
{
	VerticalEditableText->SetIsCaretMovedWhenGainFocus(InIsCaretMovedWhenGainFocus);
}


void SVerticalEditableTextBox::SetSelectAllTextWhenFocused(const TAttribute<bool>& InSelectAllTextWhenFocused)
{
	VerticalEditableText->SetSelectAllTextWhenFocused(InSelectAllTextWhenFocused);
}


void SVerticalEditableTextBox::SetRevertTextOnEscape(const TAttribute<bool>& InRevertTextOnEscape)
{
	VerticalEditableText->SetRevertTextOnEscape(InRevertTextOnEscape);
}


void SVerticalEditableTextBox::SetClearKeyboardFocusOnCommit(const TAttribute<bool>& InClearKeyboardFocusOnCommit)
{
	VerticalEditableText->SetClearKeyboardFocusOnCommit(InClearKeyboardFocusOnCommit);
}


void SVerticalEditableTextBox::SetSelectAllTextOnCommit(const TAttribute<bool>& InSelectAllTextOnCommit)
{
	VerticalEditableText->SetSelectAllTextOnCommit(InSelectAllTextOnCommit);
}

void SVerticalEditableTextBox::SetSelectWordOnMouseDoubleClick(const TAttribute<bool>& InSelectWordOnMouseDoubleClick)
{
	VerticalEditableText->SetSelectWordOnMouseDoubleClick(InSelectWordOnMouseDoubleClick);
}

void SVerticalEditableTextBox::SetJustification(const TAttribute<ETextJustify::Type>& InJustification)
{
	VerticalEditableText->SetJustification(InJustification);
}


void SVerticalEditableTextBox::SetAllowContextMenu(TAttribute<bool> InAllowContextMenu)
{
	VerticalEditableText->SetAllowContextMenu(InAllowContextMenu);
}

void SVerticalEditableTextBox::SetVirtualKeyboardDismissAction(TAttribute<EVirtualKeyboardDismissAction> InVirtualKeyboardDismissAction)
{
	VerticalEditableText->SetVirtualKeyboardDismissAction(InVirtualKeyboardDismissAction);
}

void SVerticalEditableTextBox::EnableTextInputMethodContext()
{
	VerticalEditableText->EnableTextInputMethodContext();
}
#if WITH_ACCESSIBILITY
TSharedRef<FSlateAccessibleWidget> SVerticalEditableTextBox::CreateAccessibleWidget()
{
	return MakeShareable<FSlateAccessibleWidget>(new FSlateAccessibleEditableTextBox(SharedThis(this)));
}

TOptional<FText> SVerticalEditableTextBox::GetDefaultAccessibleText(EAccessibleType AccessibleType) const
{
	// The parent Construct() function will call this before EditableText exists,
	// so we need a guard here to ignore that function call.
	if (VerticalEditableText.IsValid())
	{
		return VerticalEditableText->GetHintText();
	}
	return TOptional<FText>();
}
#endif

bool VerifyTextLength(const FText& InText, FText& OutErrorMessage, int32 InMaximumLength);

void SVerticalEditableTextBox::OnEditableTextChanged(const FText& InText)
{
	OnTextChanged.ExecuteIfBound(InText);

	const int32 MaximumLengthValue = MaximumLength.Get();
	if (OnVerifyTextChanged.IsBound() || MaximumLengthValue >= 0)
	{
		FText OutErrorMessage;
		if (!VerifyTextLength(InText, OutErrorMessage, MaximumLengthValue) ||
			(OnVerifyTextChanged.IsBound() && !OnVerifyTextChanged.Execute(InText, OutErrorMessage)))
		{
			// Display as an error.
			SetError(OutErrorMessage);
		}
		else
		{
			SetError(FText::GetEmpty());
		}
	}
}

void SVerticalEditableTextBox::OnEditableTextCommitted(const FText& InText, ETextCommit::Type InCommitType)
{
	FText OutErrorMessage;
	if (!VerifyTextLength(InText, OutErrorMessage, MaximumLength.Get()) ||
		(OnVerifyTextChanged.IsBound() && !OnVerifyTextChanged.Execute(InText, OutErrorMessage)))
	{
		// Display as an error.
		if (InCommitType == ETextCommit::OnEnter)
		{
			SetError(OutErrorMessage);
		}
		return;
	}

	// Text commited without errors, so clear error text
	SetError(FText::GetEmpty());

	OnTextCommitted.ExecuteIfBound(InText, InCommitType);
}

void SVerticalEditableTextBox::SetRotation(TAttribute<ERotation> InRotation)
{
	Rotation = InRotation;
	if (VerticalEditableText.IsValid())
		VerticalEditableText->SetRotation(Rotation);
}

#define LOCTEXT_NAMESPACE "SVerticalEditableTextBox"

bool VerifyTextLength(const FText& InText, FText& OutErrorMessage, int32 InMaximumLength)
{
	const int TextLength = InText.ToString().Len();
	if (InMaximumLength > 0 && TextLength > InMaximumLength)
	{
		OutErrorMessage = FText::Format(LOCTEXT("TextTooLong", "This text is too long. It uses {0} characters of {1} allowed."), TextLength, InMaximumLength);
		return false;
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
