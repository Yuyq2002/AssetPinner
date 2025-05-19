// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedSectionBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components/WrapBox.h"
#include "PinnedAssetSlotBase.h"
#include "AssetRegistry/AssetRegistryModule.h"

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

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	WrapBox->ClearChildren();
	for (auto& AssetPath : List)
	{
		TArray<FAssetData> Assets;
		AssetRegistry.GetAssetsByPackageName(FName(AssetPath), Assets);
		if (Assets.Num() <= 0)
			continue;

		UPinnedAssetSlotBase* NewSlot = CreateWidget<UPinnedAssetSlotBase>(this, AssetSlotWidget);
		NewSlot->SetAssetData(AssetPath, Assets[0]);
		NewSlot->SetParentRef(this);

		WrapBox->AddChildToWrapBox(NewSlot);
	}

	return;
}

FReply UPinnedSectionBase::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
		InUnpinMode = true;
	return FReply::Handled();
}

FReply UPinnedSectionBase::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
		InUnpinMode = false;
	return FReply::Handled();
}

bool UPinnedSectionBase::GetInUnpinMode()
{
	return InUnpinMode;
}
