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
	
	DECLARE_DELEGATE_OneParam(FAssetPathListChangedSignature, const TArray<FPinnedAssetData>&);

	FString FilePath;
	TArray<FPinnedAssetData> AssetDataList;

	UPROPERTY()
	TArray<FString> Tabs;

	float SlotSize = 100.f;

public:
	FAssetPathListChangedSignature OnListChangedDelegate;

	void AddAssetPath(FString Path, EPathType Type = EPathType::Asset, bool IsPinned = true);
	void AddAssetPath(FString Path, EPathType Type, int TabIndex);
	void RemoveAssetPath(FString Path);
	void MoveAssetPath(FString Path, int TabIndex);
	bool MoveAssetPath(FString Path, FString TabName);
	void ClearRecent();

	void SetTabs(const TArray<FSection>& InTabs);
	void AddTabNames(FString Name);
	void RemoveTab(int Index);
	void RenameTab(FString Name, int Index);
	void EmptyTabName();
	TArray<FString> GetTabNames();
	int GetTabIndex(FString TabName);

	bool GetStatus(FString Path);
	EPathType GetPathType(FString Path);
	const TArray<FPinnedAssetData>& GetAssetDataList();

	bool ContainsEntry(FString Path, int TabIndex);
	bool FindPath(FString Path, int& OutIndex);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnAssetEditorOpen(UObject* Asset);

	// Move to style class later
	TSharedPtr<FSlateImageBrush> FolderIconBrush;
	TSharedPtr<FSlateImageBrush> AddIconBrush;
	FButtonStyle AddButtonStyle;
};
