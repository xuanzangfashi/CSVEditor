// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SplineEditorEdModeToolkit.h"
#include "SplineEditorEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "./Editor/ComponentVisualizers/Public/SplineComponentVisualizer.h"
#include "./Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#include "./Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "JsonPointFactory.h"
#include "JsonPoints.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Json.h"
#include "Misc/MessageDialog.h"
#include "Factories/CurveFactory.h"
#include "EditorModeManager.h"
#include "AssetToolsModule.h"
#include "Runtime/Engine/Classes/Curves/CurveVector.h"


#define LOCTEXT_NAMESPACE "FSplineEditorEdModeToolkit"
#pragma optimize("",off)
TSharedPtr<SEditableTextBox> FSplineEditorEdModeToolkit::NumberInput;

FSplineEditorEdModeToolkit::FSplineEditorEdModeToolkit()
{
}
//#pragma optimize("",off)

void FSplineEditorEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return GEditor->GetSelectedActors()->Num() != 0;
		}

		static FReply OnButtonClick(FVector InOffset)
		{
			USelection* SelectedActors = GEditor->GetSelectedActors();

			// Let editor know that we're about to do something that we want to undo/redo
			GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

			// For each selected actor
			for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
			{
				if (AActor* LevelActor = Cast<AActor>(*Iter))
				{
					// Register actor in opened transaction (undo/redo)
					LevelActor->Modify();
					// Move actor to given location
					LevelActor->TeleportTo(LevelActor->GetActorLocation() + InOffset, FRotator(0, 0, 0));
				}
			}

			// We're done moving actors so close transaction
			GEditor->EndTransaction();

			return FReply::Handled();
		}


		static TSharedRef<SWidget> MakeButton(FText InLabel, const FVector InOffset)
		{
			return SNew(SButton)
				.Text(InLabel)
				.OnClicked_Static(&Locals::OnButtonClick, InOffset);
		}
	};

	const float Factor = 256.0f;

	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(50)
		[
			SNew(STextBlock)
			.AutoWrapText(true)
		.Text(LOCTEXT("HelperLabel", "1.Select the single spline component \r\n 2.select two spline points \r\n 3.input the number of point that you want to generate \r\n 4. click the generate button!"))
		]
	//+ SVerticalBox::Slot()
	//	.HAlign(HAlign_Center)
	//	.AutoHeight()
	//	[
	//		Locals::MakeButton(LOCTEXT("UpButtonLabel", "None"), FVector(0, 0, Factor))
	//	]
	+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			//+ SHorizontalBox::Slot()
			//.AutoWidth()
			//[
			//	Locals::MakeButton(LOCTEXT("LeftButtonLabel", "None"), FVector(0, -Factor, 0))
			//]
		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(NumberInput, SEditableTextBox)
			.Text(LOCTEXT("NumberInput", "NumberInput"))

		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("Generate Points", "Generate Points"))
		.OnClicked_Static(&FSplineEditorEdModeToolkit::OnGenerateButtonClick)
		]

		]
	//+ SVerticalBox::Slot()
	//	.HAlign(HAlign_Center)
	//	.AutoHeight()
	//	[
	//		Locals::MakeButton(LOCTEXT("DownButtonLabel", "None"), FVector(0, 0, -Factor))
	//	]
	+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(LOCTEXT("Import json file(s)", "Import json file(s)"))
		.OnClicked_Static(&FSplineEditorEdModeToolkit::ImportJsonFiles)
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(LOCTEXT("Write spline", "Write spline"))
		.OnClicked_Static(&FSplineEditorEdModeToolkit::WriteCurrentPoints2CurrentSelectedSplineCom)
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(LOCTEXT("Export points", "Export points"))
		.OnClicked_Static(&FSplineEditorEdModeToolkit::ExportFile)
		]

		];

	FModeToolkit::Init(InitToolkitHost);
}

//#pragma optimize("",on)

FName FSplineEditorEdModeToolkit::GetToolkitFName() const
{
	return FName("SplineEditorEdMode");
}

