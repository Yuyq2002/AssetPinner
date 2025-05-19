// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSlotBase.h"
#include "Components/TextBlock.h"

void UPinnedAssetSlotBase::SetAssetData(const FString& Path)
{
	AssetPath = Path;

	Name->SetText(FText::FromString(FPackageName::GetShortName(*Path)));
}

FReply UPinnedAssetSlotBase::NativeOnMouseButtonDoubleClick(FGeometry InMyGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("Click"));

	return FReply::Handled();
}
