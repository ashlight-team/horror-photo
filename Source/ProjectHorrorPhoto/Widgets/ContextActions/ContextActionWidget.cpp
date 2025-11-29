// Fill out your copyright notice in the Description page of Project Settings.


#include "ContextActionWidget.h"

#include "Components/Button.h"
#include "ProjectHorrorPhoto/Components/Inventory/Interfaces/ContextActionProvider.h"
#include "ProjectHorrorPhoto/Widgets/ViewModelCommon/TextDisplayWidget.h"
#include "ProjectHorrorPhoto/Widgets/ViewModelCommon/TextureDisplayWidget.h"

void UContextActionWidget::OnViewModelChanged(UContextActionViewModel* OldViewModel,
                                              UContextActionViewModel* NewViewModel)
{
	if (NewViewModel)
	{
		if (ActionButton->OnClicked.IsBound())
		{
			ActionButton->OnClicked.Clear();
		}
		ActionButton->OnClicked.AddDynamic(this, &UContextActionWidget::HandleActionButtonClicked);
		ActionImage->SetViewModel(NewViewModel->GetActionIconViewModel());
		ActionDisplayName->SetViewModel(NewViewModel->GetActionNameViewModel());
	}
}

void UContextActionWidget::HandleActionButtonClicked()
{
	if (UContextActionViewModel* VM = GetViewModel())
	{
		AActor* HitActor = VM->GetActionHitActor();
		AActor* CallerActor = VM->GetActionCallerActor();
		if (IsValid(HitActor) && IsValid(CallerActor))
		{
			FContextActionContext Context;
			Context.SourceActor = HitActor;
			Context.Caller = CallerActor;
			
			IContextActionProvider::Execute_ExecuteAction(HitActor,
				VM->GetActionTagViewModel()->GetGameplayTag(),
				NAME_None,
				Context);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ContextActionWidget: HitActor or CallerActor is invalid."));
		}
	}
}