FText FSplineEditorEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("SplineEditorEdModeToolkit", "DisplayName", "SplineEditorEdMode Tool");
}

class FEdMode* FSplineEditorEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FSplineEditorEdMode::EM_SplineEditorEdModeId);
}

FReply FSplineEditorEdModeToolkit::OnGenerateButtonClick()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();
	TSharedPtr<FSplineComponentVisualizer> visualizer = StaticCastSharedPtr<FSplineComponentVisualizer>(GUnrealEd->FindComponentVisualizer(USplineComponent::StaticClass()));
	TArray<int32> keys = visualizer->GetSelectedKeys().Array();
	USplineComponent* spline = NULL;
	//if (GEditor->GetSelectedComponents()->Num() > 0)
	spline = visualizer->GetEditedSplineComponent();
	if (spline && keys.Num() == 2)
	{
		float pointA_distance = spline->GetDistanceAlongSplineAtSplinePoint(keys[0]);
		float pointB_distance = spline->GetDistanceAlongSplineAtSplinePoint(keys[1]);

		FString number_in_str = NumberInput->GetText().ToString();
		int number = UKismetStringLibrary::Conv_StringToInt(number_in_str);
		float delta_length = FMath::Abs((pointA_distance - pointB_distance) / (float)(number + 1));
		TArray<FVector> locations;
		
		int big_index = 0;
		int small_index = 0;
		//keys[0] > keys[1] ? big_index = 0, small_index = 1 : big_index = 1, small_index = 0;
		if (keys[0] > keys[1])
		{
			big_index = 0;
			small_index = 1;
		}
		else
		{
			small_index = 0;
			big_index = 1;
		}

		float distance_forward = spline->GetDistanceAlongSplineAtSplinePoint(keys[small_index]);
		for (int i = 0; i < number; i++)
		{
			distance_forward += delta_length;
			locations.Add(spline->GetLocationAtDistanceAlongSpline(distance_forward, ESplineCoordinateSpace::World));
		}

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::FromInt(small_index) + "," + FString::FromInt(big_index));
		int tmp = keys[big_index];
		int delete_key_index = keys[small_index] + 1;
		spline->Modify();
		while (delete_key_index < tmp)
		{
			spline->RemoveSplinePoint(delete_key_index);
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::FromInt(spline->GetNumberOfSplinePoints()));
			tmp--;

		}
		spline->UpdateSpline();
		for (int i = 0; i < number; i++)
		{
			spline->AddSplinePointAtIndex(locations[i], keys[small_index] + i + 1, ESplineCoordinateSpace::World);
			spline->SetTangentAtSplinePoint(keys[small_index] + 1 + i, FVector::ZeroVector, ESplineCoordinateSpace::World);
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::FromInt(spline->GetNumberOfSplinePoints()));

		}
		visualizer->EndEditing();

	}

	return FReply::Handled();
}

