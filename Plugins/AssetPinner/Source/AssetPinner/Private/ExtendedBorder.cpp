#include "ExtendedBorder.h"
#include "ExtendedSlateBorder.h"
#include "Components/BorderSlot.h"
#include "PinnedAssetSlotBase.h"

UExtendedBorder::UExtendedBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{

}


TSharedRef<SWidget> UExtendedBorder::RebuildWidget()
{
	MyBorder = SNew(SExtendedSlateBorder)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection)
		.AssetPath(Path);

	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}
