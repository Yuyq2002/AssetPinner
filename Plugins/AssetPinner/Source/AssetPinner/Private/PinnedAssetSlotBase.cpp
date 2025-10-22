// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSlotBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PinnedSectionBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components\AssetThumbnailWidget.h"
#include <ContentBrowserModule.h>
#include "IContentBrowserSingleton.h"

void UPinnedAssetSlotBase::SetAssetData(const FString& Path, EPathType Type, const FString& AlternativeName)
{
	AssetPath = Path;

	if (Name)
		if (AlternativeName.IsEmpty())
			Name->SetText(FText::FromString(FPackageName::GetShortName(*Path)));
		else
			Name->SetText(FText::FromString(AlternativeName));
	
	Background->Path = Path;

	PathType = Type;
}

void UPinnedAssetSlotBase::SetThumbnail(UTexture2D* ThumbnailTexture)
{
	Thumbnail->SetBrushFromTexture(ThumbnailTexture);
}


FString UPinnedAssetSlotBase::GetAssetPath()
{
	return AssetPath;
}

void UPinnedAssetSlotBase::SetSize(int Width, int Height)
{
	SizeBox->SetHeightOverride(Height);
	SizeBox->SetWidthOverride(Width);
}

FReply UPinnedAssetSlotBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		return FReply::Unhandled();

	if (PathType == EPathType::Folder)
	{
		TArray<FString> Assets{ AssetPath };

		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		ContentBrowserModule.Get().SyncBrowserToFolders(Assets);

		return FReply::Handled();
	}

	TArray<FAssetData> Assets;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	AssetRegistry.GetAssetsByPackageName(FName(AssetPath), Assets);

	if (Assets.Num() <= 0)
		return FReply::Handled();

	UObject* Asset = Assets[0].GetAsset();

	if (!Asset)
	{
		UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed - Asset is not valid"));
		return FReply::Handled();
	}

	UAssetEditorSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr;
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed - Asset Editor Subsystem is not valid"));
		return FReply::Handled();
	}

	bool success = Subsystem->OpenEditorForAsset(Asset);
	if (success)
	{
		UE_LOG(LogTemp, Log, TEXT("Open Asset Window Succeeded"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Open Asset Window Failed"));

	return FReply::Handled();
}

void UPinnedAssetSlotBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Background->SetBrushColor(HoverColor);
}

void UPinnedAssetSlotBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Background->SetBrushColor(BaseColor);
}
