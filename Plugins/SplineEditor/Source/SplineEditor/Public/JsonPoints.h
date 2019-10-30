// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "JsonPoints.generated.h"



USTRUCT(BlueprintType)
struct FJsonPoint
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		float t;
	UPROPERTY(EditAnywhere)
		float x;
	UPROPERTY(EditAnywhere)
		float y;
	UPROPERTY(EditAnywhere)
		float orientation;
};
/**
 * 
 */
UCLASS()
class SPLINEEDITOR_API UJsonPoints : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FJsonPoint> Points;
	
};
