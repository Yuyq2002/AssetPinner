#include "SlotDragOperation.h"
#include "SPinnedSlot.h"
#include "PinnedAssetSubsystem.h"

TSharedRef<FSlotDragOperation> FSlotDragOperation::New(FAssetData InAssetData, UActorFactory* ActorFactory, TWeakPtr<SPinnedSlot> InDraggedWidget, TWeakPtr<SPinnedSection> InOriginalParent)
{
	TSharedPtr<FSlotDragOperation> Operation = MakeShared<FSlotDragOperation>();
	Operation->Init(TArray<FAssetData>{InAssetData}, TArray<FString>(), ActorFactory);
	Operation->DraggedWidget = InDraggedWidget;
	Operation->OriginalParent = InOriginalParent;
	Operation->Construct();
	return Operation.ToSharedRef();
}

TSharedPtr<SWidget> FSlotDragOperation::GetDefaultDecorator() const
{
	if(!DraggedWidget.IsValid())
		return nullptr;

	UPinnedAssetSubsystem* Subsystem = GEditor->GetEditorSubsystem<UPinnedAssetSubsystem>();
	if (!Subsystem)
		return nullptr;

	TSharedPtr<SPinnedSlot> DraggedSlot = DraggedWidget.Pin();

	return SNew(SBox)
		.WidthOverride(100)
		.HeightOverride(125)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.018f, 0.018f, 0.018f, 1))
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.FillHeight(10)
						[
							SNew(SBox)
								.WidthOverride(100)
								.HeightOverride(100)
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								[
									DraggedSlot->MakeThumbnailWidget(100).ToSharedRef()
								]
						]
					+ SVerticalBox::Slot()
						.FillHeight(2.5)
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
								.Text(DraggedSlot->GetAssetName())
								.ColorAndOpacity(FLinearColor::White)
								.ShadowColorAndOpacity(FLinearColor::Black)
								.ShadowOffset(FVector2D(1.f, 1.f))
								.Font(FAppStyle::Get().GetFontStyle("PropertyWindow.NormalFont"))
						]
				]
		];
}

void FSlotDragOperation::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	if (DraggedWidget.IsValid())
	{
		DraggedWidget.Pin()->SetVisibility(EVisibility::Visible);
		DraggedWidget.Pin()->SetSlotState(0);
	}
}
