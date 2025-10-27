// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedSectionBase.h"
#include "PinnedAssetSubsystem.h"
#include "Components/WrapBox.h"
#include "Components/ScrollBox.h"
#include "EditorUtilityWidgetComponents.h"
#include "PinnedAssetSlotBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IImageWrapper.h" 
#include "IImageWrapperModule.h" 
#include <ObjectTools.h>
#include <ImageUtils.h>
#include "Components/WidgetSwitcher.h"
#include "Tab.h"
#include "Components/HorizontalBox.h"

void UPinnedSectionBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!GEditor)
		return;

	PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->OnListChangedDelegate.BindDynamic(this, &UPinnedSectionBase::OnListChangedCallback);

	ConfigPath = FPaths::GameUserDeveloperDir() + "PinnedAssetConfig.txt";
	if (FPaths::ValidatePath(ConfigPath))
	{
		FString Data;
		FFileHelper::LoadFileToString(Data, *ConfigPath);
		Size = FCString::Atof(*Data);
	}

	if (TabList)
	{
		UTab* PinnedTab = CreateWidget<UTab>(this, TabWidget);
		PinnedTab->SetInfo("Pinned", this, 0);
		PinnedTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedSectionBase::OnTabClicked);
		ActiveTab = PinnedTab;
		ActiveTab->SetSelected(true);
		UTab* HistoryTab = CreateWidget<UTab>(this, TabWidget);
		HistoryTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedSectionBase::OnTabClicked);
		HistoryTab->SetInfo("History", this, 1);

		TabList->AddChild(PinnedTab);
		TabList->AddChild(HistoryTab);
	}

	Refresh(PinnedAssetSubsystem->GetAssetDataList());

	ClearButton->OnClicked.AddDynamic(this, &UPinnedSectionBase::OnClearButtonClicked);
}

void UPinnedSectionBase::NativeDestruct()
{
	//FFileHelper::SaveStringToFile(FString::SanitizeFloat(Size), *ConfigPath);
}

EditState UPinnedSectionBase::CheckInEditMode()
{
	return EditMode;
}


void UPinnedSectionBase::AddRecheck(UPinnedAssetSlotBase* Caller, FKey Input)
{
	RecallEditAction = Caller;
	MouseInput = Input;
}

void UPinnedSectionBase::SwitchTab(int Index)
{
	TabController->SetActiveWidgetIndex(Index);
}

void UPinnedSectionBase::OnListChangedCallback(const TArray<FPinnedAssetData>& List)
{
	Refresh(List);
}

void UPinnedSectionBase::Refresh(const TArray<FPinnedAssetData>& List)
{
	if (!AssetSlotWidget) return;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	PinnedWrapBox->ClearChildren();
	RecentWrapBox->ClearChildren();
	Slots.Empty();

	for (const auto& Data : List)
	{
		TArray<FAssetData> Assets;
		AssetRegistry.GetAssetsByPackageName(FName(Data.AssetPath), Assets);
		if (Assets.Num() <= 0 && Data.PathType == EPathType::Asset)
			continue;

		UPinnedAssetSlotBase* NewSlot = CreateWidget<UPinnedAssetSlotBase>(this, AssetSlotWidget);
		NewSlot->SetAssetData(Data);
		NewSlot->SetThumbnail(Assets.IsValidIndex(0) ? Assets[0] : nullptr, PinnedAssetSubsystem);
		NewSlot->SetSize(Size, Size * Ratio);
		Slots.Add(NewSlot);

		if (Data.PinnedStatus)
			PinnedWrapBox->AddChildToWrapBox(NewSlot);
		else
			RecentWrapBox->AddChildToWrapBox(NewSlot);
	}

	return;
}

void UPinnedSectionBase::OnTabClicked(UTab* Initiator)
{
	ActiveTab->SetSelected();
	ActiveTab = Initiator;
	SwitchTab(ActiveTab->SetSelected(true));
}

void UPinnedSectionBase::OnClearButtonClicked()
{
	PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->ClearRecent();
}


FReply UPinnedSectionBase::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::InEditMode;
		PinnedScrollBox->SetIsEnabled(false);
		RecentScrollBox->SetIsEnabled(false);
	}
	return FReply::Handled();
}

FReply UPinnedSectionBase::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::NotInEditMode;
		PinnedScrollBox->SetIsEnabled(true);
		RecentScrollBox->SetIsEnabled(true);
	}
	return FReply::Handled();
}

FReply UPinnedSectionBase::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (EditMode == EditState::InEditMode)
	{
		Size += InMouseEvent.GetWheelDelta() * 10;
		Size = FMath::Max(Size, MinSize);

		for (auto PinSlot : Slots)
		{
			PinSlot->SetSize(Size, Size * Ratio);
		}
		return FReply::Handled();
	}

	return FReply::Handled();
}

FReply UPinnedSectionBase::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::NotInEditMode;
	return FReply::Handled();
}

void UPinnedSectionBase::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::Unfocused;
	RecallEditAction = nullptr;
}

// Old code to get the thumbnail of an asset
//UTexture2D* UPinnedSectionBase::GetObjectThumbnailAsTexture2D(const FAssetData& AssetData)
//{
//	UTexture2D* CreatedTexture = nullptr;
//
//	FString PackageFilename;
//	const FName ObjectFullName = FName(*AssetData.GetFullName());
//	TSet<FName> ObjectFullNames;
//	ObjectFullNames.Add(ObjectFullName);
//	if (FPackageName::DoesPackageExist(AssetData.PackageName.ToString(), &PackageFilename))
//	{
//		FThumbnailMap ThumbnailMap;
//		ThumbnailTools::LoadThumbnailsFromPackage(PackageFilename, ObjectFullNames,
//			ThumbnailMap);
//		const FObjectThumbnail* Test = ThumbnailTools::GetThumbnailForObject(AssetData.GetAsset());
//
//		FObjectThumbnail* objTN = ThumbnailMap.Find(ObjectFullName);
//
//		IImageWrapperModule& ImageWrapperModule = FModuleManager::Get().LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));
//		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
//
//		ImageWrapper->SetRaw(objTN->GetUncompressedImageData().GetData(), objTN->GetUncompressedImageData().Num(), objTN->GetImageWidth(), objTN->GetImageHeight(), ERGBFormat::BGRA, 8);
//		const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();
//
//		CreatedTexture = FImageUtils::ImportBufferAsTexture2D(CompressedByteArray);
//
//		return CreatedTexture;
//	}
//	else
//		return nullptr;
//}
