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
                && false == Montage_IsPlaying(DashBwdAnimMontage))
            {
                // 플레이 할 때, bIsAttackBasic true
                // 애니메이션 끝날 때, 델리게이트 직접 만들거나 
                // 모든 몬타쥬 끝날때 실행되는 Montage_SetEndDelegate를 바인드걸어서 false로 바꿔주면 서로 충돌안함
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
                && false == Montage_IsPlaying(DashBwdAnimMontage))
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
                && false == Montage_IsPlaying(DashBwdAnimMontage))
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
                && false == Montage_IsPlaying(DashBwdAnimMontage))
            {
                Montage_Play(AttackSuperAnimMontage);
            }
        }
    }
}

void USAnimInstance::PlayDashAnimMontage()
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
                && false == Montage_IsPlaying(DashBwdAnimMontage))
            {
                Montage_Play(DashAnimMontage);
            }
        }
    }
}

void USAnimInstance::PlayDashBwdAnimMontage()
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
                && false == Montage_IsPlaying(DashBwdAnimMontage))
            {
                Montage_Play(DashBwdAnimMontage);
            }
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
