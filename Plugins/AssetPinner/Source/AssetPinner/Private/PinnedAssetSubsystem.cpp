// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSubsystem.h"
#include "Misc/PackageName.h"

void UPinnedAssetSubsystem::AddAssetPath(FString Path, EPathType Type, bool IsPinned)
{
	bool SaveData = false;
	if (!AssetPathList.Contains(Path))
	{
		AssetPathList.Add(Path);
		StatusList.Add(IsPinned);
		PathTypes.Add(Type);

		SaveData = true;

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList, StatusList, PathTypes);
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
				OnListChangedDelegate.Execute(AssetPathList, StatusList, PathTypes);
		}
	}

	if (SaveData)
	{
		TArray<FString> SaveList;
		for (int i = 0; i < AssetPathList.Num(); i++)
		{
			SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0') + ' ' + FString::FromInt((int)PathTypes[i]));
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
		PathTypes.RemoveAt(Index);

		TArray<FString> SaveList;
		for (int i = 0; i < AssetPathList.Num(); i++)
		{
			SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0') + ' ' + FString::FromInt((int)PathTypes[i]));
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList, StatusList, PathTypes);
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
			SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0') + ' ' + FString::FromInt((int)PathTypes[i]));
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetPathList, StatusList, PathTypes);
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
			PathTypes.RemoveAt(i);
			i--;
		}
	}

	TArray<FString> SaveList;
	for (int i = 0; i < AssetPathList.Num(); i++)
	{
		SaveList.Add(AssetPathList[i] + ' ' + (StatusList[i] ? '1' : '0') + ' ' + FString::FromInt((int)PathTypes[i]));
	}
	FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

	if (OnListChangedDelegate.IsBound())
		OnListChangedDelegate.Execute(AssetPathList, StatusList, PathTypes);
}

bool UPinnedAssetSubsystem::GetStatus(FString Path)
{
	int Index = -1;
	if (AssetPathList.Find(Path, Index))
		return StatusList[Index];

	return false;
}

EPathType UPinnedAssetSubsystem::GetPathType(FString Path)
{
	int Index = -1;
	if (AssetPathList.Find(Path, Index))
		return PathTypes[Index];

	return EPathType::None;
}

const TArray<FString>& UPinnedAssetSubsystem::GetAssetPathList()
{
	return AssetPathList;
}

const TArray<bool>& UPinnedAssetSubsystem::GetStatusList()
{
	return StatusList;
}

const TArray<EPathType>& UPinnedAssetSubsystem::GetPathTypes()
{
	return PathTypes;
}

void UPinnedAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FolderIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/AssetPinner/Folder_Base_256x.Folder_Base_256x'"));
	FilePath = FPaths::GameUserDeveloperDir() + "PinnedAssetData.txt";
	TArray<FString> LoadList;
	FFileHelper::LoadFileToStringArray(LoadList, *FilePath);
	bool IsPinned = false;

	for (auto& line : LoadList)
	{
		TArray<FString> SplitString;
		line.ParseIntoArray(SplitString, TEXT(" "));
		AssetPathList.Add(SplitString[0]);
		StatusList.Add(SplitString[1] == "1");
		PathTypes.Add(SplitString.IsValidIndex(2) ? (EPathType)FCString::Atoi(*SplitString[2]) : EPathType::Asset);
	}

	for (int i = 0; i < AssetPathList.Num(); i++)
	{
		FPackagePath OutPath;
		FPackagePath PackagePath;
		if (FPackagePath::TryFromPackageName(AssetPathList[i], PackagePath) || PathTypes[i] != EPathType::Asset)
		{
			if (!FPackageName::DoesPackageExist(PackagePath, &OutPath) && PathTypes[i] == EPathType::Asset)
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *AssetPathList[i]);
				AssetPathList.RemoveAt(i);
				StatusList.RemoveAt(i);
				PathTypes.RemoveAt(i);
				i--;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *AssetPathList[i]);
			AssetPathList.RemoveAt(i);
			StatusList.RemoveAt(i);
			PathTypes.RemoveAt(i);
			i--;
		}
	}

	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>() : nullptr;
	if (!AssetEditorSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Bind Event Failed - Asset Editor Subsystem is not valid"));
		return;
	}

	AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddUObject(this, &UPinnedAssetSubsystem::OnAssetEditorOpen);
}

void UPinnedAssetSubsystem::OnAssetEditorOpen(UObject* Asset)
{
	FAssetData AssetData(Asset);

	AddAssetPath(AssetData.PackageName.ToString(), EPathType::Asset, false);
}