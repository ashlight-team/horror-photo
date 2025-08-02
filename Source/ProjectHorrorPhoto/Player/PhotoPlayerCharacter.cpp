// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoPlayerCharacter.h"


// Sets default values
APhotoPlayerCharacter::APhotoPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APhotoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhotoPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APhotoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

