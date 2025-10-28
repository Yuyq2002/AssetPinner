// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedSectionBase.h"

#include "Components/WrapBox.h"
#include "Components/ScrollBox.h"

#include "PinnedAssetSlotBase.h"

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
