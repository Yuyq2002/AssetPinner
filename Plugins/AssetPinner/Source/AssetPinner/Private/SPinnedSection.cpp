#include "SPinnedSection.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "SPinnedSlot.h"
#include "PinnedAssetSubsystem.h"

SPinnedSection::SPinnedSection()
{
}

void SPinnedSection::Construct(const FArguments& InArgs)
{
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

FReply SPinnedSection::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	/*USlotDragOperation* Operation = Cast<USlotDragOperation>(InOperation);
	if (!Operation)
		return false;

	if (Operation->OriginalParent != WrapBox)
	{
		Operation->DraggedWidget->RemoveFromParent();
		WrapBox->AddChildToWrapBox(Operation->DraggedWidget);
	}

	Operation->DraggedWidget->SetVisibility(ESlateVisibility::Visible);

	return true;*/

	return FReply::Unhandled();
}

SHistorySection::SHistorySection()
{
}

void SHistorySection::Construct(const FArguments& InArgs)
{
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