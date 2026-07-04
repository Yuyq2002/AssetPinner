#include "SPinnedTab.h"
#include "SVerticalTextBlock.h"
#include "SVerticalEditableText.h"
#include "ExtendedSlateBorder.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include <SlotDragOperation.h>
#include <PinnedAssetSubsystem.h>
#include "SPinnedSlot.h"
#include "Widgets/Colors/SColorPicker.h"

SPinnedTab::SPinnedTab()
{
	BaseColor = FLinearColor(0.007f, 0.007f, 0.007f, 1.0f);
	HoverColor = FLinearColor(0.024f, 0.024f, 0.024f, 1.0f);
	SelectedColor = FLinearColor(0.036f, 0.036f, 0.036f, 1.0f);
	bIsSelected = false;
}

void SPinnedTab::Construct(const FArguments& InArgs)
{
	Widget = InArgs._Widget;
	bIsPersistent = InArgs._IsPersistent;
	OnTabClickedDelegate = InArgs._OnTabClickedDelegate;
	OnNameChangedDelegate = InArgs._OnNameChangedDelegate;
	OnRemoveDelegate = InArgs._OnRemoveDelegate;

	ChildSlot
		[
			SNew(SBox)
				.WidthOverride(35)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Center)
				.Visibility(EVisibility::SelfHitTestInvisible)
				[
					SAssignNew(Background, SExtendedSlateBorder)
						.BorderBackgroundColor(BaseColor)
						.ContextMenuExtender(this, &SPinnedTab::BuildContextMenu)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(0, 5, 0, 5))
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
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
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
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
										.Text(InArgs._Name)
										.Rotation(ERotation::CounterClockwise)
										.Visibility(EVisibility::Collapsed)
								]
						]
				]
		];

	if (InArgs._InitInRenameMode)
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime) -> bool
			{
				ActivateRenameBox();

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
		FSlateApplication::Get().SetKeyboardFocus(RenameTextBox, EFocusCause::SetDirectly);
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
		OnTabClickedDelegate.Execute(SharedThis(this));
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
	TSharedPtr< FDragDropOperation > Operation = DragDropEvent.GetOperation();

	if (!Operation.IsValid() || !Operation->IsOfType<FSlotDragOperation>())
		return FReply::Handled();

	TSharedPtr<FSlotDragOperation> SlotOperation = StaticCastSharedPtr<FSlotDragOperation>(Operation);

	TSharedPtr<SPinnedSection> OriginalParent = SlotOperation->OriginalParent.Pin();
	TSharedPtr<SPinnedSlot> DraggedSlot = SlotOperation->DraggedWidget.Pin();

	if (OriginalParent == Widget)
		return FReply::Handled();

	UPinnedAssetSubsystem* Subsystem = GEditor ? GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>() : nullptr;
	if (!Subsystem)
		return FReply::Handled();

	if (!Subsystem->MoveAssetPath(DraggedSlot->GetAssetPath(), Text->GetText().ToString()))
	{
		return FReply::Handled();
	}

	return FReply::Handled();
}

void SPinnedTab::OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod)
{
	OnNameChangedDelegate.ExecuteIfBound(SharedThis(this), Text->GetText(), InText);

	EditName(false);
}

void SPinnedTab::OnRemoveClicked()
{
	OnRemoveDelegate.ExecuteIfBound(SharedThis(this));
}

void SPinnedTab::BuildContextMenu(FMenuBuilder& Builder)
{
#define LOCTEXT_NAMESPACE "TabContextMenu"
	Builder.BeginSection("EditSection", LOCTEXT("Heading", "Edit Action"));
	{
		FUIAction RenameTabAction(
			FExecuteAction::CreateRaw(this, &SPinnedTab::OpenRenameBox),
			FCanExecuteAction::CreateRaw(this, &SPinnedTab::CanEdit)
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "TabRenameLabel", "Rename"),
			NSLOCTEXT("AssetPinner", "TabRenameTooltip", "Rename the tab"),
			FSlateIcon(),
			RenameTabAction
		);

		Builder.AddSubMenu(
			NSLOCTEXT("AssetPinner", "ColorPickerLabel", "Change Color"),
			NSLOCTEXT("AssetPinner", "ColorPickerTooltip", "Open color picker"),
			FNewMenuDelegate::CreateRaw(this, &SPinnedTab::OpenColorPicker)
		);

		FUIAction RemoveTabAction(
			FExecuteAction::CreateRaw(this, &SPinnedTab::RemoveTab),
			FCanExecuteAction::CreateRaw(this, &SPinnedTab::CanEdit)
		);

		Builder.AddMenuEntry(
			NSLOCTEXT("AssetPinner", "TabRemoveLabel", "Remove"),
			NSLOCTEXT("AssetPinner", "TabRemoveTooltip", "Remove the tab"),
			FSlateIcon(),
			RemoveTabAction
		);
	}
	Builder.EndSection();
#undef LOCTEXT_NAMESPACE
}

void SPinnedTab::OpenRenameBox()
{
	ActivateRenameBox();
}

bool SPinnedTab::CanEdit()
{
	return !bIsPersistent;
}

void SPinnedTab::OpenColorPicker(FMenuBuilder& Builder)
{
	Builder.AddWidget(
		SNew(SColorPicker)
			.TargetColorAttribute(BaseColor)
			.UseAlpha(false)
			.OnlyRefreshOnMouseUp(false)
			.OnlyRefreshOnOk(false)
			.OnColorCommitted(FOnLinearColorValueChanged::CreateRaw(this, &SPinnedTab::OnSetColorFromColorPicker))
			.OnColorPickerCancelled(FOnColorPickerCancelled::CreateRaw(this, &SPinnedTab::OnColorPickerCancelled))
			.DisplayGamma(GEngine->GetDisplayGamma()),
		FText()
		);
}

void SPinnedTab::RemoveTab()
{
	OnRemoveDelegate.Execute(SharedThis(this));
}

void SPinnedTab::OnSetColorFromColorPicker(FLinearColor NewColor)
{
	BaseColor = NewColor;
	HoverColor = BaseColor * FLinearColor(HoveredRatio, HoveredRatio, HoveredRatio, 1);
	SelectedColor = BaseColor * FLinearColor(SelectedRatio, SelectedRatio, SelectedRatio, 1);

	if (bIsSelected)
		Background->SetBorderBackgroundColor(SelectedColor);
	else
		Background->SetBorderBackgroundColor(BaseColor);
}

void SPinnedTab::OnColorPickerCancelled(FLinearColor OriginalColor)
{
	BaseColor = OriginalColor;
	HoverColor = BaseColor * FLinearColor(HoveredRatio, HoveredRatio, HoveredRatio, 1);
	SelectedColor = BaseColor * FLinearColor(SelectedRatio, SelectedRatio, SelectedRatio, 1);

	if(bIsSelected)
		Background->SetBorderBackgroundColor(SelectedColor);
	else
		Background->SetBorderBackgroundColor(BaseColor);
}