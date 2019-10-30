// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"

class FSplineEditorEdModeToolkit : public FModeToolkit
{
public:

	FSplineEditorEdModeToolkit();
	static TSharedPtr<SEditableTextBox> NumberInput;
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:

	TSharedPtr<SWidget> ToolkitWidget;

public:
	static FReply OnGenerateButtonClick();
	static FReply ImportJsonFiles();
	static FReply WriteCurrentPoints2CurrentSelectedSplineCom();
	static FReply ExportFile();
private:
	static void OpenFileDialog(const FString& dialogTitle, const FString& defaultPath, const FString& fileTypes, TArray<FString>& outFileNames);
	static void OpenSaveFileDialog(const FString& dialogTitle, const FString& defaultPath, TArray<FString>& OutNames);
};