FReply FSplineEditorEdModeToolkit::ImportJsonFiles()
{
	TArray<FString> outNames;
	OpenFileDialog("Import json", "C:/", "*.json", outNames);
	//TSharedPtr<UJsonPointFactory> jsonPoints_factory = MakeShareable(NewObject<UJsonPointFactory>());
	//TSharedPtr<UCurveFloatFactory> curve_factory = MakeShareable(NewObject<UCurveFloatFactory>());
	//FString package_name = TEXT("/Game/JsonFiles/");
	//TSharedPtr<UPackage> package = MakeShareable(CreatePackage(NULL, *package_name));
	TArray<FAssetData>allSelectAsset;
	GEditor->GetContentBrowserSelections(allSelectAsset);
	UCurveVectorFactory* cureve_factory = NewObject<UCurveVectorFactory>();
	FAssetToolsModule& assetToolsModule = FAssetToolsModule::GetModule();
	UObject* newAsset = assetToolsModule.Get().CreateAsset(cureve_factory->GetSupportedClass(), cureve_factory);
	TArray<UObject*>ObjectsToSync;
	ObjectsToSync.Add(newAsset);
	GEditor->SyncBrowserToObjects(ObjectsToSync);
	UCurveVector* curve_asset = (UCurveVector*)newAsset;
	for (int i = 0; i < outNames.Num(); i++)
	{
		FString file_name = FPaths::GetCleanFilename(outNames[i]);
		//(UJsonPoints*)jsonPoints_factory->FactoryCreateNew(UJsonPoints::StaticClass(), package.Get(), *file_name, RF_Standalone | RF_Public, NULL, GWarn);
		UJsonPoints* JsonPoints = Cast<UJsonPoints>(allSelectAsset[i].GetAsset());


		

		if (JsonPoints != NULL)
		{

			FString json_data;
			FFileHelper::LoadFileToString(json_data, *outNames[i]);
			TSharedPtr<FJsonObject> json_obj;
			TSharedRef<TJsonReader<TCHAR>> json_reader = TJsonReaderFactory<TCHAR>::Create(json_data);
			if (FJsonSerializer::Deserialize(json_reader, json_obj))
			{
				auto _array = json_obj->GetArrayField("points");

				FScopedSlowTask sst(_array.Num(), LOCTEXT("importing......", "importing....."));
				sst.MakeDialog(true);

				for (int j = 0; j < _array.Num(); j++)
				{
					TSharedPtr<FJsonObject> obj = _array[j]->AsObject();
					float t = obj->GetNumberField("t");
					float x = obj->GetNumberField("x");
					float y = obj->GetNumberField("y");
					float orientation = obj->GetNumberField("orientation");
					curve_asset->FloatCurves[0].AddKey(t, x);
					curve_asset->FloatCurves[1].AddKey(t, y);
					curve_asset->FloatCurves[2].AddKey(t, orientation);
					FJsonPoint point;
					point.t = t;
					point.x = x;
					point.y = y;
					point.orientation = orientation;

					JsonPoints->Points.Add(point);
					sst.EnterProgressFrame();
				}
				
			}
			//FString asset_name = "new_json_points_" + FGuid::NewGuid().ToString();
			//JsonPoints->Rename(*asset_name);
			//JsonPoints->PostRename()
			//FAssetRegistryModule::AssetCreated(JsonPoints)
			//package->SetDirtyFlag(true);;
			
			JsonPoints->Rename(*FPaths::GetBaseFilename(outNames[i]));
			curve_asset->Rename(*(FPaths::GetBaseFilename(outNames[i]) + "_Curve"));
			JsonPoints->MarkPackageDirty();
			JsonPoints->PostEditChange();
			JsonPoints->AddToRoot();
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("you did not select a vaild JsonPoints asset!"));
		}
		//curve_factory = NULL;
		//jsonPoints_factory = NULL;
		//package_name = NULL;
	}

	return FReply::Handled();
}

FReply FSplineEditorEdModeToolkit::WriteCurrentPoints2CurrentSelectedSplineCom()
{
	//TSharedPtr<FSplineComponentVisualizer> visualizer = StaticCastSharedPtr<FSplineComponentVisualizer>(GUnrealEd->FindComponentVisualizer(USplineComponent::StaticClass()));
	//TArray<int32> keys = visualizer->GetSelectedKeys().Array();
	USplineComponent* spline = NULL;
	//if (GEditor->GetSelectedComponents()->Num() > 0)
	//spline = visualizer->GetEditedSplineComponent();
	spline = Cast<USplineComponent>(GEditor->GetSelectedComponents()->GetSelectedObject(0));
	TArray<FAssetData>allSelectAsset;
	GEditor->GetContentBrowserSelections(allSelectAsset);

	//selection->GetSelectedObjects(UJsonPoints::StaticClass(), outer);
	UJsonPoints* jPoint = Cast<UJsonPoints>(allSelectAsset[0].GetAsset());
	if (spline)
	{
		if (jPoint != NULL)
		{
			spline->ClearSplinePoints();
			for (int i = 0; i < jPoint->Points.Num(); i++)
			{
				spline->AddSplinePointAtIndex(FVector(jPoint->Points[i].x, 0, jPoint->Points[i].y), i, ESplineCoordinateSpace::Local);
			}

			//visualizer->EndEditing();
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("you did not select a vaild JsonPoints asset!"));
			//MessageBoxA(NULL, , "a", 1);
		}
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("you did not select a vaild Spline component!"));

	}
	return FReply::Handled();

}

