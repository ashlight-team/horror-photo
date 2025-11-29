// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mvvm/BaseViewModel.h"
#include "ProjectHorrorPhoto/Components/Inventory/Types/ContextAction.h"
#include "ProjectHorrorPhoto/ViewModels/Common/TagViewModel.h"
#include "ProjectHorrorPhoto/ViewModels/Common/TextureViewModel.h"
#include "ProjectHorrorPhoto/ViewModels/Common/TextViewModel.h"
#include "ContextActionViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UContextActionViewModel : public UBaseViewModel
{
	GENERATED_BODY()
	
public:
	UContextActionViewModel();
	
	void InitData(FContextAction ContextAction, AActor* HitActor, AActor* CallerActor);
private:
	void DisplayContextActionData(FContextAction ContextAction);
	
	VM_PROP_AG_AS(UTextViewModel*, ActionNameViewModel, public, public);
	VM_PROP_AG_AS(UTagViewModel*, ActionTagViewModel, public, public);
	VM_PROP_AG_AS(UTextureViewModel*, ActionIconViewModel, public, public);
	VM_PROP_AG_AS(AActor*, ActionHitActor, public, private);
	VM_PROP_AG_AS(AActor*, ActionCallerActor, public, private);
};
