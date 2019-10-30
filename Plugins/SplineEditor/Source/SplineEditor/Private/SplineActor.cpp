// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "JsonPoints.h"
// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	this->spline = CreateDefaultSubobject<USplineComponent>(TEXT("spline"));
	this->spline->SetMobility(EComponentMobility::Movable);
	this->spline->SetRelativeLocation(FVector::ZeroVector);
	this->spline->SetRelativeRotation(FQuat::Identity);
	this->spline->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
	
	
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASplineActor::BuildSpline(UJsonPoints* jsonPoints)
{
	for (int i= 0; i < jsonPoints->Points.Num(); i++)
	{
		this->spline->AddSplinePointAtIndex(FVector(jsonPoints->Points[i].x, 0, jsonPoints->Points[i].y), i, ESplineCoordinateSpace::Local);
	}
}



