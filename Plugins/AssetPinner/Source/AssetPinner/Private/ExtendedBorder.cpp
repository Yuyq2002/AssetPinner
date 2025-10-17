#include "ExtendedBorder.h"
#include "ExtendedSlateBorder.h"
#include "Components/BorderSlot.h"

UExtendedBorder::UExtendedBorder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{

}


TSharedRef<SWidget> UExtendedBorder::RebuildWidget()
{
	MyBorder = SNew(SExtendedSlateBorder)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection);

	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}
