// STestCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "STestCharacter.generated.h"

UCLASS()
class STUDYPROJECT_API ASTestCharacter : public ASCharacter
{
    GENERATED_BODY()

    friend class UBTTask_Attack;

public:
    ASTestCharacter();

    virtual void BeginPlay() override;

    bool IsNowAttacking() const { return bIsAttacking; }

    virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    virtual void SetWidget(class UStudyUserWidget* InStudyUserWidget) override;

private:
    void Attack();

    UFUNCTION()
    void OnAttackAnimMontageEnded(class UAnimMontage* Montage, bool bIsInterrupt);

private:
    float AttackRange = 200.f;

    float AttackRadius = 50.f;

    bool bIsAttacking = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ASTestCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class USWidgetComponent> WidgetComponent;
};
