// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSubsystem.h"
#include "Misc/PackageName.h"

void UPinnedAssetSubsystem::AddAssetPath(FString Path, EPathType Type, bool IsPinned)
{
	bool SaveData = false;
	if (!ContainsPath(Path))
	{
		AssetDataList.Add(FPinnedAssetData(Path, IsPinned, Type));

		SaveData = true;

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetDataList);
	}
	else
	{
		int Index = -1;
		if (FindPath(Path, Index))
		{
			if (AssetDataList[Index].PinnedStatus || !IsPinned)
				return;

			AssetDataList[Index].PinnedStatus = true;

			SaveData = true;

			if (OnListChangedDelegate.IsBound())
				OnListChangedDelegate.Execute(AssetDataList);
		}
	}

	if (SaveData)
	{
		TArray<FString> SaveList;
		for (const auto& Data : AssetDataList)
		{
			SaveList.Add(Data.GetSaveString());
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);
	}
}

void UPinnedAssetSubsystem::RemoveAssetPath(FString Path)
{
	int Index = -1;
	if (FindPath(Path, Index))
	{
		AssetDataList.RemoveAt(Index);

		TArray<FString> SaveList;
		for (const auto& Data : AssetDataList)
		{
			SaveList.Add(Data.GetSaveString());
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetDataList);
	}
}

void UPinnedAssetSubsystem::MoveAssetPath(FString Path)
{
	int Index = -1;
	if (FindPath(Path, Index))
	{
		if (AssetDataList[Index].PinnedStatus)
			return;

		AssetDataList[Index].PinnedStatus = true;

		TArray<FString> SaveList;
		for (const auto& Data : AssetDataList)
		{
			SaveList.Add(Data.GetSaveString());
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetDataList);
	}
}

void UPinnedAssetSubsystem::ClearRecent()
{
	AssetDataList.Empty();

	TArray<FString> SaveList;
	for (const auto& Data : AssetDataList)
	{
		SaveList.Add(Data.GetSaveString());
	}
	FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

	if (OnListChangedDelegate.IsBound())
		OnListChangedDelegate.Execute(AssetDataList);
}

void UPinnedAssetSubsystem::RenamePinnedAsset(FString Path, FString NewName)
{
	int Index = -1;
	if (FindPath(Path, Index))
	{
		AssetDataList[Index].AlternativeName = NewName;

		TArray<FString> SaveList;
		for (const auto& Data : AssetDataList)
		{
			SaveList.Add(Data.GetSaveString());
		}
		FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetDataList);
	}
}

bool UPinnedAssetSubsystem::GetStatus(FString Path)
{
	int Index = -1;
	if (FindPath(Path, Index))
		return AssetDataList[Index].PinnedStatus;

	return false;
}

EPathType UPinnedAssetSubsystem::GetPathType(FString Path)
{
	int Index = -1;
	if (FindPath(Path, Index))
		return AssetDataList[Index].PathType;

	return EPathType::None;
}

const TArray<FPinnedAssetData>& UPinnedAssetSubsystem::GetAssetDataList()
{
	return AssetDataList;
}

bool UPinnedAssetSubsystem::ContainsPath(FString Path)
{
	for (const FPinnedAssetData& Data : AssetDataList)
		if (Data.AssetPath.Equals(Path))
			return true;

	return false;
}

bool UPinnedAssetSubsystem::FindPath(FString Path, int& OutIndex)
{
	int Index = 0;
	for (const FPinnedAssetData& Data : AssetDataList)
	{
		if (Data.AssetPath.Equals(Path))
		{
			OutIndex = Index;
			return true;
		}

		Index++;
	}

	OutIndex = -1;
	return false;
}

void UPinnedAssetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FolderIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/AssetPinner/Icon/Folder_Base_256x.Folder_Base_256x'"));
	FilePath = FPaths::GameUserDeveloperDir() + "PinnedAssetData.txt";
	TArray<FString> LoadList;
	FFileHelper::LoadFileToStringArray(LoadList, *FilePath);
	bool IsPinned = false;

	for (auto& line : LoadList)
	{
		TArray<FString> SplitString;
		line.ParseIntoArray(SplitString, TEXT(" "));
		AssetDataList.Add(FPinnedAssetData(
			SplitString[0], 
			SplitString[1] == "1", 
			SplitString.IsValidIndex(2) ? (EPathType)FCString::Atoi(*SplitString[2]) : EPathType::Asset,
			SplitString.IsValidIndex(3) ? SplitString[3] : ""
		));
	}

	for (int i = 0; i < AssetDataList.Num(); i++)
	{
		FPackagePath OutPath;
		FPackagePath PackagePath;
		if (FPackagePath::TryFromPackageName(AssetDataList[i].AssetPath, PackagePath) || AssetDataList[i].PathType != EPathType::Asset)
		{
			if (!FPackageName::DoesPackageExist(PackagePath, &OutPath) && AssetDataList[i].PathType == EPathType::Asset)
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *AssetDataList[i].AssetPath);
				AssetDataList.RemoveAt(i);
				i--;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot find file: %s"), *AssetDataList[i].AssetPath);
			AssetDataList.RemoveAt(i);
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