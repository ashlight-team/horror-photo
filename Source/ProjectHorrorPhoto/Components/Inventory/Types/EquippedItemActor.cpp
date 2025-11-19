// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippedItemActor.h"

#include "InventoryItemInstance.h"
#include "ItemDefinition.h"
#include "ItemViewActor.h"

AEquippedItemActor::AEquippedItemActor()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ViewChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("ViewChild"));
	ViewChild->SetupAttachment(Root);
	ViewChild->SetChildActorClass(AItemViewActor::StaticClass());
}

void AEquippedItemActor::InitFromInstance(class UInventoryItemInstance* Instance)
{
	LinkedInstance = Instance;

	TSubclassOf<AItemViewActor> ViewClass = AItemViewActor::StaticClass();
	if (Instance && Instance->ItemDef && Instance->ItemDef->FirstPersonViewClass)
	{
		ViewClass = Instance->ItemDef->FirstPersonViewClass;
	}

	ViewChild->SetChildActorClass(ViewClass);

	if (AItemViewActor* View = Cast<AItemViewActor>(ViewChild->GetChildActor()))
	{
		View->InitFromInstance(Instance);
		View->SetOwner(GetOwner());
		View->Mesh->SetOnlyOwnerSee(true);
		View->Mesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
		
		
	}
}
