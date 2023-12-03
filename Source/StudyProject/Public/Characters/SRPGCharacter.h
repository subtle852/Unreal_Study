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

    // �ִԳ�Ƽ���� Ŭ����
    friend class UAN_CheckHit;

public:
    ASRPGCharacter();

    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    // Ÿ�̸�
    void OnSprintTimer();

    // InputValue
    float GetForwardInputValue() const { return ForwardInputValue; }

    float GetRightInputValue() const { return RightInputValue; }


    UFUNCTION()
    virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual void Move(const FInputActionValue& InValue);

    virtual void SprintStarted(const FInputActionValue& InValue);

    virtual void SprintCompleted(const FInputActionValue& InValue);

    virtual void Look(const FInputActionValue& InValue);

    virtual void Attack(const FInputActionValue& InValue);


    UFUNCTION()
    virtual void CheckHit();

    virtual void BeginCombo();

    UFUNCTION()
    virtual void CheckCanNextCombo();

    UFUNCTION()
    virtual void EndCombo(class UAnimMontage* InAnimMontage, bool bInterrupted);

    UFUNCTION()
    virtual void OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel);


protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess = true))
    float ForwardInputValue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess = true))
    float RightInputValue;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", meta = (AllowPrivateAccess))
    TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

    // Attack
    uint8 bIsAttacking : 1;

    FString AttackAnimMontageSectionName = FString(TEXT("Attack"));

    int32 MaxComboCount = 3;

    int32 CurrentComboCount = 0;

    bool bIsAttackKeyPressed = false;
    // �����Ϳ��� �����ǰų� �ø�������� �� �ʿ� �����Ƿ� �׳� bool �ڷ��� ��밡��

    float AttackRange = 200.f;

    float AttackRadius = 50.f;

    // Sprint
    bool bIsSprintStarted = false;

    bool bIsSprintCompleted = false;

    FTimerHandle SprintTimerHandle;

    float SprintTimerCount = 0.f;

    UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    float CharDeltaSeconds;

    float SprintInterpSpeed = 50.f;
};