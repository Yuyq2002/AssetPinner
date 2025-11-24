// Fill out your copyright notice in the Description page of Project Settings.


#include "VerticalEditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UE5MainStreamObjectVersion.h"
#include "UObject/UE5ReleaseStreamObjectVersion.h"
#include "Engine/Font.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SVerticalEditableTextBox.h"
#include "Styling/DefaultStyleCache.h"
#include "Styling/UMGCoreStyle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VerticalEditableTextBox)

#define LOCTEXT_NAMESPACE "VerticalEditableTextBox"

/////////////////////////////////////////////////////
// UVerticalEditableTextBox

UVerticalEditableTextBox::UVerticalEditableTextBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITOR
	bIsFontDeprecationDone = false;
#endif
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		IsReadOnly = false;
	IsPassword = false;
	MinimumDesiredWidth = 0.0f;
	IsCaretMovedWhenGainFocus = true;
	SelectAllTextWhenFocused = false;
	RevertTextOnEscape = false;
	ClearKeyboardFocusOnCommit = true;
	SelectAllTextOnCommit = false;
	AllowContextMenu = true;
	VirtualKeyboardDismissAction = EVirtualKeyboardDismissAction::TextChangeOnDismiss;
	OverflowPolicy = ETextOverflowPolicy::Clip;

	WidgetStyle = UE::Slate::Private::FDefaultStyleCache::GetRuntime().GetEditableTextBoxStyle();

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> DefaultFontObj(*UWidget::GetDefaultFontName());
		FSlateFontInfo Font(DefaultFontObj.Object, 24, FName("Regular"));
		//The FSlateFontInfo just created doesn't contain a composite font (while the default from the WidgetStyle does),
		//so in the case the Font object is replaced by a null one, we have to keep the composite one as a fallback.
		Font.CompositeFont = WidgetStyle.TextStyle.Font.CompositeFont;

		WidgetStyle.SetFont(Font);
	}

#if WITH_EDITOR 
	if (IsEditorWidget())
	{
		WidgetStyle = UE::Slate::Private::FDefaultStyleCache::GetEditor().GetEditableTextBoxStyle();
	}
#endif // WITH_EDITOR
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_EDITORONLY_DATA
		AccessibleBehavior = ESlateAccessibleBehavior::Auto;
	bCanChildrenBeAccessible = false;
#endif
}

void UVerticalEditableTextBox::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FUE5ReleaseStreamObjectVersion::GUID);

	Super::Serialize(Ar);

#if WITH_EDITOR
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		if (Ar.IsLoading() && !bIsFontDeprecationDone && GetLinkerCustomVersion(FUE5ReleaseStreamObjectVersion::GUID) < FUE5ReleaseStreamObjectVersion::RemoveDuplicatedStyleInfo)
		{
			FTextBlockStyle& TextStyle = WidgetStyle.TextStyle;
			TextStyle.SetFont(WidgetStyle.Font_DEPRECATED);
			bIsFontDeprecationDone = true;
		}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
#endif
}

void UVerticalEditableTextBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyVerticalEditableTextBlock.Reset();
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
TSharedRef<SWidget> UVerticalEditableTextBox::RebuildWidget()
{
	MyVerticalEditableTextBlock = SNew(SVerticalEditableTextBox)
		.Style(&WidgetStyle)
		.IsReadOnly(IsReadOnly)
		.IsPassword(IsPassword)
		.MinDesiredWidth(MinimumDesiredWidth)
		.IsCaretMovedWhenGainFocus(IsCaretMovedWhenGainFocus)
		.SelectAllTextWhenFocused(SelectAllTextWhenFocused)
		.RevertTextOnEscape(RevertTextOnEscape)
		.ClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit)
		.SelectAllTextOnCommit(SelectAllTextOnCommit)
		.AllowContextMenu(AllowContextMenu)
		.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnTextChanged))
		.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, HandleOnTextCommitted))
		.VirtualKeyboardType(EVirtualKeyboardType::AsKeyboardType(KeyboardType.GetValue()))
		.VirtualKeyboardOptions(VirtualKeyboardOptions)
		.VirtualKeyboardTrigger(VirtualKeyboardTrigger)
		.VirtualKeyboardDismissAction(VirtualKeyboardDismissAction)
		.Justification(Justification)
		.OverflowPolicy(OverflowPolicy);

	return MyVerticalEditableTextBlock.ToSharedRef();
}

void UVerticalEditableTextBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyVerticalEditableTextBlock.IsValid())
	{
		return;
	}

	TAttribute<FText> TextBinding = PROPERTY_BINDING(FText, Text);
	TAttribute<FText> HintTextBinding = PROPERTY_BINDING(FText, HintText);

	MyVerticalEditableTextBlock->SetRotation(Rotation);
	MyVerticalEditableTextBlock->SetStyle(&WidgetStyle);
	MyVerticalEditableTextBlock->SetText(TextBinding);
	MyVerticalEditableTextBlock->SetHintText(HintTextBinding);
	MyVerticalEditableTextBlock->SetIsReadOnly(IsReadOnly);
	MyVerticalEditableTextBlock->SetIsPassword(IsPassword);
	MyVerticalEditableTextBlock->SetMinimumDesiredWidth(MinimumDesiredWidth);
	MyVerticalEditableTextBlock->SetIsCaretMovedWhenGainFocus(IsCaretMovedWhenGainFocus);
	MyVerticalEditableTextBlock->SetSelectAllTextWhenFocused(SelectAllTextWhenFocused);
	MyVerticalEditableTextBlock->SetRevertTextOnEscape(RevertTextOnEscape);
	MyVerticalEditableTextBlock->SetClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit);
	MyVerticalEditableTextBlock->SetSelectAllTextOnCommit(SelectAllTextOnCommit);
	MyVerticalEditableTextBlock->SetAllowContextMenu(AllowContextMenu);
	MyVerticalEditableTextBlock->SetVirtualKeyboardDismissAction(VirtualKeyboardDismissAction);
	MyVerticalEditableTextBlock->SetJustification(Justification);
	MyVerticalEditableTextBlock->SetOverflowPolicy(OverflowPolicy);

	ShapedTextOptions.SynchronizeShapedTextProperties(*MyVerticalEditableTextBlock);
}

void UVerticalEditableTextBox::SetRotation(ERotation InRotation)
{
	Rotation = InRotation;
	if (MyVerticalEditableTextBlock.IsValid())
		MyVerticalEditableTextBlock->SetRotation(Rotation);
}

FText UVerticalEditableTextBox::GetText() const
{
	if (MyVerticalEditableTextBlock.IsValid())
	{
		return MyVerticalEditableTextBlock->GetText();
	}

	return Text;
}

void UVerticalEditableTextBox::SetText(FText InText)
{
	if (SetTextInternal(InText))
	{
		if (MyVerticalEditableTextBlock.IsValid())
		{
			MyVerticalEditableTextBlock->SetText(Text);
		}
		BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::Text);
	}
}

bool UVerticalEditableTextBox::SetTextInternal(const FText& InText)
{
	if (!Text.IdenticalTo(InText, ETextIdenticalModeFlags::DeepCompare | ETextIdenticalModeFlags::LexicalCompareInvariants))
	{
		Text = InText;
		return true;
	}

	return false;
}

FText UVerticalEditableTextBox::GetHintText() const
{
	if (MyVerticalEditableTextBlock.IsValid())
	{
		return MyVerticalEditableTextBlock->GetHintText();
	}

	return HintText;
}

void UVerticalEditableTextBox::SetHintText(FText InText)
{
	HintText = InText;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetHintText(HintText);
	}
}

float UVerticalEditableTextBox::GetMinimumDesiredWidth() const
{
	return MinimumDesiredWidth;
}

void UVerticalEditableTextBox::SetMinDesiredWidth(float InMinDesiredWidth)
{
	MinimumDesiredWidth = InMinDesiredWidth;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetMinimumDesiredWidth(MinimumDesiredWidth);
	}
}

void UVerticalEditableTextBox::SetIsCaretMovedWhenGainFocus(bool bIsCaretMovedWhenGainFocus)
{
	IsCaretMovedWhenGainFocus = bIsCaretMovedWhenGainFocus;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetIsCaretMovedWhenGainFocus(bIsCaretMovedWhenGainFocus);
	}
}

bool UVerticalEditableTextBox::GetIsCaretMovedWhenGainFocus() const
{
	return IsCaretMovedWhenGainFocus;
}

