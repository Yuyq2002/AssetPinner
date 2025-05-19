// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "PinAssetAction.generated.h"

/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:
	UPinAssetAction();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void PinAssets();

private:
	void GetSelectedAssets(TArray<FAssetData>& SelectedAssets);
};
