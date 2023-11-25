// SAnimInstance.cpp


#include "Animations/SAnimInstance.h"
#include "Characters/SRPGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SStatComponent.h"

USAnimInstance::USAnimInstance()
{
    
}

void USAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    CurrentSpeed = 0.f;

    Velocity = FVector::ZeroVector;

    bIsFalling = false;

    bIsCrouching = false;

    bIsDead = false;

    bIsSprint = false;

    ASCharacter* OwnerCharacter = Cast<ASCharacter>(TryGetPawnOwner());
    if (true == ::IsValid(OwnerCharacter))
    {
        USStatComponent* StatComponent = OwnerCharacter->GetStatComponent();
        if (true == ::IsValid(StatComponent))
        {
            if (false == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
            {
                StatComponent->OnOutOfCurrentHPDelegate.AddDynamic(this, &ThisClass::OnCharacterDeath);
            }

            if (false == StatComponent->OnSprintDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterSprint))
            {
                StatComponent->OnSprintDelegate.AddDynamic(this, &ThisClass::OnCharacterSprint);
            }
        }
    }
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    ASCharacter* OwnerCharacter = Cast<ASCharacter>(TryGetPawnOwner());
    if (true == ::IsValid(OwnerCharacter))
    {
        UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
        if (true == ::IsValid(CharacterMovementComponent))
        {
            Velocity = CharacterMovementComponent->GetLastUpdateVelocity();
            CurrentSpeed = Velocity.Size();
            bIsFalling = CharacterMovementComponent->IsFalling();
            bIsCrouching = CharacterMovementComponent->IsCrouching();
            //bIsDead = OwnerCharacter->IsDead();
            //bIsSprint = OwnerCharacter->IsSprint();
        }
    }
}

void USAnimInstance::PlayAttackAnimMontage()
{
    if (bIsFalling == false) 
    {
        if (true == ::IsValid(AttackAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackAnimMontage))
            {
                Montage_Play(AttackAnimMontage);
            }
        }
    }
}

void USAnimInstance::AnimNotify_CheckHit()
{
    if (true == OnCheckHitDelegate.IsBound())
    {
        OnCheckHitDelegate.Broadcast();
    }
}

void USAnimInstance::AnimNotify_CheckCanNextCombo()
{
    if (true == OnCheckCanNextComboDelegate.IsBound())
    {
        OnCheckCanNextComboDelegate.Broadcast();
    }
}

void USAnimInstance::OnCharacterDeath()
{
    bIsDead = true;
}

void USAnimInstance::OnCharacterSprint(bool InSprint)
{
    bIsSprint = InSprint;
}
