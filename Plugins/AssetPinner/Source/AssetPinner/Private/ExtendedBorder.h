#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "ExtendedBorder.generated.h"

UCLASS(MinimalAPI)
class UExtendedBorder : public UBorder
{
	GENERATED_UCLASS_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	FString Path;
};

