#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"

class ExtendedBorder : public UBorder
{
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};

