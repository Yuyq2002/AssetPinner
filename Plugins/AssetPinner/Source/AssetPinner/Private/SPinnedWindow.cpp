#include "SPinnedWindow.h"
#include "PinnedAssetSubsystem.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "SPinnedSection.h"
#include "SPinnedTab.h"
#include "SPinnedSlot.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetPinnerPublic.h"

#include "SVerticalTextBlock.h"

TSharedRef<SWidget> FAssetPinnerPublic::MakeAssetPinnerWidget()
{
	return SNew(SPinnedWindow);
}

void SPinnedWindow::Construct(const FArguments& InArgs)
{
	TSharedPtr<SPinnedSection> PinnedSection;
	TSharedPtr<SPinnedSection> HistorySection;

	ChildSlot
		[
			SNew(SConstraintCanvas)
				+ SConstraintCanvas::Slot()
				.Anchors(FAnchors(0, 0, 0, 1))
				.Offset(FMargin(0, 0, 40, 40))
				.Alignment(FVector2D(0, 1))
				[
					SNew(SScrollBox)
						.ScrollBarThickness(FVector2D(9, 5))
						.ScrollBarAlwaysVisible(true)
						+ SScrollBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SAssignNew(TabList, SVerticalBox)
						]
				]
				+ SConstraintCanvas::Slot()
				.Anchors(FAnchors(0, 1, 0, 1))
				.Offset(FMargin(0, 0, 40, 40))
				.Alignment(FVector2D(0, 1))
				[
						SNew(SButton)
								.OnClicked(this, &SPinnedWindow::OnNewTabClicked)
				]
				+ SConstraintCanvas::Slot()
				.Anchors(FAnchors(0, 0, 1, 1))
				.Offset(FMargin(40, 0, 0, 0))
				[
					SAssignNew(TabController, SWidgetSwitcher)
						+ SWidgetSwitcher::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							SAssignNew(PinnedSection, SPinnedSection)
						]
						+ SWidgetSwitcher::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							SAssignNew(HistorySection, SHistorySection)
						]
				]
		];

	TabController->SetActiveWidget(PinnedSection.ToSharedRef());

	if (PinnedSection)
		SectionMap.Add(FSection("Pinned", PinnedSection.ToWeakPtr(), true));

	if (HistorySection)
		SectionMap.Add(FSection("History", HistorySection.ToWeakPtr(), true));

	if (TabList)
	{
		TabList->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.AutoHeight()
			[
				SAssignNew(ActiveTab, SPinnedTab)
					.Name(FText::FromString("Pinned"))
					.Widget(PinnedSection)
					.IsPersistent(true)
					.OnTabClickedDelegate(this, &SPinnedWindow::OnTabClicked)
			];
		ActiveTab->SetSelected(true);

		TabList->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.AutoHeight()
			[
				SNew(SPinnedTab)
					.Name(FText::FromString("History"))
					.Widget(HistorySection)
					.IsPersistent(true)
					.OnTabClickedDelegate(this, &SPinnedWindow::OnTabClicked)
			];
	}

	TArray<FString> Data;
	ConfigPath = FPaths::GameUserDeveloperDir() + "PinnedAssetConfig.txt";
	if (FPaths::ValidatePath(ConfigPath))
	{
		FFileHelper::LoadFileToStringArray(Data, *ConfigPath);
		if (!Data.IsEmpty())
		{
			if (Data[0].IsNumeric())
			{
				Size = FCString::Atof(*Data[0]);
				Data.RemoveAt(0);
			}
		}
	}

	for (auto& line : Data)
	{
		TSharedPtr<SPinnedSection> NewSection;
		TabController->AddSlot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SAssignNew(NewSection, SPinnedSection)
			];

		SectionMap.Add(FSection(line, NewSection));

		TabList->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.AutoHeight()
			[
				SNew(SPinnedTab)
					.Name(FText::FromString(line))
					.Widget(NewSection)
					.IsPersistent(false)
					.OnTabClickedDelegate(this, &SPinnedWindow::OnTabClicked)
					.OnNameChangedDelegate(this, &SPinnedWindow::OnTabRenamed)
					.OnRemoveDelegate(this, &SPinnedWindow::OnTabRemoved)
			];
	}
	
	PinnedAssetSubsystem->SetTabs(SectionMap);

	Refresh(PinnedAssetSubsystem->GetAssetDataList());
}

SPinnedWindow::SPinnedWindow()
{
	if (!GEngine)
		return;

	PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!PinnedAssetSubsystem)
		return;

	PinnedAssetSubsystem->OnListChangedDelegate.BindRaw(this, &SPinnedWindow::OnListChangedCallback);
}

SPinnedWindow::~SPinnedWindow()
{
	FString SaveConfig;
	SaveConfig += FString::SanitizeFloat(Size) + '\n';

	for (auto& Section : SectionMap)
		if (!Section.bIsPersistent)
			SaveConfig += Section.Name + '\n';

	FFileHelper::SaveStringToFile(SaveConfig, *ConfigPath);

	PinnedAssetSubsystem->EmptyTabName();
}

EditState SPinnedWindow::CheckInEditMode()
{
	return EditMode;
}

void SPinnedWindow::SwitchTab(TSharedRef<SWidget> Widget)
{
	TabController->SetActiveWidget(Widget);
}

