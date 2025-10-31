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
	
	if (!GEngine)
		return;

	PinnedAssetSubsystem = GEngine->GetEngineSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->OnListChangedDelegate.BindDynamic(this, &UPinnedWindowBase::OnListChangedCallback);

	TArray<FString> Data;
	ConfigPath = FPaths::GameUserDeveloperDir() + "PinnedAssetConfig.txt";
	if (FPaths::ValidatePath(ConfigPath))
	{
		FFileHelper::LoadFileToStringArray(Data, *ConfigPath);
		Size = FCString::Atof(*Data[0]);
		Data.RemoveAt(0);
	}

	if (PinnedSection)
		SectionMap.Add(FSection("Pinned", PinnedSection, true));

	if (RecentSection)
		SectionMap.Add(FSection("Recent", RecentSection, true));

	if (TabList)
	{
		UTab* PinnedTab = CreateWidget<UTab>(this, TabWidget);
		PinnedTab->SetInfo(FText::FromString("Pinned"), this, PinnedSection, true);
		PinnedTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
		ActiveTab = PinnedTab;
		ActiveTab->SetSelected(true);
		UTab* HistoryTab = CreateWidget<UTab>(this, TabWidget);
		HistoryTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
		HistoryTab->SetInfo(FText::FromString("History"), this, RecentSection, true);

		TabList->AddChild(PinnedTab);
		TabList->AddChild(HistoryTab);
	}

	if (PinnedSectionWidget)
	{
		for (auto& line : Data)
		{
			UPinnedSectionBase* NewSection = CreateWidget<UPinnedSectionBase>(this, PinnedSectionWidget);
			UWidgetSwitcherSlot* NewSlot = Cast<UWidgetSwitcherSlot>(TabController->AddChild(NewSection));
			NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

			SectionMap.Add(FSection(line, NewSection));

			UTab* NewTab = CreateWidget<UTab>(this, TabWidget);
			NewTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
			NewTab->OnNameChangedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabRenamed);
			NewTab->OnRemoveClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabRemoved);
			NewTab->SetInfo(FText::FromString(line), this, NewSection);

			TabList->AddChild(NewTab);
		}
	}
	PinnedAssetSubsystem->SetTabs(SectionMap);

	Refresh(PinnedAssetSubsystem->GetAssetDataList());

	if (NewTabButton)
		NewTabButton->OnClicked.AddDynamic(this, &UPinnedWindowBase::OnNewTabClicked);
}

void UPinnedWindowBase::NativeDestruct()
{
	FString SaveConfig;
	SaveConfig += FString::SanitizeFloat(Size) + '\n';

	for (auto& Section : SectionMap)
		if(!Section.bIsPersistent)
			SaveConfig += Section.Name + '\n';

	FFileHelper::SaveStringToFile(SaveConfig, *ConfigPath);

	PinnedAssetSubsystem->EmptyTabName();
}

EditState UPinnedWindowBase::CheckInEditMode()
{
	return EditMode;
}


void UPinnedWindowBase::SwitchTab(UWidget* Widget)
{
	TabController->SetActiveWidget(Widget);
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

	Slots.Empty();
	for (auto& Section : SectionMap)
		Section.SectionWidget->ClearPinnedAsset();

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

		SectionMap[Data.TabIndex].SectionWidget->AddPinnedAsset(NewSlot);
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
		PinnedAssetSubsystem->RenameTab(NewName.ToString(), Index);
	}
}

void UPinnedWindowBase::OnTabRemoved(UTab* Initiator)
{
	UWidget* Section = Initiator->GetSection();

	int Index = -1;
	if (FindSection(Initiator->GetName(), Index))
	{
		SectionMap.RemoveAt(Index);
		PinnedAssetSubsystem->RemoveTab(Index);
	}

	TabController->RemoveChild(Section);
	TabList->RemoveChild(Initiator);
}

void UPinnedWindowBase::OnClearButtonClicked()
{
	PinnedAssetSubsystem->ClearRecent();
}

void UPinnedWindowBase::OnNewTabClicked()
{
	FString NewName = "NewTab_" + FString::FromInt(DefaultNameIndex);
	while (ContainsSection(NewName))
	{
		DefaultNameIndex++;
		NewName = "NewTab_" + FString::FromInt(DefaultNameIndex);
	}

	UPinnedSectionBase* NewSection = CreateWidget<UPinnedSectionBase>(this, PinnedSectionWidget);
	UWidgetSwitcherSlot* NewSlot = Cast<UWidgetSwitcherSlot>(TabController->AddChild(NewSection));
	NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	SectionMap.Add(FSection(NewName, NewSection));

	UTab* NewTab = CreateWidget<UTab>(this, TabWidget);
	NewTab->OnTabClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabClicked);
	NewTab->OnNameChangedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabRenamed);
	NewTab->OnRemoveClickedDelegate.BindDynamic(this, &UPinnedWindowBase::OnTabRemoved);
	NewTab->SetInfo(FText::FromString(NewName), this, NewSection);
	NewTab->EditName(true);

	TabList->AddChild(NewTab);
	PinnedAssetSubsystem->AddTabNames(NewName);

	DefaultNameIndex++;
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

bool UPinnedWindowBase::ContainsSection(FString Name)
{
	for (auto& Section : SectionMap)
	{
		if (Section.Name == Name)
		{
			return true;
		}
	}

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
