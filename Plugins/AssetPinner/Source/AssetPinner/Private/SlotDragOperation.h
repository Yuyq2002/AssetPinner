#pragma once

#include "CoreMinimal.h"
#include <DragAndDrop/AssetDragDropOp.h>

class SPinnedSlot;
class SPinnedSection;
class UActorFactory;

class ASSETPINNER_API FSlotDragOperation : public FAssetDragDropOp
{
public:
	static TSharedRef<FSlotDragOperation> New(FAssetData InAssetData, UActorFactory* ActorFactory);

	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;


	TWeakPtr<SPinnedSlot> DraggedWidget;

	FVector2D DragOffset;

	TWeakPtr<SPinnedSection> OriginalParent;
};