FReply FSplineEditorEdModeToolkit::ExportFile()
{
	FJsonSerializableArray OutName;
	OpenSaveFileDialog("save files", "C:/", OutName);
	if (OutName.GetData())
	{
		FString ex = FPaths::GetExtension(OutName[0]);
		USelection* selection = GEditor->GetSelectedComponents();
		USplineComponent* spline = NULL;
		if (selection->Num() > 0)
		{
			spline = (USplineComponent*)selection->GetSelectedObject(0);
		}
		if (spline)
		{
			if (ex == "csv")
			{
				FString data = "t,x,y,orientation\r\n";
				for (int i = 0; i < spline->GetNumberOfSplinePoints(); i++)
				{
					FVector location = spline->GetLocationAtSplineInputKey(i, ESplineCoordinateSpace::Local);
					data = data + "0" + "," + UKismetStringLibrary::Conv_FloatToString(location.X) + ","
						+ UKismetStringLibrary::Conv_FloatToString(location.Z) + ","
						+ "0" + "\r\n";
				}
				FFileHelper::SaveStringToFile(data, *OutName[0]);
			}
			else if (ex == "json")
			{
				FString data = "";
				TSharedPtr<FJsonObject> jobj = MakeShareable(new FJsonObject);
				TArray <TSharedPtr<FJsonValue>>array_values;
				for (int i = 0; i < spline->GetNumberOfSplinePoints(); i++)
				{
					TSharedPtr<FJsonObject> tmp_j = MakeShareable(new FJsonObject);
					FVector location = spline->GetLocationAtSplineInputKey(i, ESplineCoordinateSpace::Local);
					tmp_j->SetNumberField("t", 0.0);
					tmp_j->SetNumberField("x", location.X);
					tmp_j->SetNumberField("y", location.Z);
					tmp_j->SetNumberField("orientation", 0.0);
					TSharedPtr<FJsonValueObject> json_value = MakeShareable(new FJsonValueObject(tmp_j));

					array_values.Add(json_value);
				}
				jobj->SetArrayField("points", array_values);
				FString outJsonStr;
				TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&outJsonStr);
				FJsonSerializer::Serialize(jobj.ToSharedRef(), writer);
				FFileHelper::SaveStringToFile(outJsonStr, *OutName[0]);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("you did not select a vaild Spline component!"));
		}
	}
	return FReply::Handled();
}

void FSplineEditorEdModeToolkit::OpenFileDialog(const FString & dialogTitle, const FString & defaultPath, const FString & fileTypes, TArray<FString>& outFileNames)
{
	if (GEngine)
	{
		//if (GEngine->GameViewport)
		{
			//void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				//Opening the file picker!
				uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
				DesktopPlatform->OpenFileDialog(0, dialogTitle, defaultPath, FString(""), fileTypes, SelectionFlag, outFileNames);

			}
		}
	}
}

void FSplineEditorEdModeToolkit::OpenSaveFileDialog(const FString& dialogTitle, const FString& defaultPath, TArray<FString>& OutNames)
{
	if (GEngine)
	{
		//if (GEngine->GameViewport)
		{
			//void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				//Opening the file picker!
				uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
				//DesktopPlatform->OpenDirectoryDialog(0, dialogTitle, defaultPath, outFileName);
				DesktopPlatform->SaveFileDialog(0, dialogTitle, defaultPath, FString(""), FString("json file|*.json|csv file|*.csv"), EFileDialogFlags::None, OutNames);

			}
		}
	}
}

#pragma optimize("",on)

#undef LOCTEXT_NAMESPACE