void SPinnedWindow::OnListChangedCallback(const TArray<FPinnedAssetData>& List)
{
	Refresh(List);
}

void SPinnedWindow::Refresh(const TArray<FPinnedAssetData>& List)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	Slots.Empty();
	for (auto& Section : SectionMap)
		if (TSharedPtr<SPinnedSection> SectionPtr = Section.SectionWidget.Pin())
			SectionPtr->ClearPinnedSlots();

	for (const auto& Data : List)
	{
		TArray<FAssetData> Assets;
		AssetRegistry.GetAssetsByPackageName(FName(Data.AssetPath), Assets);
		if (Assets.Num() <= 0 && Data.PathType == EPathType::Asset)
			continue;

		TSharedPtr<SPinnedSection> SectionPtr = SectionMap[Data.TabIndex].SectionWidget.Pin();
		TSharedPtr<SPinnedSlot> NewSlot;
		if (SectionPtr.IsValid())
		{
			SectionPtr->AddPinnedSlot(
				SAssignNew(NewSlot, SPinnedSlot)
				.Data(Data)
				.AssetData(Assets.IsValidIndex(0) ? Assets[0] : nullptr)
				.Size(Size)
			);
		};

		Slots.Add(NewSlot.ToWeakPtr());
	}

	return;
}

void SPinnedWindow::OnTabClicked(TSharedPtr<SPinnedTab> Initiator)
{
	ActiveTab->SetSelected();
	ActiveTab = Initiator;
	SwitchTab(ActiveTab->SetSelected(true).ToSharedRef());
}

void SPinnedWindow::OnTabRenamed(TSharedPtr<SPinnedTab> Initiator, FText OldName, FText NewName)
{
	Initiator->SetName(NewName);

	int Index;
	if (FindSection(OldName.ToString(), Index))
	{
		SectionMap[Index].Name = NewName.ToString();
		PinnedAssetSubsystem->RenameTab(NewName.ToString(), Index);
	}
}

void SPinnedWindow::OnTabRemoved(TSharedPtr<SPinnedTab> Initiator)
{
	TSharedPtr<SPinnedSection> Section = Initiator->GetSection();

	int Index = -1;
	if (FindSection(Initiator->GetName(), Index))
	{
		SectionMap.RemoveAt(Index);
		PinnedAssetSubsystem->RemoveTab(Index);
	}

	TabController->RemoveSlot(Section.ToSharedRef());
	TabList->RemoveSlot(Initiator.ToSharedRef());
}

void SPinnedWindow::OnClearButtonClicked()
{
	PinnedAssetSubsystem->ClearRecent();
}

FReply SPinnedWindow::OnNewTabClicked()
{
	FString NewName = "NewTab_" + FString::FromInt(DefaultNameIndex);
	while (ContainsSection(NewName))
	{
		DefaultNameIndex++;
		NewName = "NewTab_" + FString::FromInt(DefaultNameIndex);
	}

	TSharedPtr<SPinnedSection> NewSection;
	TabController->AddSlot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SAssignNew(NewSection, SPinnedSection)
		];

	SectionMap.Add(FSection(NewName, NewSection));

	TabList->AddSlot()
		[
			SNew(SPinnedTab)
				.Name(FText::FromString(NewName))
				.Widget(NewSection)
				.IsPersistent(true)
				.OnTabClickedDelegate(this, &SPinnedWindow::OnTabClicked)
				.OnNameChangedDelegate(this, &SPinnedWindow::OnTabRenamed)
				.OnRemoveDelegate(this, &SPinnedWindow::OnTabRemoved)
		];

	PinnedAssetSubsystem->AddTabNames(NewName);

	DefaultNameIndex++;

	return FReply::Handled();
}


FReply SPinnedWindow::OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::InEditMode;
		for (auto& Section : SectionMap)
			if (TSharedPtr<SPinnedSection> SectionPtr = Section.SectionWidget.Pin())
				SectionPtr->SetEnableScrolling(false);
	}
	return FReply::Handled();
}

FReply SPinnedWindow::OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		EditMode = EditState::NotInEditMode;
		for (auto& Section : SectionMap)
			if (TSharedPtr<SPinnedSection> SectionPtr = Section.SectionWidget.Pin())
				SectionPtr->SetEnableScrolling(true);
	}
	return FReply::Handled();
}

FReply SPinnedWindow::OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (EditMode == EditState::InEditMode)
	{
		Size += InMouseEvent.GetWheelDelta() * 10;
		Size = FMath::Max(Size, MinSize);

		for (auto PinSlot : Slots)
		{
			if (TSharedPtr<SPinnedSlot> SlotPtr = PinSlot.Pin())
				SlotPtr->SetSize(Size, Size * Ratio);
		}
		return FReply::Handled();
	}

	return FReply::Handled();
}

FReply SPinnedWindow::OnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::NotInEditMode;
	return FReply::Handled();
}

void SPinnedWindow::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	EditMode = EditState::Unfocused;
}

bool SPinnedWindow::FindSection(FString Name, int& OutIndex)
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

bool SPinnedWindow::ContainsSection(FString Name)
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
//UTexture2D* SPinnedWindow::GetObjectThumbnailAsTexture2D(const FAssetData& AssetData)
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
