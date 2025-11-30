// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mvvm/BaseView.h"
#include "ProjectHorrorPhoto/CommonUI/InputRouterActivatableWidget.h"
#include "ProjectHorrorPhoto/ViewModels/ContextAction/ContextMenuViewModel.h"
#include "ContextMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTHORRORPHOTO_API UContextMenu : public UInputRouterActivatableWidget, public TBaseView<UContextMenu, UContextMenuViewModel>
{
	GENERATED_BODY()

private:
	virtual void OnViewModelChanged(UContextMenuViewModel* OldViewModel, UContextMenuViewModel* NewViewModel) override;

	void DisplayMenu();
	void ArrangeItems();

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* ActionsMenuCanvas = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	float RotationOffsetDegrees = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float ActionWidgetRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	float MenuRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true, AllowAbstract=false))
	TSubclassOf<class UContextActionWidget> ActionWidgetClass = nullptr;

	UPROPERTY()
	TArray<UContextActionViewModel*> CachedActions;

	virtual void NativeConstruct() override;

};
