// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SplineActor.generated.h"

UCLASS()
class SPLINEEDITOR_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere, Category = "ASplineActor")
		class USceneComponent* root;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "ASplineActor")
		class USplineComponent* spline;
public:	
	// Sets default values for this actor's properties
	ASplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BuildSpline(class UJsonPoints* jsonPoints);
	

};
