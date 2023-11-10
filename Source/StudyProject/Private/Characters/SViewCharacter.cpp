// SViewCharacter.cpp


#include "Characters/SViewCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inputs/SInputConfigData.h"

ASViewCharacter::ASViewCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASViewCharacter::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (true == ::IsValid(PlayerController))
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (true == ::IsValid(Subsystem))
        {
            Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
        }
    }
}

void ASViewCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SetViewMode(EViewMode::BackView);
}

void ASViewCharacter::SetViewMode(EViewMode InViewMode)
{
    if (CurrentViewMode == InViewMode)
    {
        return;
    }

    CurrentViewMode = InViewMode;

    switch (CurrentViewMode)
    {
        case EViewMode::BackView:
            SpringArmComponent->TargetArmLength = 400.f;
            SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);
            // ControlRotation�� Pawn�� ȸ���� ����ȭ -> Pawn�� ȸ���� SprintArm�� ȸ�� ����ȭ. �̷� ���� SetRotation()�� ���ǹ�.

            bUseControllerRotationPitch = false;
            bUseControllerRotationYaw = false;
            bUseControllerRotationRoll = false;

            SpringArmComponent->bUsePawnControlRotation = true;
            SpringArmComponent->bDoCollisionTest = true;
            SpringArmComponent->bInheritPitch = true;
            SpringArmComponent->bInheritYaw = true;
            SpringArmComponent->bInheritRoll = false;

            GetCharacterMovement()->bOrientRotationToMovement = true;
            GetCharacterMovement()->bUseControllerDesiredRotation = false;
            GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

            break;

        case EViewMode::End:
            break;
    
        default:
            break;
    }
}

void ASViewCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ASViewCharacter::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ASViewCharacter::Look);
    }
}

void ASViewCharacter::Move(const FInputActionValue& InValue)
{
    FVector2D MovementVector = InValue.Get<FVector2D>();

    switch (CurrentViewMode)
    {
        case EViewMode::None:
            break;

        case EViewMode::BackView:
        { // Switch-Case ���� ������ Scope�� �����ϸ� �ش� Scope ������ ���� ������ ��������.
            const FRotator ControlRotation = GetController()->GetControlRotation();
            const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

            const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
            const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

            AddMovementInput(ForwardVector, MovementVector.X);
            AddMovementInput(RightVector, MovementVector.Y);

            break;
        }

        case EViewMode::End:
            break;

        default:
            AddMovementInput(GetActorForwardVector(), MovementVector.X);
            AddMovementInput(GetActorRightVector(), MovementVector.Y);
            break;
    }
}

void ASViewCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D LookVector = InValue.Get<FVector2D>();

    switch (CurrentViewMode)
    {
        case EViewMode::None:
            break;

        case EViewMode::BackView:
            AddControllerYawInput(LookVector.X);
            AddControllerPitchInput(LookVector.Y);
            break;

        case EViewMode::End:
            break;

        default:
            break;
    }
}