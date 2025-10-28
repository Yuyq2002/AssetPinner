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

	FPinnedAssetData(FString InAssetPath, bool InPinnedStatus, EPathType InPathType)
	{
		AssetPath = InAssetPath;
		PinnedStatus = InPinnedStatus;
		PathType = InPathType;
	}

	FPinnedAssetData(FString InAssetPath, bool InPinnedStatus, EPathType InPathType, FString InAlternativeName) : FPinnedAssetData(InAssetPath, InPinnedStatus, InPathType)
	{
		AlternativeName = InAlternativeName;
	}

	FString GetSaveString() const { return AssetPath + ' ' + (PinnedStatus ? '1' : '0') + ' ' + FString::FromInt((int)PathType) + ' ' + AlternativeName; };

	FString AssetPath;
	FString AlternativeName;
	bool PinnedStatus;
	EPathType PathType;
};

USTRUCT()
struct FSection
{
	GENERATED_BODY()

	FString Name;

	UPROPERTY()
	UPinnedSectionBase* SectionWidget;

	FSection() = default;

	FSection(FString InName, UPinnedSectionBase* InSectionWidget)
	{
		Name = InName;
		SectionWidget = InSectionWidget;
	}
};