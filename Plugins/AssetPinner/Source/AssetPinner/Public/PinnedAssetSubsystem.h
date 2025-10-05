// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "PinnedAssetSubsystem.generated.h"

/**
 * 
 */                                                  
UCLASS()
class ASSETPINNER_API UPinnedAssetSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FAssetPathListChangedSignature, const TArray<FString>&, AssetList, const TArray<bool>&, StatusList);

	FString FilePath;
	TArray<FString> AssetPathList;
	TArray<bool> StatusList;

public:
	FAssetPathListChangedSignature OnListChangedDelegate;

	void AddAssetPath(FString Path, bool IsPinned = true);
	void RemoveAssetPath(FString Path);
	void MoveAssetPath(FString Path);
	void ClearRecent();
	const TArray<FString>& GetAssetPathList();
	const TArray<bool>& GetStatusList();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnAssetEditorOpen(UObject* Asset);
};
