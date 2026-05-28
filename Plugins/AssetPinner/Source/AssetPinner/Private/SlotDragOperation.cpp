#include "SlotDragOperation.h"
#include "SPinnedSlot.h"

TSharedRef<FSlotDragOperation> FSlotDragOperation::New(FAssetData InAssetData, UActorFactory* ActorFactory)
{
	TSharedPtr<FSlotDragOperation> Operation = MakeShared<FSlotDragOperation>();
	Operation->Init(TArray<FAssetData>{InAssetData}, TArray<FString>(), ActorFactory);
	return Operation.ToSharedRef();
}

TSharedPtr<SWidget> FSlotDragOperation::GetDefaultDecorator() const
{
	if(DraggedWidget.Pin())
	{
		return DraggedWidget.Pin();
	}

	return nullptr;
}
