// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextActionViewModel.h"

#include "ProjectHorrorPhoto/ViewModels/Common/TextureViewModel.h"

UContextActionViewModel::UContextActionViewModel()
{
	ActionNameViewModelField = NewObject<UTextViewModel>();
	ActionTagViewModelField = NewObject<UTagViewModel>();
	ActionIconViewModelField = NewObject<UTextureViewModel>();
}

void UContextActionViewModel::InitData(FContextAction ContextAction, AActor* HitActor, AActor* CallerActor)
{
	DisplayContextActionData(ContextAction);
	if (IsValid(HitActor) && IsValid(CallerActor))
	{
		SetActionHitActor(HitActor);
		SetActionCallerActor(CallerActor);
	}
}

void UContextActionViewModel::DisplayContextActionData(FContextAction ContextAction)
{
	ActionTagViewModelField->SetGameplayTag(ContextAction.ActionTag);
	ActionNameViewModelField->SetText(ContextAction.DisplayName);
	ActionIconViewModelField->SetTexture(nullptr);
	
	if (ContextAction.Icon)
	{
		ActionIconViewModelField->SetTexture(ContextAction.Icon);
	}
}
