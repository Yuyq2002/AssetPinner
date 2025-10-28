// Fill out your copyright notice in the Description page of Project Settings.

// Header
#include "PinnedWindowBase.h"

// Unreal Headers
#include "Components/WidgetSwitcher.h"
#include "Components/WidgetSwitcherSlot.h"
#include "Components/HorizontalBox.h"
#include "EditorUtilityWidgetComponents.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IImageWrapper.h" 
#include "IImageWrapperModule.h" 
#include <ObjectTools.h>
#include <ImageUtils.h>

// Custom Headers
#include "PinnedAssetSubsystem.h"
#include "Tab.h"
#include "PinnedAssetSlotBase.h"
#include "PinnedSectionBase.h"
#include "Structs.h"

void UPinnedWindowBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!GEditor)
		return;

	PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->OnListChangedDelegate.BindDynamic(this, &UPinnedWindowBase::OnListChangedCallback);

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
		PinnedTab->SetInfo(FText::FromString("Pinned"), this, 0);
		PinnedTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
		ActiveTab = PinnedTab;
		ActiveTab->SetSelected(true);
		UTab* HistoryTab = CreateWidget<UTab>(this, TabWidget);
		HistoryTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
		HistoryTab->SetInfo(FText::FromString("History"), this, 1);

		TabList->AddChild(PinnedTab);
		TabList->AddChild(HistoryTab);
	}

	Refresh(PinnedAssetSubsystem->GetAssetDataList());

	if (ClearButton)
		ClearButton->OnClicked.AddDynamic(this, &UPinnedWindowBase::OnClearButtonClicked);
	if (NewTabButton)
		NewTabButton->OnClicked.AddDynamic(this, &UPinnedWindowBase::OnNewTabClicked);
}

void UPinnedWindowBase::NativeDestruct()
{
	FFileHelper::SaveStringToFile(FString::SanitizeFloat(Size), *ConfigPath);
}

EditState UPinnedWindowBase::CheckInEditMode()
{
	return EditMode;
}


void UPinnedWindowBase::SwitchTab(int Index)
{
	TabController->SetActiveWidgetIndex(Index);
}

void UPinnedWindowBase::OnListChangedCallback(const TArray<FPinnedAssetData>& List)
{
	Refresh(List);
}

void UPinnedWindowBase::Refresh(const TArray<FPinnedAssetData>& List)
{
	if (!AssetSlotWidget) return;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	PinnedSection->ClearPinnedAsset();
	RecentSection->ClearPinnedAsset();
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
			PinnedSection->AddPinnedAsset(NewSlot);
		else
			RecentSection->AddPinnedAsset(NewSlot);
	}

	return;
}

void UPinnedWindowBase::OnTabClicked(UTab* Initiator)
{
	ActiveTab->SetSelected();
	ActiveTab = Initiator;
	SwitchTab(ActiveTab->SetSelected(true));
}

void UPinnedWindowBase::OnTabRenamed(UTab* Initiator, FText OldName, FText NewName)
{
	Initiator->SetInfo(NewName);

	int Index;
	if (FindSection(OldName.ToString(), Index))
	{
		SectionMap[Index].Name = NewName.ToString();
	}
}

void UPinnedWindowBase::OnClearButtonClicked()
{
	PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->ClearRecent();
}

void UPinnedWindowBase::OnNewTabClicked()
{
	UTab* NewTab = CreateWidget<UTab>(this, TabWidget);
	NewTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
	NewTab->OnNameChangedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabRenamed);
	NewTab->SetInfo(FText::FromString("NewTab"), this, TabList->GetChildrenCount() + 1);
	NewTab->EditName(true);

	TabList->AddChild(NewTab);

	UPinnedSectionBase* NewSection = CreateWidget<UPinnedSectionBase>(this, UPinnedSectionBase::StaticClass());
	UWidgetSwitcherSlot* NewSlot = Cast<UWidgetSwitcherSlot>(TabController->AddChild(NewSection));
	NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	SectionMap.Add(FSection("NewTab", NewSection));
}


FReply UPinnedWindowBase::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::InEditMode;
		PinnedSection->SetEnableScrolling(false);
		RecentSection->SetEnableScrolling(false);
	}
	return FReply::Handled();
}

FReply UPinnedWindowBase::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::NotInEditMode;
		PinnedSection->SetEnableScrolling(true);
		RecentSection->SetEnableScrolling(true);
	}
	return FReply::Handled();
}

FReply UPinnedWindowBase::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

FReply UPinnedWindowBase::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::NotInEditMode;
	return FReply::Handled();
}

void UPinnedWindowBase::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::Unfocused;
}

bool UPinnedWindowBase::FindSection(FString Name, int& OutIndex)
{
	int Index = 0;
	for (auto& Section : SectionMap)
	{
		if (Section.Name == Name)
		{
			OutIndex = Index;
			return true;
		}

		Index++;
	}

	OutIndex = -1;
	return false;
}

// Old code to get the thumbnail of an asset
//UTexture2D* UPinnedWindowBase::GetObjectThumbnailAsTexture2D(const FAssetData& AssetData)
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
