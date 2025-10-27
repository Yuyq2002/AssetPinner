// Fill out your copyright notice in the Description page of Project Settings.


#include "Tab.h"
#include "PinnedSectionBase.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UTab::SetInfo(FString Name, UPinnedSectionBase* InParent, int InTabIndex)
{
	Parent = InParent;
	Index = InTabIndex;

	if(Text)
		Text->SetText(FText::FromString(Name));
}

int UTab::SetSelected(bool IsSelected)
{
	bIsSelected = IsSelected;

	if (bIsSelected)
	{
		Background->SetBrushColor(SelectedColor);
		return Index;
	}
	else
	{
		Background->SetBrushColor(BaseColor);
		return -1;
	}
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

