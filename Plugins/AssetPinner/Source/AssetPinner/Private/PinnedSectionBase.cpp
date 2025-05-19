// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedSectionBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components/WrapBox.h"
#include "PinnedAssetSlotBase.h"

void UPinnedSectionBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!GEngine)
		return;

	UPinnedAssetSubsystem* Subsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return;

	Subsystem->OnListChangedDelegate.BindDynamic(this, &UPinnedSectionBase::OnListChangedCallback);

	Refresh(Subsystem->GetAssetPathList());
}

void UPinnedSectionBase::OnListChangedCallback(const TArray<FString>& List)
{
	Refresh(List);
}

void UPinnedSectionBase::Refresh(const TArray<FString>& List)
{
	if (!AssetSlotWidget) return;

	WrapBox->ClearChildren();
	for (auto& AssetPath : List)
	{
		UPinnedAssetSlotBase* NewSlot = CreateWidget<UPinnedAssetSlotBase>(this, AssetSlotWidget);
		NewSlot->SetAssetData(AssetPath);

		WrapBox->AddChildToWrapBox(NewSlot);
	}

	return;
}