void UVerticalEditableTextBox::SetSelectAllTextWhenFocused(bool bSelectAllTextWhenFocused)
{
	SelectAllTextWhenFocused = bSelectAllTextWhenFocused;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetSelectAllTextWhenFocused(bSelectAllTextWhenFocused);
	}
}

bool UVerticalEditableTextBox::GetSelectAllTextWhenFocused() const
{
	return SelectAllTextWhenFocused;
}

void UVerticalEditableTextBox::SetRevertTextOnEscape(bool bRevertTextOnEscape)
{
	RevertTextOnEscape = bRevertTextOnEscape;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetRevertTextOnEscape(bRevertTextOnEscape);
	}
}

bool UVerticalEditableTextBox::GetRevertTextOnEscape() const
{
	return RevertTextOnEscape;
}

void UVerticalEditableTextBox::SetClearKeyboardFocusOnCommit(bool bClearKeyboardFocusOnCommit)
{
	ClearKeyboardFocusOnCommit = bClearKeyboardFocusOnCommit;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetClearKeyboardFocusOnCommit(bClearKeyboardFocusOnCommit);
	}
}

bool UVerticalEditableTextBox::GetClearKeyboardFocusOnCommit() const
{
	return ClearKeyboardFocusOnCommit;
}

void UVerticalEditableTextBox::SetSelectAllTextOnCommit(bool bSelectAllTextOnCommit)
{
	SelectAllTextOnCommit = bSelectAllTextOnCommit;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetSelectAllTextOnCommit(bSelectAllTextOnCommit);
	}

}

bool UVerticalEditableTextBox::GetSelectAllTextOnCommit() const
{
	return SelectAllTextOnCommit;
}

void UVerticalEditableTextBox::SetForegroundColor(FLinearColor color)
{
	WidgetStyle.ForegroundColor = color;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetForegroundColor(color);
	}
}

void UVerticalEditableTextBox::SetError(FText InError)
{
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetError(InError);
	}
}

bool UVerticalEditableTextBox::GetIsReadOnly() const
{
	return IsReadOnly;
}

void UVerticalEditableTextBox::SetIsReadOnly(bool bIsReadOnly)
{
	IsReadOnly = bIsReadOnly;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetIsReadOnly(IsReadOnly);
	}
}

bool UVerticalEditableTextBox::GetIsPassword() const
{
	return IsPassword;
}

void UVerticalEditableTextBox::SetIsPassword(bool bIsPassword)
{
	IsPassword = bIsPassword;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetIsPassword(IsPassword);
	}
}

void UVerticalEditableTextBox::ClearError()
{
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetError(FText::GetEmpty());
	}
}

bool UVerticalEditableTextBox::HasError() const
{
	if (MyVerticalEditableTextBlock.IsValid())
	{
		return MyVerticalEditableTextBlock->HasError();
	}

	return false;
}


ETextJustify::Type UVerticalEditableTextBox::GetJustification() const
{
	return Justification;
}

void UVerticalEditableTextBox::SetJustification(ETextJustify::Type InJustification)
{
	Justification = InJustification;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetJustification(InJustification);
	}
}

ETextOverflowPolicy UVerticalEditableTextBox::GetTextOverflowPolicy() const
{
	return OverflowPolicy;
}

void UVerticalEditableTextBox::SetTextOverflowPolicy(ETextOverflowPolicy InOverflowPolicy)
{
	OverflowPolicy = InOverflowPolicy;
	if (MyVerticalEditableTextBlock.IsValid())
	{
		MyVerticalEditableTextBlock->SetOverflowPolicy(InOverflowPolicy);
	}
}

void UVerticalEditableTextBox::HandleOnTextChanged(const FText& InText)
{
	if (SetTextInternal(InText))
	{
		BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::Text);
		OnTextChanged.Broadcast(InText);
	}
}

void UVerticalEditableTextBox::HandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod)
{
	if (SetTextInternal(InText))
	{
		BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::Text);
	}
	OnTextCommitted.Broadcast(InText, CommitMethod);
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS

#if WITH_ACCESSIBILITY
TSharedPtr<SWidget> UVerticalEditableTextBox::GetAccessibleWidget() const
{
	return MyVerticalEditableTextBlock;
}
#endif

#if WITH_EDITOR

const FText UVerticalEditableTextBox::GetPaletteCategory()
{
	return LOCTEXT("Input", "Input");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

