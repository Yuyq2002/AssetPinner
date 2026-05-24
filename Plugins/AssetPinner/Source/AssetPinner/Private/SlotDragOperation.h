#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "SlotDragOperation.generated.h"

class UPinnedAssetSlotBase;

UCLASS()
class ASSETPINNER_API USlotDragOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UPinnedAssetSlotBase* DraggedWidget;

	UPROPERTY()
	FVector2D DragOffset;

	UPROPERTY()
	UWidget* OriginalParent;
};