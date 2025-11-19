// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ItemDefinition.generated.h"


USTRUCT(BlueprintType)
struct FItemStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Weight = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Durability = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Capacity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damage = 0.f;
	bool operator==(const FItemStats& ItemStats) const;
};

UCLASS(BlueprintType)
class PROJECTHORRORPHOTO_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AItem> OverrideActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemStats BaseStats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bStackable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UPhotoAbilitySet* GrantedAbilitySet = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer ItemTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View")
	TSubclassOf<class AItemViewActor> FirstPersonViewClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="View")
	TSubclassOf<class AItemViewActor> WorldViewClass;
};