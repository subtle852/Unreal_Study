// SCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class STUDYPROJECT_API ASCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ASCharacter();

    float GetMaxHP() const { return MaxHP; }

    float GetCurrentHP() const { return CurrentHP; }

    void SetMaxHP(float InMaxHP) { MaxHP = InMaxHP; }

    void SetCurrentHP(float InCurrentHP) { CurrentHP = InCurrentHP; }

    bool IsDead() const { return bIsDead; }

    bool IsSprint() const { return bIsSprint; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", meta = (AllowPrivateAccess))
    TObjectPtr<class USpringArmComponent> SpringArmComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTPSCharacter", meta = (AllowPrivateAccess))
    TObjectPtr<class UCameraComponent> CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    float MaxHP = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    float CurrentHP = 200.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    uint8 bIsDead : 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    uint8 bIsSprint : 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASRPGCharacter", Meta = (AllowPrivateAccess))
    float SprintSpeedMultiplier = 1.7f;

};