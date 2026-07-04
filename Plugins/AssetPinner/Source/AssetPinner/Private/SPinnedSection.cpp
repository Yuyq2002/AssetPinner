#include "SPinnedSection.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "SPinnedSlot.h"
#include "PinnedAssetSubsystem.h"
#include "SlotDragOperation.h"

SPinnedSection::SPinnedSection()
{
}

void SPinnedSection::Construct(const FArguments& InArgs)
{
	TabName = InArgs._TabName;
	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			BuildSectionWidget().ToSharedRef()
		];
}

void SPinnedSection::ClearPinnedSlots()
{
	WrapBox->ClearChildren();
}

void SPinnedSection::AddPinnedSlot(TSharedRef<SPinnedSlot> NewPinnedSlot)
{
	WrapBox->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			NewPinnedSlot
		];
}

void SPinnedSection::SetEnableScrolling(bool IsEnabled)
{
	ScrollBox->SetEnabled(IsEnabled);
}

TSharedPtr<SWidget> SPinnedSection::BuildSectionWidget()
{
	return SAssignNew(ScrollBox, SScrollBox)
		+ SScrollBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SAssignNew(WrapBox, SWrapBox)
				.Orientation(EOrientation::Orient_Horizontal)
				.UseAllottedSize(true)
		];
}

bool SPinnedSection::IsInSection(TSharedPtr<SPinnedSlot> Slot)
{
	FChildren* AllChildren = WrapBox->GetChildren();

	for(int i = 0; i < AllChildren->Num(); i++)
	{
		TSharedRef<SWidget> ChildWidget = AllChildren->GetChildAt(i);
		if (ChildWidget == Slot)
			return true;
	}

	return false;
}

FReply SPinnedSection::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr< FDragDropOperation > Operation = DragDropEvent.GetOperation();

	if (!Operation.IsValid())
		return FReply::Handled();

	if (Operation->IsOfType<FSlotDragOperation>())
	{
		TSharedPtr<FSlotDragOperation> SlotOperation = StaticCastSharedPtr<FSlotDragOperation>(Operation);

		TSharedPtr<SPinnedSlot> DraggedSlot = SlotOperation->DraggedWidget.Pin();

		DraggedSlot->SetVisibility(EVisibility::Visible);
	}
	else if(Operation->IsOfType<FAssetDragDropOp>())
	{
		TSharedPtr<FAssetDragDropOp> AssetOperation = StaticCastSharedPtr<FAssetDragDropOp>(Operation);

		UPinnedAssetSubsystem* Subsystem = nullptr;
		if (GEditor)
			Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();

		if (!Subsystem)
			return FReply::Handled();

		int TabIndex = Subsystem->GetTabIndex(TabName);
		
		if (AssetOperation->HasAssets())
		{
			TArray<FAssetData> DraggedAssets = AssetOperation->GetAssets();

			for (const FAssetData& AssetData : DraggedAssets)
			{
				FString AssetPath = AssetData.PackageName.ToString();
				Subsystem->AddAssetPath(AssetPath, EPathType::Asset, TabIndex);
			}
		}

		if (AssetOperation->HasAssetPaths())
		{
			TArray<FString> DraggedAssets = AssetOperation->GetAssetPaths();

			for (const FString& AssetPath : DraggedAssets)
			{
				Subsystem->AddAssetPath(AssetPath, EPathType::Folder, TabIndex);
			}
		}
	}

	return FReply::Handled();
}

SHistorySection::SHistorySection()
{
}

void SHistorySection::Construct(const FArguments& InArgs)
{
	TabName = InArgs._TabName;
	ChildSlot
		[
			SNew(SConstraintCanvas)
				+ SConstraintCanvas::Slot()
				.Anchors(FAnchors(0, 0, 1, 1))
				.Offset(FMargin(0, 0, 0, 60))
				[
					BuildSectionWidget().ToSharedRef()
				]
				+ SConstraintCanvas::Slot()
				.Anchors(FAnchors(0, 1, 1, 1))
				.Offset(FMargin(0, 0, 0, 60))
				.Alignment(FVector2D(0, 1))
				[
					SNew(SButton)
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						.OnClicked(this, &SHistorySection::OnClearClicked)
						[
							SNew(STextBlock)
								.Text(FText::FromString("Clear"))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 32))
								.Justification(ETextJustify::Center)
						]
				]
		];
}

FReply SHistorySection::OnClearClicked()
{
	UPinnedAssetSubsystem* PinnedAssetSubsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (PinnedAssetSubsystem)
		PinnedAssetSubsystem->ClearRecent();

	return FReply::Handled();
}