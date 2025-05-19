// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSubsystem.h"

void UPinnedAssetSubsystem::AddAssetPath(FString Path)
{
	if (!AssetPathList.Contains(Path))
	{
		AssetPathList.Add(Path);

		FFileHelper::SaveStringArrayToFile(AssetPathList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList);
	} 
}

const TArray<FString>& UPinnedAssetSubsystem::GetAssetPathList()
{
	return AssetPathList;
}

void UPinnedAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FilePath = FPaths::ProjectConfigDir() + "PinnedAssetData.txt";

	FFileHelper::LoadFileToStringArray(AssetPathList, *FilePath);
}
