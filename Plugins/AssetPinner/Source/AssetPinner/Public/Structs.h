// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Structs.generated.h"

class UPinnedSectionBase;

/**
 * 
 */
USTRUCT()
struct ASSETPINNER_API FPinnedAssetData
{
	GENERATED_BODY()

	FPinnedAssetData() = default;

	FPinnedAssetData(FString InAssetPath, int InTabIndex, EPathType InPathType)
	{
		AssetPath = InAssetPath;
		TabIndex = InTabIndex;
		PathType = InPathType;
	}

	FString GetSaveString() const { return AssetPath + ' ' + FString::FromInt(TabIndex) + ' ' + FString::FromInt((int)PathType); };

	FString AssetPath;
	int TabIndex;
	EPathType PathType;
};

USTRUCT()
struct FSection
{
	GENERATED_BODY()

	FString Name;

	UPROPERTY()
	UPinnedSectionBase* SectionWidget = nullptr;

	UPROPERTY()
	bool bIsPersistent = false;

	FSection() = default;

	FSection(FString InName, UPinnedSectionBase* InSectionWidget, bool InIsPersistent = false)
	{
		Name = InName;
		SectionWidget = InSectionWidget;
		bIsPersistent = InIsPersistent;
	}
};