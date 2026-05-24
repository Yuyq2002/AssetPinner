// Fill out your copyright notice in the Description page of Project Settings.


#include "Tab.h"
#include "PinnedWindowBase.h"
#include "TabBorder.h"
#include <VerticalTextBlock.h>
#include "VerticalEditableTextBox.h"
#include "EditorUtilityWidgetComponents.h"
#include "Containers/Ticker.h"
#include "SlotDragOperation.h"
#include "PinnedAssetSubsystem.h"
#include "PinnedAssetSlotBase.h"
#include "PinnedSectionBase.h"
#include "Components/WrapBox.h"

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
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime) -> bool
			{
				ActivateRenameBox();

				// Return false to tell the ticker to stop. (If you return true, it loops).
				return false;

			}), 0.0f);
}

void UTab::SetInfo(FText InName, UPinnedWindowBase* InParent, UPinnedSectionBase* InWidget, bool InIsPersistent)
{
	Parent = InParent;
	Widget = InWidget;

	if (Text)
		Text->SetText(InName);

	if (RenameTextBox)
		RenameTextBox->SetText(InName);

	bIsPersistent = InIsPersistent;
}

void UTab::SetInfo(UPinnedWindowBase* InParent, UPinnedSectionBase* InWidget)
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

UPinnedSectionBase* UTab::SetSelected(bool IsSelected)
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

UPinnedSectionBase* UTab::GetSection()
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

bool UTab::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	USlotDragOperation* Operation = Cast<USlotDragOperation>(InOperation);

	if (!Operation)
		return false;

	if (Operation->OriginalParent == Widget)
		return false;

	UPinnedAssetSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>() : nullptr;
	if(!Subsystem)
		return false;

	if (!Subsystem->MoveAssetPath(Operation->DraggedWidget->GetAssetPath(), Text->GetText().ToString()))
	{
		return false;
	}

	return true;
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
