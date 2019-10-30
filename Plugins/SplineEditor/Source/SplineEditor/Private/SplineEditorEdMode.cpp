// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SplineEditorEdMode.h"
#include "SplineEditorEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FSplineEditorEdMode::EM_SplineEditorEdModeId = TEXT("EM_SplineEditorEdMode");

FSplineEditorEdMode::FSplineEditorEdMode()
{

}

FSplineEditorEdMode::~FSplineEditorEdMode()
{

}

void FSplineEditorEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FSplineEditorEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FSplineEditorEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FSplineEditorEdMode::UsesToolkits() const
{
	return true;
}




