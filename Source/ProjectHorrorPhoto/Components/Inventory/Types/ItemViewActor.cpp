// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemViewActor.h"
#include "InventoryItemInstance.h"
#include "ItemDefinition.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


AItemViewActor::AItemViewActor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetEnableGravity(false);
	
}

void AItemViewActor::InitFromInstance(UInventoryItemInstance* Instance)
{
	LinkedInstance = Instance;

	if (Instance && Instance->ItemDef && Instance->ItemDef->StaticMesh)
	{
		Mesh->SetStaticMesh(Instance->ItemDef->StaticMesh);
	}

	ApplyVisuals();
}

void AItemViewActor::ApplyVisuals_Implementation(){}
