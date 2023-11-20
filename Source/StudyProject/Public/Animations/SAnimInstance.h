// SAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckHitDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckCanNextComboDelegate);

UCLASS()
class STUDYPROJECT_API USAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

    friend class ASRPGCharacter;

public:
    USAnimInstance();

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
    void PlayAttackAnimMontage();

    UFUNCTION()
    void AnimNotify_CheckHit();

    UFUNCTION()
    void AnimNotify_CheckCanNextCombo();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USAnimInstance")
    float CurrentSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USAnimInstance")
    FVector Velocity;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USAnimInstance")
    uint8 bIsFalling : 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USAnimInstance")
    uint8 bIsCrouching : 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "USAnimInstance", Meta = (AllowPrivateAccess))
    TObjectPtr<class UAnimMontage> AttackAnimMontage;

    FOnCheckHitDelegate OnCheckHitDelegate;

    FOnCheckCanNextComboDelegate OnCheckCanNextComboDelegate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USAnimInstance", Meta = (AllowPrivateAccess))
    uint8 bIsDead : 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USAnimInstance", Meta = (AllowPrivateAccess))
    uint8 bIsSprint : 1;
};
