// Fill out your copyright notice in the Description page of Project Settings.
#include "JsonPointFactory.h"
#include "JsonPoints.h"
#include "AssetTypeCategories.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"


UJsonPointFactory::UJsonPointFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	//Configure the class that this factory creates
	SupportedClass = UJsonPoints::StaticClass();
}

uint32 UJsonPointFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Blueprint;
}

UObject* UJsonPointFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UJsonPoints* JsonPoints = NewObject<UJsonPoints>(InParent, InClass, InName, Flags);
	return JsonPoints;
}

//UObject* UJsonPointFactory::ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
//{
//	UObject* Result = nullptr;
//	CurrentFilename = Filename;
//
//
//	// sanity check the file size of the impending import and prompt
//	// the user if they wish to continue if the file size is very large
//	const int64 FileSize = IFileManager::Get().FileSize(*CurrentFilename);
//
//	//@third party BEGIN SIMPLYGON
//	// Hack do not hash files that are huge.  They take forever anway
//	const int32 Gigabyte = 1024 * 1024 * 1024;
//	if (FileSize < Gigabyte)
//	{
//		FileHash = FMD5Hash::HashFile(*CurrentFilename);
//	}
//
//	//@third party END SIMPLYGON
//
//
//	if (CanCreateNew())
//	{
//		UE_LOG(LogFactory, Log, TEXT("FactoryCreateNew: %s with %s (%i %i %s)"), *InClass->GetName(), *GetClass()->GetName(), bCreateNew, bText, *Filename);
//		ParseParms(Parms);
//
//		Result = FactoryCreateNew(InClass, InOuter, InName, InFlags, nullptr, GWarn);
//	}
//	else if (!Filename.IsEmpty())
//	{
//		if (FileSize == INDEX_NONE)
//		{
//			UE_LOG(LogFactory, Error, TEXT("Can't find file '%s' for import"), *Filename);
//		}
//		else
//		{
//			UE_LOG(LogFactory, Log, TEXT("FactoryCreateFile: %s with %s (%i %i %s)"), *InClass->GetName(), *GetClass()->GetName(), bCreateNew, bText, *Filename);
//
//			Result = FactoryCreateFile(InClass, InOuter, InName, InFlags, *Filename, Parms, GWarn, OutCanceled);
//		}
//	}
//
//	if (Result != nullptr)
//	{
//		Result->MarkPackageDirty();
//		ULevel::LevelDirtiedEvent.Broadcast();
//		Result->PostEditChange();
//	}
//
//	CurrentFilename = TEXT("");
//
//	return Result;
//}
