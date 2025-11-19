// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PhotoSceneCaptureComponent2D.generated.h"

USTRUCT(BlueprintType)
struct FActorCaptureInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* Actor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float CoveragePercent = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CapturedPixels = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float TotalPixels = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float DistanceFromCamera = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FVector RelativeLocation = FVector::ZeroVector;
};


USTRUCT(BlueprintType)
struct FPhotoData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<uint8> ImageData; 

	UPROPERTY(BlueprintReadWrite)
	TArray<FActorCaptureInfo> VisibleActors; 
};



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTHORRORPHOTO_API UPhotoSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FPhotoData TakePhoto(TArray<AActor*> IgnoreActors);

protected:
	bool CaptureToPNG(TArray<uint8>& OutImage);

	TArray<FActorCaptureInfo> CaptureVisibleActors(float MinCoveragePercent = 0.1f, TArray<AActor*> IgnoreActors = {});

	bool IsInFOV(const AActor* Actor);

	void CountRenderedCoverage(AActor* Actor, float& OutCaptured, float& OutTotal);
	
};
