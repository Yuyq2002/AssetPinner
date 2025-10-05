// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ASSETPINNER_API PinAssetAction
{
public:
	static void PinAssets(const TArray<FAssetData>& SelectedAssets);

private:
	//void GetSelectedAssets(TArray<FAssetData>& SelectedAssets);
};
