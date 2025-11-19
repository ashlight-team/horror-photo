// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PhotoHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API APhotoHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UCommonUserWidget> BasicFrontendWidgetClass;
	
private:
	virtual void Tick(float DeltaSeconds) override;
	void PrintInventoryDebug();
	void PrintHotbarDebug();

	virtual void BeginPlay() override;
};
