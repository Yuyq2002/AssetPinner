// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Structs.h"
#include "PinnedAssetSubsystem.generated.h"

/**
 * 
 */                                                  
UCLASS()
class ASSETPINNER_API UPinnedAssetSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FAssetPathListChangedSignature, const TArray<FPinnedAssetData>&, DataList);

	FString FilePath;
	TArray<FPinnedAssetData> AssetDataList;

public:
	FAssetPathListChangedSignature OnListChangedDelegate;

	void AddAssetPath(FString Path, EPathType Type = EPathType::Asset, bool IsPinned = true);
	void RemoveAssetPath(FString Path);
	void MoveAssetPath(FString Path, int TabIndex);
	void ClearRecent();

	bool GetStatus(FString Path);
	EPathType GetPathType(FString Path);
	const TArray<FPinnedAssetData>& GetAssetDataList();

	bool ContainsPath(FString Path);
	bool FindPath(FString Path, int& OutIndex);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnAssetEditorOpen(UObject* Asset);

	UPROPERTY()
	UTexture2D* FolderIcon;
};
