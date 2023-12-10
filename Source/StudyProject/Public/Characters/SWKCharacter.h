// SWKCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SRPGCharacter.h"
#include "InputActionValue.h"
#include "SWKCharacter.generated.h"

UCLASS()
class STUDYPROJECT_API ASWKCharacter : public ASRPGCharacter
{
    GENERATED_BODY()

    // 애님노티파이 클래스
    friend class UAN_CheckHit;

public:
    ASWKCharacter();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    // 타이머
    //void OnSprintTimer();

    //// InputValue
    //float GetForwardInputValue() const { return ForwardInputValue; }

    //float GetRightInputValue() const { return RightInputValue; }


    //UFUNCTION()
    //virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;

    //virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void Move(const FInputActionValue& InValue) override;

    //virtual void SprintStarted(const FInputActionValue& InValue) override;

    //virtual void SprintCompleted(const FInputActionValue& InValue) override;

    virtual void Look(const FInputActionValue& InValue) override;

    //virtual void AttackBasic(const FInputActionValue& InValue) override;


    //UFUNCTION()
    virtual void CheckHit() override;

    //virtual void BeginCombo() override;

    //UFUNCTION()
    //virtual void CheckCanNextCombo() override;

    //UFUNCTION()
    //virtual void MontageEnded(class UAnimMontage* InAnimMontage, bool bInterrupted) override;

    //UFUNCTION()
    virtual void OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel) override;


private:

};