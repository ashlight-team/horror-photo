// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemCustomDataValue.generated.h"

USTRUCT(BlueprintType)
struct PROJECTHORRORPHOTO_API FItemCustomDataValue
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FloatValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StringValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bBoolValue = false;
	
	bool operator==(const FItemCustomDataValue& Value) const;

	float AsFloat(float Default = 0.f) const;
};