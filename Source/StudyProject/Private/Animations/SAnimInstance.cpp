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

    bIsDash = false;

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

void USAnimInstance::PlayAttackBasicAnimMontage()
{
    if (bIsFalling == false) 
    {
        if (true == ::IsValid(AttackBasicAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackBasicAnimMontage)
                && false == Montage_IsPlaying(AttackSkillAAnimMontage)
                && false == Montage_IsPlaying(AttackSkillBAnimMontage)
                && false == Montage_IsPlaying(AttackSuperAnimMontage)
                && false == Montage_IsPlaying(DashAnimMontage)
                && false == Montage_IsPlaying(DashBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactFwdAnimMontage)
                && false == Montage_IsPlaying(HitReactBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactLeftAnimMontage)
                && false == Montage_IsPlaying(HitReactRightAnimMontage))
            {
                Montage_Play(AttackBasicAnimMontage);
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

void USAnimInstance::PlayAttackAirAnimMontage()
{
    if (bIsFalling == true)
    {
        if (true == ::IsValid(AttackAirAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackAirAnimMontage))
            Montage_Play(AttackAirAnimMontage);
        }
    }
}

void USAnimInstance::PlayAttackSkillAAnimMontage()
{
    if (bIsFalling == false)
    {
        if (true == ::IsValid(AttackSkillAAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackBasicAnimMontage)
                && false == Montage_IsPlaying(AttackSkillAAnimMontage)
                && false == Montage_IsPlaying(AttackSkillBAnimMontage)
                && false == Montage_IsPlaying(AttackSuperAnimMontage)
                && false == Montage_IsPlaying(DashAnimMontage)
                && false == Montage_IsPlaying(DashBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactFwdAnimMontage)
                && false == Montage_IsPlaying(HitReactBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactLeftAnimMontage)
                && false == Montage_IsPlaying(HitReactRightAnimMontage))
            {
                Montage_Play(AttackSkillAAnimMontage);
            }
        }
    }
}

void USAnimInstance::PlayAttackSkillBAnimMontage()
{
    if (bIsFalling == false)
    {
        if (true == ::IsValid(AttackSkillBAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackBasicAnimMontage)
                && false == Montage_IsPlaying(AttackSkillAAnimMontage)
                && false == Montage_IsPlaying(AttackSkillBAnimMontage)
                && false == Montage_IsPlaying(AttackSuperAnimMontage)
                && false == Montage_IsPlaying(DashAnimMontage)
                && false == Montage_IsPlaying(DashBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactFwdAnimMontage)
                && false == Montage_IsPlaying(HitReactBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactLeftAnimMontage)
                && false == Montage_IsPlaying(HitReactRightAnimMontage))
            {
                Montage_Play(AttackSkillBAnimMontage);
            }
        }
    }
}

void USAnimInstance::PlayAttackSuperAnimMontage()
{
    if (bIsFalling == false)
    {
        if (true == ::IsValid(AttackSuperAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackBasicAnimMontage)
                && false == Montage_IsPlaying(AttackSkillAAnimMontage)
                && false == Montage_IsPlaying(AttackSkillBAnimMontage)
                && false == Montage_IsPlaying(AttackSuperAnimMontage)
                && false == Montage_IsPlaying(DashAnimMontage)
                && false == Montage_IsPlaying(DashBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactFwdAnimMontage)
                && false == Montage_IsPlaying(HitReactBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactLeftAnimMontage)
                && false == Montage_IsPlaying(HitReactRightAnimMontage))
            {
                Montage_Play(AttackSuperAnimMontage);
            }
        }
    }
}

void USAnimInstance::PlayDashAnimMontage()
{
    if (true == ::IsValid(DashAnimMontage))
    {
        if (false == Montage_IsPlaying(AttackBasicAnimMontage)
            && false == Montage_IsPlaying(AttackSkillAAnimMontage)
            && false == Montage_IsPlaying(AttackSkillBAnimMontage)
            && false == Montage_IsPlaying(AttackSuperAnimMontage)
            && false == Montage_IsPlaying(DashAnimMontage)
            && false == Montage_IsPlaying(DashBwdAnimMontage)
            && false == Montage_IsPlaying(HitReactFwdAnimMontage)
            && false == Montage_IsPlaying(HitReactBwdAnimMontage)
            && false == Montage_IsPlaying(HitReactLeftAnimMontage)
            && false == Montage_IsPlaying(HitReactRightAnimMontage))
        {
            Montage_Play(DashAnimMontage);
        }
    }
    
}

void USAnimInstance::PlayDashBwdAnimMontage()
{
    if (bIsFalling == false)
    {
        if (true == ::IsValid(DashBwdAnimMontage))
        {
            if (false == Montage_IsPlaying(AttackBasicAnimMontage)
                && false == Montage_IsPlaying(AttackSkillAAnimMontage)
                && false == Montage_IsPlaying(AttackSkillBAnimMontage)
                && false == Montage_IsPlaying(AttackSuperAnimMontage)
                && false == Montage_IsPlaying(DashAnimMontage)
                && false == Montage_IsPlaying(DashBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactFwdAnimMontage)
                && false == Montage_IsPlaying(HitReactBwdAnimMontage)
                && false == Montage_IsPlaying(HitReactLeftAnimMontage)
                && false == Montage_IsPlaying(HitReactRightAnimMontage))
            {
                Montage_Play(DashBwdAnimMontage);
            }
        }
    }
}

void USAnimInstance::PlayHitReactAnimMontage(float InDegree)
{
    if (true == Montage_IsPlaying(AttackSuperAnimMontage)
        || true == Montage_IsPlaying(DashAnimMontage)
        || true == Montage_IsPlaying(DashBwdAnimMontage))
    {
        return;
    }

    if (0 <= InDegree && InDegree <= 45
        || 315 <= InDegree && InDegree <= 365)
    {
        if (true == ::IsValid(HitReactFwdAnimMontage))
        {
            Montage_Play(HitReactFwdAnimMontage);
        }
    }
    if (45 < InDegree && InDegree <= 135)
    {
        if (true == ::IsValid(HitReactLeftAnimMontage))
        {
            Montage_Play(HitReactLeftAnimMontage);
        }
    }
    if (135 < InDegree && InDegree <= 225)
    {
        if (true == ::IsValid(HitReactBwdAnimMontage))
        {
            Montage_Play(HitReactBwdAnimMontage);
        }
    }
    if (225 <= InDegree && InDegree < 315)
    {
        if (true == ::IsValid(HitReactRightAnimMontage))
        {
            Montage_Play(HitReactRightAnimMontage);
        }
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
