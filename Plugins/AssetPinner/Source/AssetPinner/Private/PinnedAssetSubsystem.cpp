// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSubsystem.h"
#include "Misc/PackageName.h"

void UPinnedAssetSubsystem::AddAssetPath(FString Path, bool IsPinned)
{
	bool SaveData = false;
	if (!AssetPathList.Contains(Path))
	{
		AssetPathList.Add(Path);
		StatusList.Add(IsPinned);

		SaveData = true;

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList, StatusList);
	}
	else
	{
		int Index = -1;
		if (AssetPathList.Find(Path, Index))
		{
			if (StatusList[Index] || !IsPinned)
				return;

			StatusList[Index] = true;

			SaveData = true;

			if (OnListChangedDelegate.IsBound())
				OnListChangedDelegate.Execute(AssetPathList, StatusList);
		}
	}

	if (SaveData)
	{
		TArray<FString> SaveList;
		for (int i = 0; i < AssetPathList.Num(); i++)
		{
			SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0'));
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);
	}
}

void UPinnedAssetSubsystem::RemoveAssetPath(FString Path)
{
	int Index = -1;
	if (AssetPathList.Find(Path, Index))
	{
		AssetPathList.RemoveAt(Index);
		StatusList.RemoveAt(Index);

		TArray<FString> SaveList;
		for (int i = 0; i < AssetPathList.Num(); i++)
		{
			SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0'));
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList, StatusList);
	}
}

void UPinnedAssetSubsystem::MoveAssetPath(FString Path)
{
	int Index = -1;
	if (AssetPathList.Find(Path, Index))
	{
		if (StatusList[Index])
			return;

		StatusList[Index] = true;

		TArray<FString> SaveList;
		for (int i = 0; i < AssetPathList.Num(); i++)
		{
			SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0'));
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList, StatusList);
	}
}

void UPinnedAssetSubsystem::ClearRecent()
{
	for (int i = 0; i < AssetPathList.Num(); i++)
	{
		if (!StatusList[i])
		{
			AssetPathList.RemoveAt(i);
			StatusList.RemoveAt(i);
			i--;
		}
	}

	TArray<FString> SaveList;
	for (int i = 0; i < AssetPathList.Num(); i++)
	{
		SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0'));
	}
	FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

	if (OnListChangedDelegate.IsBound())
		OnListChangedDelegate.Execute(AssetPathList, StatusList);
}

const TArray<FString>& UPinnedAssetSubsystem::GetAssetPathList()
{
	return AssetPathList;
}

const TArray<bool>& UPinnedAssetSubsystem::GetStatusList()
{
	return StatusList;
}

void UPinnedAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FilePath = FPaths::GameUserDeveloperDir() + "PinnedAssetData.txt";
	TArray<FString> LoadList;
	FFileHelper::LoadFileToStringArray(LoadList, *FilePath);
	bool IsPinned = false;

	for (auto& line : LoadList)
	{
		FString Path, Status;
		line.Split(" ", &Path, &Status);
		AssetPathList.Add(Path);
		StatusList.Add(Status == "1");
	}

	for (int i = 0; i < AssetPathList.Num(); i++)
	{
		FPackagePath OutPath;
		FPackagePath PackagePath;
		if (FPackagePath::TryFromPackageName(AssetPathList[i], PackagePath))
		{
			if (!FPackageName::DoesPackageExist(PackagePath, &OutPath))
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *AssetPathList[i]);
				AssetPathList.RemoveAt(i);
				StatusList.RemoveAt(i);
				i--;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *AssetPathList[i]);
			AssetPathList.RemoveAt(i);
			StatusList.RemoveAt(i);
			i--;
		}
	}

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr;
	if (!AssetEditorSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Bind Event Failed - Asset Editor Subsystem is not valid"));
		return;
	}

	AssetEditorSubsystem->OnAssetEditorOpened().AddUObject(this, &UPinnedAssetSubsystem::OnAssetEditorOpen);
}

void UPinnedAssetSubsystem::OnAssetEditorOpen(UObject* Asset)
{
	FAssetData AssetData(Asset);

	AddAssetPath(AssetData.PackageName.ToString(), false);
}