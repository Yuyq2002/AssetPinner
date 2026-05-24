// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSlotBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PinnedWindowBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components\AssetThumbnailWidget.h"
#include <ContentBrowserModule.h>
#include "IContentBrowserSingleton.h"
#include "Structs.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBoxSlot.h"
#include "SlotDragOperation.h"

void UPinnedAssetSlotBase::SetAssetData(const FPinnedAssetData& Data)
{
	AssetPath = Data.AssetPath;

	if (Name)
		Name->SetText(FText::FromString(FPackageName::GetShortName(*Data.AssetPath)));

	Background->AssetPath = Data.AssetPath;

	PathType = Data.PathType;
}

void UPinnedAssetSlotBase::SetThumbnail(const FAssetData& AssetData, const UPinnedAssetSubsystem* PinnedAssetSubsystem)
{
	USizeBoxSlot* SlotPtr = nullptr;

	if (PathType == EPathType::Folder)
	{
		UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Thumbnail"));
		Image->SetBrushFromTexture(PinnedAssetSubsystem->FolderIcon);
		SlotPtr = Cast<USizeBoxSlot>(ThumbnailHolder->AddChild(Image));
	}
	else if (PathType == EPathType::Asset)
	{
		Thumbnail = WidgetTree->ConstructWidget<UAssetThumbnailWidget>(UAssetThumbnailWidget::StaticClass(), TEXT("Thumbnail"));
		Thumbnail->SetAsset(AssetData);
		SlotPtr = Cast<USizeBoxSlot>(ThumbnailHolder->AddChild(Thumbnail));
	}

	if (SlotPtr)
	{
		SlotPtr->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		SlotPtr->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
}


FString UPinnedAssetSlotBase::GetAssetPath()
{
	return AssetPath;
}

void UPinnedAssetSlotBase::SetSize(int Width, int Height)
{
	SizeBox->SetHeightOverride(Height);
	SizeBox->SetWidthOverride(Width);

	if (Thumbnail)
		Thumbnail->SetResolution(FIntPoint(Width));
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

FReply UPinnedAssetSlotBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FEventReply Reply;
		Reply.NativeReply = FReply::Handled();

		if (this)
		{
			TSharedPtr<SWidget> SlateWidgetDetectingDrag = GetCachedWidget();
			if (SlateWidgetDetectingDrag.IsValid())
			{
				Reply.NativeReply = Reply.NativeReply.DetectDrag(SlateWidgetDetectingDrag.ToSharedRef(), EKeys::LeftMouseButton);
				return Reply.NativeReply;
			}
		}
	}

	return FReply::Unhandled();
}

void UPinnedAssetSlotBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	USlotDragOperation* DragDropOperation = NewObject<USlotDragOperation>();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	DragDropOperation->DraggedWidget = this;
	DragDropOperation->DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	DragDropOperation->OriginalParent = GetParent();

	DragDropOperation->DefaultDragVisual = this;	
	DragDropOperation->Pivot = EDragPivot::MouseDown;

	OutOperation = DragDropOperation;
}

void UPinnedAssetSlotBase::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SetVisibility(ESlateVisibility::Visible);
}
