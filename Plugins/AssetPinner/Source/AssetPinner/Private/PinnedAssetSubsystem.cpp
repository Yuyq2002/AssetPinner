// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSubsystem.h"
#include "Misc/PackageName.h"

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

void UPinnedAssetSubsystem::RemoveAssetPath(FString Path)
{
	if (AssetPathList.Contains(Path))
	{
		AssetPathList.Remove(Path);

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

	FilePath = FPaths::GameUserDeveloperDir() + "PinnedAssetData.txt";

	FFileHelper::LoadFileToStringArray(AssetPathList, *FilePath);

	TArray<FString> PendingRemove;
	for (auto& Path : AssetPathList)
	{
		FPackagePath OutPath;
		FPackagePath PackagePath;
		if (FPackagePath::TryFromPackageName(Path, PackagePath))
		{
			if (!FPackageName::DoesPackageExist(PackagePath, &OutPath))
			{
				PendingRemove.Add(Path);

				UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *Path);
			}
		}
	}

	AssetPathList.RemoveAll([PendingRemove](FString Candidate) {return PendingRemove.Contains(Candidate); });
}
