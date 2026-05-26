#include "SPinnedTab.h"
#include "SVerticalTextBlock.h"
#include "SVerticalEditableText.h"
#include "ExtendedSlateBorder.h"
#include "Widgets/Layout/SConstraintCanvas.h"

SPinnedTab::SPinnedTab()
{
}

void SPinnedTab::Construct(const FArguments& InArgs)
{
	Widget = InArgs._Widget;
	bIsPersistent = InArgs._IsPersistent;

	ChildSlot
		[
			SNew(SBox)
				.WidthOverride(35)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				[
					SAssignNew(Background, SExtendedSlateBorder)
						.ContextMenuExtender(this, &SPinnedTab::BuildContextMenu)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Center)
						.Padding(FMargin(0, 3, 0, 3))
						[
							SNew(SConstraintCanvas)
								+ SConstraintCanvas::Slot()
								.AutoSize(true)
								.Anchors(FAnchors(0.5, 0.5))
								.Alignment(FVector2D(0.5f, 0.5f))
								.ZOrder(1)
								[
									SAssignNew(Text, SVerticalTextBlock)
										.Text(InArgs._Name)
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
										.Rotation(ERotation::CounterClockwise)
										.ColorAndOpacity(FLinearColor::White)
								]
								+ SConstraintCanvas::Slot()
								.AutoSize(true)
								.Anchors(FAnchors(0.5, 0.5))
								.Alignment(FVector2D(0.5f, 0.5f))
								.ZOrder(1)
								[
									SAssignNew(RenameTextBox, SVerticalEditableText)
										.OnTextCommitted(this, &SPinnedTab::OnNameChanged)
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
										.Text(InArgs._Name)
										.Rotation(ERotation::CounterClockwise)
										.Visibility(EVisibility::Collapsed)
								]
						]
				]
		];

	if (bInitInRenameMode)
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime) -> bool
			{
				ActivateRenameBox();

				// Return false to tell the ticker to stop. (If you return true, it loops).
				return false;

			}), 0.0f);
}

void SPinnedTab::SetName(FText InName)
{
	if (Text)
		Text->SetText(InName);

	if (RenameTextBox)
		RenameTextBox->SetText(InName);
}

void SPinnedTab::EditName(bool EnableEditing)
{
	if (EnableEditing)
	{
		RenameTextBox->SetVisibility(EVisibility::Visible);
		Text->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		RenameTextBox->SetVisibility(EVisibility::Collapsed);
		Text->SetVisibility(EVisibility::Visible);
	}
}

void SPinnedTab::ActivateRenameBox()
{
	EditName(true);
}

TSharedPtr<SPinnedSection> SPinnedTab::SetSelected(bool IsSelected)
{
	if(!Background.IsValid())
		return nullptr;

	bIsSelected = IsSelected;

	if (bIsSelected)
	{
		Background->SetBorderBackgroundColor(SelectedColor);
		return Widget;
	}
	else
	{
		Background->SetBorderBackgroundColor(BaseColor);
		return nullptr;
	}
}

TSharedPtr<SPinnedSection> SPinnedTab::GetSection()
{
	return Widget;
}

FString SPinnedTab::GetName()
{
	return Text->GetText().ToString();
}

FReply SPinnedTab::OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && OnTabClickedDelegate.IsBound())
	{
		OnTabClickedDelegate.Execute(this);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SPinnedTab::OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!Background.IsValid())
		return;

	if (!bIsSelected)
		Background->SetBorderBackgroundColor(HoverColor);
}

void SPinnedTab::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!Background.IsValid())
		return;

	if (!bIsSelected)
		Background->SetBorderBackgroundColor(BaseColor);
}

FReply SPinnedTab::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	//USlotDragOperation* Operation = Cast<USlotDragOperation>(InOperation);

	//if (!Operation)
	//	return false;

	//if (Operation->OriginalParent == Widget)
	//	return false;

	//UPinnedAssetSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>() : nullptr;
	//if (!Subsystem)
	//	return false;

	//if (!Subsystem->MoveAssetPath(Operation->DraggedWidget->GetAssetPath(), Text->GetText().ToString()))
	//{
	//	return false;
	//}

	//return true;

	return FReply::Unhandled();
}

void SPinnedTab::OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod)
{
	OnNameChangedDelegate.ExecuteIfBound(this, Text->GetText(), InText);

	EditName(false);
}

void SPinnedTab::OnRemoveClicked()
{
	OnRemoveDelegate.ExecuteIfBound(this);
}

void SPinnedTab::BuildContextMenu(FMenuBuilder& Builder)
{

}