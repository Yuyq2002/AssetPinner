#pragma once

#include "CoreMinimal.h"
#include <DragAndDrop/AssetDragDropOp.h>

class SPinnedSlot;
class SPinnedSection;
class UActorFactory;

class ASSETPINNER_API FSlotDragOperation : public FAssetDragDropOp
{
public:
	static TSharedRef<FSlotDragOperation> New(FAssetData InAssetData, UActorFactory* ActorFactory, TWeakPtr<SPinnedSlot> InDraggedWidget, TWeakPtr<SPinnedSection> InOriginalParent);

	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

	virtual void OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent) override;

	TWeakPtr<SPinnedSlot> DraggedWidget;

	FVector2D DragOffset;

	TWeakPtr<SPinnedSection> OriginalParent;
};