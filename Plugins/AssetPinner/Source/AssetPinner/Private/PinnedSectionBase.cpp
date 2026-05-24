// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedSectionBase.h"

#include "Components/WrapBox.h"
#include "Components/ScrollBox.h"

#include "PinnedAssetSlotBase.h"
#include "SlotDragOperation.h"

void UPinnedSectionBase::ClearPinnedAsset()
{
	WrapBox->ClearChildren();
}

void UPinnedSectionBase::AddPinnedAsset(UPinnedAssetSlotBase* NewPinnedSlot)
{
	WrapBox->AddChildToWrapBox(NewPinnedSlot);
}

void UPinnedSectionBase::SetEnableScrolling(bool IsEnabled)
{
	ScrollBox->SetIsEnabled(IsEnabled);
}

bool UPinnedSectionBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	USlotDragOperation* Operation = Cast<USlotDragOperation>(InOperation);
	if (!Operation)
		return false;

	if (Operation->OriginalParent != WrapBox)
	{
		Operation->DraggedWidget->RemoveFromParent();
		WrapBox->AddChildToWrapBox(Operation->DraggedWidget);
	}

	Operation->DraggedWidget->SetVisibility(ESlateVisibility::Visible);

	return true;
}