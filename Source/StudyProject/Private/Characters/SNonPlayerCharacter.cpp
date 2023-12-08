// SNonPlayerCharacter.cpp


#include "Characters/SNonPlayerCharacter.h"
#include "Controllers/SAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animations/SAnimInstance.h"
#include "Characters/SRPGCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SStatComponent.h"
#include "Game/SPlayerState.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"

#include "Components/SWidgetComponent.h"
#include "UI/StudyUserWidget.h"
#include "UI/SW_HPBar.h"


ASNonPlayerCharacter::ASNonPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AIControllerClass = ASAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    // ASNonPlayerCharacter는 레벨에 배치되거나 새롭게 생성되면 SAIController의 빙의가 자동으로 진행됨.

    WidgetComponent = CreateDefaultSubobject<USWidgetComponent>(TEXT("WidgetComponent"));
    WidgetComponent->SetupAttachment(GetRootComponent());
    WidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
    WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    WidgetComponent->SetDrawSize(FVector2D(300.0f, 100.0f));
    WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASNonPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (false == IsPlayerControlled())
    {
        bUseControllerRotationYaw = false;

        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = true;
        GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

        GetCharacterMovement()->MaxWalkSpeed = 300.f;
    }

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance) && false == AnimInstance->OnMontageEnded.IsAlreadyBound(this, &ThisClass::OnAttackAnimMontageEnded))
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackAnimMontageEnded);
    }
}

float ASNonPlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    //CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);

    //if (CurrentHP < KINDA_SMALL_NUMBER)
    //{
    //    ASRPGCharacter* DamageCauserCharacter = Cast<ASRPGCharacter>(DamageCauser);
    //    if (true == ::IsValid(DamageCauserCharacter))
    //    {
    //        DamageCauserCharacter->SetCurrentEXP(DamageCauserCharacter->GetCurrentEXP() + 5);
    //    }
    //    CurrentHP = 0.f;
    //    bIsDead = true;
    //    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    //    ASAIController* AIController = Cast<ASAIController>(GetController());
    //    if (true == ::IsValid(AIController))
    //    {
    //        AIController->EndAI();
    //    }
    //}

    if (StatComponent->GetCurrentHP() < KINDA_SMALL_NUMBER)
    {
        if (true == ::IsValid(LastHitBy))
        {
            ASRPGCharacter* DamageCauserCharacter = Cast<ASRPGCharacter>(LastHitBy->GetPawn());
            if (true == ::IsValid(DamageCauserCharacter))
            {
                ASPlayerState* PS = Cast<ASPlayerState>(DamageCauserCharacter->GetPlayerState());
                if (true == ::IsValid(PS))
                {
                    PS->SetCurrentEXP(PS->GetCurrentEXP() + 20.f);
                }
            }
        }

        ASAIController* AIController = Cast<ASAIController>(GetController());
        if (true == ::IsValid(AIController))
        {
            AIController->EndAI();
        }
    }

    if (true == ::IsValid(LastHitBy))
    {
        ASRPGCharacter* DamageCauserCharacter = Cast<ASRPGCharacter>(LastHitBy->GetPawn());
        if (true == ::IsValid(DamageCauserCharacter))
        {
            ASPlayerState* PS = Cast<ASPlayerState>(DamageCauserCharacter->GetPlayerState());
            if (true == ::IsValid(PS))
            {
                UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s CurrentEXP : %f"), *GetName(), PS->GetCurrentEXP()));
            }
        }
    }

    return FinalDamageAmount;
}

void ASNonPlayerCharacter::SetWidget(UStudyUserWidget* InStudyUserWidget)
{
    USW_HPBar* HPBarWidget = Cast<USW_HPBar>(InStudyUserWidget);
    if (true == ::IsValid(HPBarWidget))
    {
        HPBarWidget->SetMaxHP(StatComponent->GetMaxHP());
        HPBarWidget->InitializeHPBarWidget(StatComponent);
        StatComponent->OnCurrentHPChangeDelegate.AddDynamic(HPBarWidget, &USW_HPBar::OnCurrentHPChange);
    }
}

void ASNonPlayerCharacter::Attack()
{
    TSet<AActor*> DetectedActors;
    FCollisionQueryParams Params(NAME_None, false, this);
    TArray<FHitResult> SweepResults;

    // Sweep 사용시 SweepResults에 하나의 객체가 중복 탐색되는 경우가 발생
    bool bResult = GetWorld()->SweepMultiByChannel(
        SweepResults,
        GetActorLocation(),
        GetActorLocation() + GetActorForwardVector() * AttackRange,
        FQuat::Identity,
        ECollisionChannel::ECC_EngineTraceChannel2,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

    //bool bResult = GetWorld()->LineTraceMultiByChannel(
    //    SweepResults,
    //    GetActorLocation(),
    //    GetActorLocation() + GetActorForwardVector() * AttackRange,
    //    ECollisionChannel::ECC_EngineTraceChannel2,
    //    Params
    //);

    FColor DrawColor = FColor::Red;

    if (true == bResult)
    {
        for (auto const& HitResult : SweepResults)
        {
            ASRPGCharacter* PC = Cast<ASRPGCharacter>(HitResult.GetActor());
            if (true == ::IsValid(PC))
            {
                // 중복 탐색을 막기위해 TSet 사용
                if (false == DetectedActors.Contains(HitResult.GetActor()))
                {
                    DetectedActors.Add(HitResult.GetActor());

                    DrawColor = FColor::Green;

                    FDamageEvent DamageEvent;
                    HitResult.GetActor()->TakeDamage(0.f, DamageEvent, GetController(), this);
                    //return;
                }
            }
            else
            {
                //DrawColor = FColor::Red;
            }
        }
    }

    FVector TraceVec = GetActorForwardVector() * AttackRange;
    FVector Center = GetActorLocation() + TraceVec * 0.5f;
    float HalfHeight = AttackRange * 0.5f + AttackRadius;
    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
    //FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
    float DebugLifeTime = 5.f;

    DrawDebugCapsule(
        GetWorld(),
        Center,
        HalfHeight,
        AttackRadius,
        CapsuleRot,
        DrawColor,
        false,
        DebugLifeTime);

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        AnimInstance->PlayAttackBasicAnimMontage();
        bIsAttacking = true;
        if (false == AnimInstance->OnMontageEnded.IsAlreadyBound(this, &ThisClass::OnAttackAnimMontageEnded))
        {
            AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackAnimMontageEnded);
        }
    }
}

void ASNonPlayerCharacter::OnAttackAnimMontageEnded(UAnimMontage* Montage, bool bIsInterrupt)
{
    bIsAttacking = false;
}
