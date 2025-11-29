// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoPlayerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "PhotoPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectHorrorPhoto/AbilitySystem/PhotoGameplayTags.h"
#include "ProjectHorrorPhoto/AbilitySystem/Component/PhotoAbilitySystemComponent.h"
#include "ProjectHorrorPhoto/AbilitySystem/Input/PhotoAbilitySet.h"
#include "ProjectHorrorPhoto/AbilitySystem/Input/PhotoInputComponent.h"
#include "ProjectHorrorPhoto/CommonUI/CommonLayersSubsystem.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/InventoryManipulatorComponent.h"
#include "ProjectHorrorPhoto/Components/Inventory/Interfaces/ContextActionProvider.h"
#include "ProjectHorrorPhoto/Components/Physics/Grabber.h"
#include "ProjectHorrorPhoto/Useful/Useful.h"
#include "ProjectHorrorPhoto/ViewModels/Inventory/InventoryHotbarViewModel.h"
#include "ProjectHorrorPhoto/Widgets/ContextActions/ContextMenu.h"
#include "ProjectHorrorPhoto/Widgets/Inventory/InventoryHotbar.h"

APhotoPlayerCharacter::APhotoPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(GetCapsuleComponent());

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryManipulatorComponent = CreateDefaultSubobject<UInventoryManipulatorComponent>(
		TEXT("InventoryManipulatorComponent"));
}

bool APhotoPlayerCharacter::ToggleRagdoll()
{
	if (GetMesh()->IsSimulatingPhysics())
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionProfileName("CharacterMesh");
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		GetMesh()->SetRelativeLocationAndRotation(
			FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
			FRotator::ZeroRotator + FRotator(0.f, -90.f, 0.f));
		USceneComponent* CameraAttachComponent = UUseful::FindChildComponentByTag(this, FName("CameraAttachPoint"));
		SpringArmComponent->AttachToComponent(CameraAttachComponent,
		                                      FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CameraComponent->bUsePawnControlRotation = true;
	}
	else
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");
		GetCharacterMovement()->SetMovementMode(MOVE_None);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SpringArmComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                      FName("head"));
		CameraComponent->bUsePawnControlRotation = false;
	}
	return GetMesh()->IsSimulatingPhysics();
}

void APhotoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (BasicHUDLayoutWidgetClass)
	{
		UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance)
		{
			if (UCommonLayersSubsystem* LayersSubsystem = GameInstance->GetSubsystem<UCommonLayersSubsystem>())
			{
				LayersSubsystem->PushWidgetToLayer(FPhotoGameplayTags::Get().UI_Layer_Game, BasicHUDLayoutWidgetClass);
			}
		}
	}

	if (InventoryManipulatorComponent)
	{
		if (InventoryHotbarClass)
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance)
				{
					if (UCommonLayersSubsystem* LayersSubsystem = GameInstance->GetSubsystem<UCommonLayersSubsystem>())
					{
						if (UInventoryHotbar* Hotbar = Cast<UInventoryHotbar>(
							LayersSubsystem->PushWidgetToExtensionLayer(
								PC, FPhotoGameplayTags::Get().UI_Widget_InventoryHotbar, InventoryHotbarClass)))
						{
							InventoryHotbarWidget = Hotbar;
							UInventoryHotbarViewModel* HotbarViewModel = NewObject<UInventoryHotbarViewModel>();
							HotbarViewModel->InitData(InventoryManipulatorComponent);
							Hotbar->SetViewModel(HotbarViewModel);
						}
					}
				}
			}
		}
	}
}

void APhotoPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	GEngine->AddOnScreenDebugMessage(2222, 0.0f, FColor::Green,
	                                 FString::Printf(TEXT("Move: X = %f, Y = %f"), MovementVector.X, MovementVector.Y));

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APhotoPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	UPhotoAbilitySystemComponent* PhotoASC = Cast<UPhotoAbilitySystemComponent>(AbilitySystemComponent.Get());
	UGrabber* GrabberComp = FindComponentByClass<UGrabber>();

	bool bRotatingItem = true;
	bool bHasGrabbed = false;

	if (PhotoASC)
	{
		if (GrabberComp)
		{
			bHasGrabbed = GrabberComp->HasGrabbed();
		}

		bRotatingItem = PhotoASC->HasMatchingGameplayTag(FPhotoGameplayTags::Get().Status_Item_Rotating);
	}

	if (Controller != nullptr && (!bHasGrabbed || !bRotatingItem))
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void APhotoPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APhotoPlayerState* PhotoPlayerState = GetPlayerState<APhotoPlayerState>())
	{
		AbilitySystemComponent = Cast<UPhotoAbilitySystemComponent>(PhotoPlayerState->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PhotoPlayerState, this);

		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent.Get(), nullptr, this);
		}
	}
}

void APhotoPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UPhotoInputComponent* ISInputComponent = Cast<UPhotoInputComponent>(PlayerInputComponent);
	check(ISInputComponent);

	const FPhotoGameplayTags& GameplayTags = FPhotoGameplayTags::Get();
	TArray<uint32> BindHandles;

	ISInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::InputAbilityInputTagPressed,
	                                     &ThisClass::InputAbilityInputTagReleased, /*out*/ BindHandles);

	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this,
	                                   &ThisClass::Move);
	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this,
	                                   &ThisClass::Look);

	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_RotateMode, ETriggerEvent::Triggered, this,
	                                   &ThisClass::OnRotateModePressed);

	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_RotateMode, ETriggerEvent::Completed, this,
	                                   &ThisClass::OnRotateModePressed);

	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Rotate, ETriggerEvent::Triggered, this,
	                                   &ThisClass::OnRotateObject);

	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Interact, ETriggerEvent::Started, this,
	                                   &ThisClass::OnTryToOpenContextMenuForLookTarget);

	ISInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Interact, ETriggerEvent::Completed, this,
	                                   &ThisClass::OnCloseContextMenu);


	TArray<FGameplayTag> HotbarTags = {
		GameplayTags.InputTag_Hotbar_1,
		GameplayTags.InputTag_Hotbar_2,
		GameplayTags.InputTag_Hotbar_3,
		GameplayTags.InputTag_Hotbar_4,
		GameplayTags.InputTag_Hotbar_5,
		GameplayTags.InputTag_Hotbar_6,
		GameplayTags.InputTag_Hotbar_7,
		GameplayTags.InputTag_Hotbar_8,
		GameplayTags.InputTag_Hotbar_9
	};

	for (int32 i = 0; i < HotbarTags.Num(); ++i)
	{
		ISInputComponent->BindNativeAction(InputConfig, HotbarTags[i], ETriggerEvent::Started, this,
		                                   FName(*FString::Printf(TEXT("OnHotbar%d"), i + 1)));
	}
}

void APhotoPlayerCharacter::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void APhotoPlayerCharacter::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

UAbilitySystemComponent* APhotoPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void APhotoPlayerCharacter::OnHotbarInput(const FInputActionValue& Value, int32 SlotIndex)
{
	if (InventoryManipulatorComponent)
	{
		InventoryManipulatorComponent->ActivateHotbarSlot(SlotIndex);
	}
}

void APhotoPlayerCharacter::OnRotateModePressed(const FInputActionValue& Value)
{
	bool bValue = Value.Get<bool>();
	if (UPhotoAbilitySystemComponent* PhotoASC = Cast<UPhotoAbilitySystemComponent>(AbilitySystemComponent.Get()))
	{
		if (bValue)
		{
			PhotoASC->AddLooseGameplayTag(FPhotoGameplayTags::Get().Status_Item_Rotating);
		}
		else
		{
			PhotoASC->RemoveLooseGameplayTag(FPhotoGameplayTags::Get().Status_Item_Rotating, 2147483647);
		}
	}
}

void APhotoPlayerCharacter::OnRotateObject(const FInputActionValue& Value)
{
	UPhotoAbilitySystemComponent* PhotoASC = Cast<UPhotoAbilitySystemComponent>(AbilitySystemComponent.Get());
	if (PhotoASC)
	{
		if (!PhotoASC->HasMatchingGameplayTag(FPhotoGameplayTags::Get().Status_Item_Rotating))
		{
			return;
		}
	}
	if (UGrabber* GrabberComp = FindComponentByClass<UGrabber>())
	{
		if (GrabberComp->HasGrabbed())
		{
			FVector2D RotateAxisVector = Value.Get<FVector2D>();
			GrabberComp->Rotate(RotateAxisVector);
		}
	}
}

void APhotoPlayerCharacter::OnTryToOpenContextMenuForLookTarget(const FInputActionValue& Value)
{
	FHitResult HitResult;
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + (CameraComponent->GetForwardVector() * LookForwardTraceDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			IContextActionProvider* ContextActionProvider = Cast<IContextActionProvider>(HitActor);
			if (ContextActionProvider)
			{
				// TODO: this fucking shit must be rewrited cuz it looks horrible
				UContextMenuViewModel* ContextMenuViewModel = NewObject<UContextMenuViewModel>(this);
				ContextMenuViewModel->InitData(HitActor, this);

				if (!IsValid(CachedContextMenu))
				{
					UGameInstance* GameInstance = GetGameInstance();

					UCommonLayersSubsystem* CommonLayersSubsystem = GameInstance->GetSubsystem<UCommonLayersSubsystem>();
					if (CommonLayersSubsystem)
					{
						// TODO: if owner of pawn is not player controller then fuck epic games
						UCommonActivatableWidget* ContextMenuActivatable = CommonLayersSubsystem->PushWidgetToLayer_ForPlayer(Cast<APlayerController>(GetOwner()), FPhotoGameplayTags::Get().UI_Layer_GameMenu, ContextMenuWidgetClass);
						if (UContextMenu* ContextMenu = Cast<UContextMenu>(ContextMenuActivatable))
						{
							ContextMenu->SetViewModel(ContextMenuViewModel);
							CachedContextMenu = ContextMenu;
						}
					}
				}
				else
				{
					CachedContextMenu->SetViewModel(ContextMenuViewModel);
					CachedContextMenu->ActivateWidget();
				}

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ContextMenu");
			}
		}
	}
}

void APhotoPlayerCharacter::OnCloseContextMenu(const FInputActionValue& Value)
{
	if (IsValid(CachedContextMenu))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Close ContextMenu");
		CachedContextMenu->SetViewModel(nullptr);
		CachedContextMenu->DeactivateWidget();
		CachedContextMenu = nullptr;
	}
}
