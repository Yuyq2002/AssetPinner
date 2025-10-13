#include "ExtendedBorder.h"
#include "ExtendedSlateBorder.h"
#include "Components/BorderSlot.h"

TSharedRef<SWidget> ExtendedBorder::RebuildWidget()
{
	MyBorder = SNew(SExtendedSlateBorder)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection);

	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}
