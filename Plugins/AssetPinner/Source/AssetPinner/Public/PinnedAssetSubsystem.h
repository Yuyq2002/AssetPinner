// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "PinnedAssetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ASSETPINNER_API UPinnedAssetSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FAssetPathListChangedSignature, const TArray<FString>&, ChangedList);

	FString FilePath;
	TArray<FString> AssetPathList;

public:
	FAssetPathListChangedSignature OnListChangedDelegate;

	void AddAssetPath(FString Path);
	const TArray<FString>& GetAssetPathList();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
