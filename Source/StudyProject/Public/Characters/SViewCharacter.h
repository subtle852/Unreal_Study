// SViewCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Characters/SCharacter.h"
#include "InputActionValue.h"
#include "SViewCharacter.generated.h"

UENUM()
enum class EViewMode : uint8
{
    None,
    BackView,
    QuarterView,
    End
};

UCLASS()
class STUDYPROJECT_API ASViewCharacter : public ASCharacter
{
    GENERATED_BODY()

public:
    ASViewCharacter();

    virtual void BeginPlay() override;

    virtual void PossessedBy(AController* NewController) override;

    virtual void Tick(float DeltaSeconds) override;

    void ChangeView(const FInputActionValue& InValue);

    void SetViewMode(EViewMode InViewMode);

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    void Move(const FInputActionValue& InValue);

    void Look(const FInputActionValue& InValue);

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class USInputConfigData> PlayerCharacterInputConfigData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SViewCharacter", Meta = (AllowPrivateAccess))
    TObjectPtr<class UInputMappingContext> PlayerCharacterInputMappingContext;

    EViewMode CurrentViewMode = EViewMode::None;
    // UPROPERTY() ��ũ�θ� ������� �����Ƿ�, �ʱ�ȭ�� �����ؾ���.

    FVector DirectionToMove = FVector::ZeroVector;

    // ChangeView�� �ڿ������� �ϱ����� ����
    float DestArmLength = 0.f;

    float ArmLengthChangeSpeed = 3.f;

    FRotator DestArmRotation = FRotator::ZeroRotator;

    float ArmRotationChangeSpeed = 10.f;


};