#pragma once

#include "CoreMinimal.h"
#include <DragAndDrop/AssetDragDropOp.h>
#include "SlotDragOperation.generated.h"

class SPinnedSlot;

class ASSETPINNER_API FSlotDragOperation : public FAssetDragDropOp
{
public:
	TWeakPtr<SPinnedSlot> DraggedWidget;

	FVector2D DragOffset;

	TWeakPtr<SWidget> OriginalParent;
};