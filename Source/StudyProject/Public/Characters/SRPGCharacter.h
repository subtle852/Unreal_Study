// SRPGCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "InputActionValue.h"
#include "SRPGCharacter.generated.h"

UCLASS()
class STUDYPROJECT_API ASRPGCharacter : public ASCharacter
{
    GENERATED_BODY()

    // 애님노티파이 클래스
    friend class UAN_CheckHit; 
    friend class UAN_SequenceEnded;

public:
    ASRPGCharacter();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    // 타이머
    void OnSprintTimer();

    // InputValue
    float GetForwardInputValue() const { return ForwardInputValue; }

    float GetRightInputValue() const { return RightInputValue; }


    UFUNCTION()
    virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void Zoom(const FInputActionValue& InValue);

    virtual void Move(const FInputActionValue& InValue);

    virtual void SprintStarted(const FInputActionValue& InValue);

    virtual void SprintCompleted(const FInputActionValue& InValue);

    virtual void Look(const FInputActionValue& InValue);

    virtual void AttackBasic(const FInputActionValue& InValue);

    virtual void AttackAir(const FInputActionValue& InValue);

    virtual void AttackSkillA(const FInputActionValue& InValue);

    virtual void AttackSkillB(const FInputActionValue& InValue);

    virtual void AttackSuper(const FInputActionValue& InValue);

    virtual void Dash(const FInputActionValue& InValue);

    UFUNCTION()
    virtual void CheckHit();

    UFUNCTION()
    virtual void SequenceEnded();

    virtual void BeginCombo();

    UFUNCTION()
    virtual void CheckCanNextCombo();

    UFUNCTION()
    virtual void MontageEnded(class UAnimMontage* InAnimMontage, bool bInterrupted);
    // 몽타주가 완전히 종료될 때 호출 (정상적으로 종료되거나 강제로 종료되었을 때 모두 해당)

    UFUNCTION()
    virtual void MontageBlendingOut(class UAnimMontage* InAnimMontage, bool bInterrupted);
    // 몽타주가 블렌딩 아웃될 때 호출 (몽타주가 종료되면서 현재 애니메이션과 다른 애니메이션 또는 블렌드로 전환되는 시점에 호출)

    UFUNCTION()
    virtual void OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel);


protected:
    // Input
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess = true))
    float ForwardInputValue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess = true))
    float RightInputValue;

    // Component
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", meta = (AllowPrivateAccess))
    TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ASPlayerCharacter, Meta = (AllowPrivateAccess = true))
    TSubclassOf<class UCameraShakeBase> AttackShake;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ASPlayerCharacter, Meta = (AllowPrivateAccess = true))
    TSubclassOf<class UCameraShakeBase> HitReactShake;

    // Zoom
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess = true))
    float ExpectedSpringArmLength = 550.0f;

    // Attack
    uint8 bIsAttacking : 1;

    FString AttackAnimMontageSectionName = FString(TEXT("AttackBasic"));

    int32 MaxComboCount = 3;

    int32 CurrentComboCount = 0;

    bool bIsAttackKeyPressed = false;// 에디터에서 관리되거나 시리얼라이즈 될 필요 없으므로 그냥 bool 자료형 사용가능

    float AttackRange = 250.f;

    float AttackRadius = 50.f;

    // Sprint
    bool bIsSprintStarted = false;

    bool bIsSprintCompleted = false;

    FTimerHandle SprintTimerHandle;

    float SprintTimerCount = 0.f;

    float SprintInterpSpeed = 50.f;

    // Dash
    bool bIsDash = false;
    // Dash중 jog모션 없도록 하려면 Dash시작할 때, Dash 끝날때 StatComponent 델리게이트 만들어야 함

    bool bIsDashBwd = false;
    FVector DashBwdDirection = FVector::ZeroVector;
    float DashBwdValue = 100.f;

    // Invincible
    bool bIsInvincible = false;
};
