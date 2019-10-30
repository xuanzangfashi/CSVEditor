// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "JsonPointFactory.generated.h"

/**
 * 
 */
UCLASS()
class SPLINEEDITOR_API UJsonPointFactory : public UFactory
{
	GENERATED_BODY()
public:
	UJsonPointFactory();
	virtual uint32 GetMenuCategories() const override;

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	//virtual UObject* ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled) override;
};
