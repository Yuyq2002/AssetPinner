// Fill out your copyright notice in the Description page of Project Settings.


#include "PinnedAssetSubsystem.h"
#include "Misc/PackageName.h"

void UPinnedAssetSubsystem::AddAssetPath(FString Path, EPathType Type, bool IsPinned)
{
	bool SaveData = false;
	if (!ContainsPath(Path))
	{
		AssetDataList.Add(FPinnedAssetData(Path, !IsPinned, Type));

		SaveData = true;

		if (OnListChangedDelegate.IsBound())
			OnListChangedDelegate.Execute(AssetDataList);
	}
	else
	{
		int Index = -1;
		if (FindPath(Path, Index))
		{
			if (AssetDataList[Index].TabIndex == 0 || !IsPinned)
				return;

			AssetDataList[Index].TabIndex = 0;

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

void UPinnedAssetSubsystem::MoveAssetPath(FString Path, int TabIndex)
{
	int Index = -1;
	if (FindPath(Path, Index))
	{
		if (AssetDataList[Index].TabIndex == TabIndex)
			return;

		AssetDataList[Index].TabIndex = TabIndex;

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

bool UPinnedAssetSubsystem::MoveAssetPath(FString Path, FString TabName)
{
	int Index = 0;
	for(auto& Tab : Tabs)
	{
		if (Tab.Equals(TabName))
		{
			MoveAssetPath(Path, Index);
			return true;
		}

		Index++;
	}

	return false;
}

void UPinnedAssetSubsystem::ClearRecent()
{
	AssetDataList.RemoveAll(
		[](FPinnedAssetData Candidate)
		{
			return Candidate.TabIndex == 1;
		}
	);

	TArray<FString> SaveList;
	for (const auto& Data : AssetDataList)
	{
		SaveList.Add(Data.GetSaveString());
	}
	FFileHelper::SaveStringArrayToFile(SaveList, *FilePath);

	if (OnListChangedDelegate.IsBound())
		OnListChangedDelegate.Execute(AssetDataList);
}

void UPinnedAssetSubsystem::SetTabs(const TArray<FSection>& InTabs)
{
	for (const auto& Tab : InTabs)
	{
		Tabs.AddUnique(Tab.Name);
	}
}

void UPinnedAssetSubsystem::AddTabNames(FString Name)
{
	Tabs.AddUnique(Name);
}

void UPinnedAssetSubsystem::RemoveTab(int Index)
{
	if (Tabs.IsValidIndex(Index))
		Tabs.RemoveAt(Index);

	bool Modified = false;
	for (auto& AssetData : AssetDataList)
	{
		if (AssetData.TabIndex == Index)
		{
			AssetData.TabIndex = 0;
			Modified = true;
		}
		else if (AssetData.TabIndex > Index)
		{
			AssetData.TabIndex--;
			Modified = true;
		}
	}

	if (Modified)
	{
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

void UPinnedAssetSubsystem::RenameTab(FString Name, int Index)
{
	if (Tabs.IsValidIndex(Index))
		Tabs[Index] = Name;
}

void UPinnedAssetSubsystem::EmptyTabName()
{
	Tabs.Empty();
}

TArray<FString> UPinnedAssetSubsystem::GetTabNames()
{
	return Tabs;
}

bool UPinnedAssetSubsystem::GetStatus(FString Path)
{
	int Index = -1;
	if (FindPath(Path, Index))
		return AssetDataList[Index].TabIndex != 1;

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
			FCString::Atoi(*SplitString[1]), 
			SplitString.IsValidIndex(2) ? (EPathType)FCString::Atoi(*SplitString[2]) : EPathType::Asset
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