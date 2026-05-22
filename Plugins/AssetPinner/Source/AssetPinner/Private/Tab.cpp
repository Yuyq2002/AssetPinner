// Fill out your copyright notice in the Description page of Project Settings.


#include "Tab.h"
#include "PinnedWindowBase.h"
#include "TabBorder.h"
#include <VerticalTextBlock.h>
#include "VerticalEditableTextBox.h"
#include "EditorUtilityWidgetComponents.h"

void UTab::NativeConstruct()
{
	RenameTextBox->OnTextCommitted.AddDynamic(this, &UTab::OnNameChanged);

	if (Background)
	{
		Background->CheckCanRenameDelegate.BindDynamic(this, &UTab::GetIsPersistent);
		Background->OnRenameClickedDelegate.BindDynamic(this, &UTab::ActivateRenameBox);
		Background->OnRemoveClickedDelegate.BindDynamic(this, &UTab::OnRemoveClicked);
	}

	if(bInitInRenameMode)
		ActivateRenameBox();
}

void UTab::SetInfo(FText InName, UPinnedWindowBase* InParent, UWidget* InWidget, bool InIsPersistent)
{
	Parent = InParent;
	Widget = InWidget;

	if (Text)
		Text->SetText(InName);

	if (RenameTextBox)
		RenameTextBox->SetText(InName);

	bIsPersistent = InIsPersistent;
}

void UTab::SetInfo(UPinnedWindowBase* InParent, UWidget* InWidget)
{
	Parent = InParent;
	Widget = InWidget;
}

void UTab::SetInfo(FText InName)
{
	if (Text)
		Text->SetText(InName);

	if (RenameTextBox)
		RenameTextBox->SetText(InName);
}

void UTab::EditName(bool EnableEditing)
{
	if(EnableEditing)
	{ 
		RenameTextBox->SetVisibility(ESlateVisibility::Visible);
		RenameTextBox->SetKeyboardFocus();
		Text->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		RenameTextBox->SetVisibility(ESlateVisibility::Collapsed);
		Text->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTab::ActivateRenameBox()
{
	EditName(true);
}

UWidget* UTab::SetSelected(bool IsSelected)
{
	bIsSelected = IsSelected;

	if (bIsSelected)
	{
		Background->SetBrushColor(SelectedColor);
		return Widget;
	}
	else
	{
		Background->SetBrushColor(BaseColor);
		return nullptr;
	}
}

UWidget* UTab::GetSection()
{
	return Widget;
}

FString UTab::GetName()
{
	return Text->GetText().ToString();
}

FReply UTab::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnTabClickedDelegate.ExecuteIfBound(this);
	}

	return FReply::Handled();
}

void UTab::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(!bIsSelected)
		Background->SetBrushColor(HoverColor);
}

void UTab::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!bIsSelected)
		Background->SetBrushColor(BaseColor);
}

void UTab::OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod)
{
	OnNameChangedDelegate.ExecuteIfBound(this, Text->GetText(), InText);

	EditName(false);
}

void UTab::OnRemoveClicked()
{
	OnRemoveDelegate.ExecuteIfBound(this);
}
