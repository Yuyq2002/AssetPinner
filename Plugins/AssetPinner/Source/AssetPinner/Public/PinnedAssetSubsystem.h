// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Enums.h"
#include "PinnedAssetSubsystem.generated.h"

/**
 * 
 */                                                  
UCLASS()
class ASSETPINNER_API UPinnedAssetSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FAssetPathListChangedSignature, const TArray<FString>&, AssetList, const TArray<bool>&, StatusList, const TArray<EPathType>&, PathTypes);

	FString FilePath;
	TArray<FString> AssetPathList;
	TArray<bool> StatusList;
	TArray<EPathType> PathTypes;

public:
	FAssetPathListChangedSignature OnListChangedDelegate;

	void AddAssetPath(FString Path, EPathType Type = EPathType::Asset, bool IsPinned = true);
	void RemoveAssetPath(FString Path);
	void MoveAssetPath(FString Path);
	void ClearRecent();
	bool GetStatus(FString Path);
	EPathType GetPathType(FString Path);
	const TArray<FString>& GetAssetPathList();
	const TArray<bool>& GetStatusList();
	const TArray<EPathType>& GetPathTypes();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnAssetEditorOpen(UObject* Asset);

	UPROPERTY()
	UTexture2D* FolderIcon;
};
