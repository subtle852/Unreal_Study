// SWKCharacter.cpp


#include "Characters/SWKCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inputs/SInputConfigData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animations/SAnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SStatComponent.h"
#include "Game/SPlayerState.h"
#include "Characters/SRPGCharacter.h"

ASWKCharacter::ASWKCharacter()
    //: bIsAttacking(false)
{
    PrimaryActorTick.bCanEverTick = true;

    //CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

    //bUseControllerRotationPitch = false;
    //bUseControllerRotationYaw = false;
    //bUseControllerRotationRoll = false;

    //SpringArmComponent->bUsePawnControlRotation = true;
    //SpringArmComponent->bDoCollisionTest = true;
    //SpringArmComponent->bInheritPitch = true;
    //SpringArmComponent->bInheritYaw = true;
    //SpringArmComponent->bInheritRoll = false;

    //GetCharacterMovement()->bOrientRotationToMovement = true;
    //GetCharacterMovement()->bUseControllerDesiredRotation = false;
    //GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

    //GetCapsuleComponent()->SetCollisionProfileName(TEXT("SCharacter"));

    //ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    //ParticleSystemComponent->SetupAttachment(GetCapsuleComponent());
    //ParticleSystemComponent->SetAutoActivate(false);

    //ParticleSystemComponentExpEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponentExpEffect"));
    //ParticleSystemComponentExpEffect->SetupAttachment(GetRootComponent());
    //ParticleSystemComponentExpEffect->SetAutoActivate(false);
}

void ASWKCharacter::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (true == ::IsValid(PlayerController))
    {
        UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
        if (true == ::IsValid(Subsystem))
        {
            Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
        }
    }

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        if (false == AnimInstance->OnMontageEnded.IsAlreadyBound(this, &ThisClass::OnAttackMontageEnded))
        {
            AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackMontageEnded);
        }
        if (false == AnimInstance->OnCheckHitDelegate.IsAlreadyBound(this, &ThisClass::CheckHit))
        {
            AnimInstance->OnCheckHitDelegate.AddDynamic(this, &ThisClass::CheckHit);
        }
        if (false == AnimInstance->OnCheckCanNextComboDelegate.IsAlreadyBound(this, &ThisClass::CheckCanNextCombo))
        {
            AnimInstance->OnCheckCanNextComboDelegate.AddDynamic(this, &ThisClass::CheckCanNextCombo);
        }

        //AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackMontageEnded);
        //AnimInstance->OnCheckHitDelegate.AddDynamic(this, &ThisClass::CheckHit);
        //AnimInstance->OnCheckCanNextComboDelegate.AddDynamic(this, &ThisClass::CheckCanNextCombo);
    }

    ASPlayerState* PS = GetPlayerState<ASPlayerState>();
    if (true == ::IsValid(PS))
    {
        if (false == PS->OnCurrentLevelChangedDelegate.IsAlreadyBound(this, &ThisClass::OnCurrentLevelChanged))
        {
            PS->OnCurrentLevelChangedDelegate.AddDynamic(this, &ThisClass::OnCurrentLevelChanged);
        }
    }

    //if (false == ::IsValid(StatComponent))
    //{
    //    return;
    //}

    //if (false == StatComponent->OnSprintDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    //{
    //    StatComponent->OnSprintDelegate.AddDynamic(this, &ThisClass::OnCharacterDeath);
    //}

    //GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnTimer, 1.0f, true);
    //GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
}

void ASWKCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //if (true == bIsSprintStarted || true == bIsSprintCompleted)
    //{
    //    CharDeltaSeconds = DeltaSeconds;

    //    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));
    //}
}

//void ASWKCharacter::OnSprintTimer()
//{
//    SprintTimerCount += 0.25f;
//
//    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Timer Tick: %f"), SprintTimerCount));
//
//    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));
//
//    if (true == bIsSprintStarted)
//    {
//        bIsSprintCompleted = false;
//
//        GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(
//            GetCharacterMovement()->MaxWalkSpeed,
//            BaseSprintSpeed,
//            CharDeltaSeconds,
//            SprintInterpSpeed);
//    }
//
//    if (true == bIsSprintCompleted)
//    {
//        GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(
//            GetCharacterMovement()->MaxWalkSpeed,
//            BaseWalkSpeed,
//            CharDeltaSeconds,
//            SprintInterpSpeed);
//
//        if (GetCharacterMovement()->MaxWalkSpeed < BaseWalkSpeed + 5.f)
//        {
//            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
//
//            GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
//            SprintTimerCount = 0.f;
//
//            bIsSprintCompleted = false;
//        }
//    }
//}

//void ASWKCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
//{
//    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//    bIsAttacking = false;
//}

//float ASWKCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
//
//    if (GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
//    {
//        GetMesh()->SetSimulatePhysics(true);
//    }
//
//    return FinalDamageAmount;
//}

void ASWKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::Zoom);
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->SprintStartedAction, ETriggerEvent::Started, this, &ThisClass::SprintStarted);
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->SprintCompletedAction, ETriggerEvent::Completed, this, &ThisClass::SprintCompleted);
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        //EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackBasicAction, ETriggerEvent::Started, this, &ThisClass::Attack);
    }
}

//void ASWKCharacter::Move(const FInputActionValue& InValue)
//{
//    FVector2D MovementVector = InValue.Get<FVector2D>();
//    ForwardInputValue = MovementVector.X;
//    RightInputValue = MovementVector.Y;
//
//    const FRotator CurrentControlRotation = GetController()->GetControlRotation();
//    const FRotator CurrentControlRotationYaw(0.f, CurrentControlRotation.Yaw, 0.f);
//
//    FVector ForwardDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::X);
//    FVector RightDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::Y);
//
//    AddMovementInput(ForwardDirection, MovementVector.X);
//    AddMovementInput(RightDirection, MovementVector.Y);
//}

//void ASWKCharacter::SprintStarted(const FInputActionValue& InValue)
//{
//    if (GetCharacterMovement()->IsFalling() == true)
//        return;
//
//    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SprintStarted")));
//
//    bIsSprintStarted = true;
//
//    GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnSprintTimer, 0.25f, true);
//
//    StatComponent->SetIsSprint(true);
//}
//
//void ASWKCharacter::SprintCompleted(const FInputActionValue& InValue)
//{
//    //if (GetCharacterMovement()->IsFalling() == true)
//    //    return;
//
//    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SprintCompleted")));
//
//    bIsSprintStarted = false;
//    bIsSprintCompleted = true;
//
//    StatComponent->SetIsSprint(false);
//}

void ASWKCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D LookAxisVector = InValue.Get<FVector2D>();

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}
//
//void ASWKCharacter::AttackBasic(const FInputActionValue& InValue)
//{
//    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Attack() has been called.")));
//
//    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
//    if (false == ::IsValid(AnimInstance))
//    {
//        return;
//    }
//
//    if (true == AnimInstance->bIsFalling)
//    {
//        return;
//    }
//
//    if (0 == CurrentComboCount)
//    {
//        BeginCombo();
//        return;
//    }
//    else
//    {
//        ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
//        bIsAttackKeyPressed = true;
//    }
//}

void ASWKCharacter::CheckHit()
{
    //////////////////////////////////////////////// SweepSingleByChannel
    
    //    FHitResult HitResult;
    //    FCollisionQueryParams Params(NAME_None, false, this);
    //
    //    bool bResult = GetWorld()->SweepSingleByChannel(
    //        HitResult,
    //        GetActorLocation(),
    //        GetActorLocation() + GetActorForwardVector() * AttackRange,
    //        FQuat::Identity,
    //        ECollisionChannel::ECC_EngineTraceChannel2,
    //        FCollisionShape::MakeSphere(AttackRadius),
    //        Params
    //    );
    //
    //    //bool bResult = GetWorld()->LineTraceSingleByChannel(
    //    //	HitResult,
    //    //	GetActorLocation(),
    //    //	GetActorLocation() + AttackRange,
    //    //	ECollisionChannel::ECC_EngineTraceChannel2,
    //    //  Params
    //    //);
    //
    //    if (true == bResult)
    //    {
    //        if (true == ::IsValid(HitResult.GetActor()))
    //        {
    //            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
    //
    //            FDamageEvent DamageEvent;
    //            HitResult.GetActor()->TakeDamage(50.f, DamageEvent, GetController(), this);
    //        }
    //    }
    //
    //#pragma region CollisionDebugDrawing
    //    FVector TraceVec = GetActorForwardVector() * AttackRange;
    //    FVector Center = GetActorLocation() + TraceVec * 0.5f;
    //    float HalfHeight = AttackRange * 0.5f + AttackRadius;
    //    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
    //    FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
    //    float DebugLifeTime = 5.f;
    //
    //    DrawDebugCapsule(
    //        GetWorld(),
    //        Center,
    //        HalfHeight,
    //        AttackRadius,
    //        CapsuleRot,
    //        DrawColor,
    //        false,
    //        DebugLifeTime
    //    );
    //#pragma endregion

    //////////////////////////////////////////////// SweepMultiByChannel

    FVector TargetVector = GetActorForwardVector();

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackAirAnimMontage))
        {
            FRotator Rotator = GetActorForwardVector().Rotation();
            Rotator.Pitch -= 25.0f;// 각도 아래로 Pitch 회전
            FVector DiagnolVector = FRotationMatrix(Rotator).GetUnitAxis(EAxis::X);
            TargetVector = DiagnolVector;
        }
    }

    FCollisionQueryParams Params(NAME_None, false, this);
    TArray<FHitResult> SweepResults;

    bool bResult = GetWorld()->SweepMultiByChannel(
        SweepResults,
        GetActorLocation(),
        GetActorLocation() + TargetVector * AttackRange,
        FQuat::Identity,
        ECollisionChannel::ECC_EngineTraceChannel2,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

    FColor DrawColor = FColor::Red;

    if (true == bResult)
    {
        for (auto const& HitResult : SweepResults)
        {
            ASCharacter* NPC = Cast<ASCharacter>(HitResult.GetActor());
            if (true == ::IsValid(NPC))
            {
				// UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Detected!")));
				DrawColor = FColor::Green;

                FDamageEvent DamageEvent;
                HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);
				//return;
			}
			else
			{
				//DrawColor = FColor::Red;
			}
		}
	}

    // 충돌 디버그 
    FVector TraceVec = TargetVector * AttackRange;
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
        DebugLifeTime
    );

}

//void ASWKCharacter::BeginCombo()
//{
//    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
//    if (false == ::IsValid(AnimInstance))
//    {
//        return;
//    }
//
//    CurrentComboCount = 1;
//
//    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
//
//    AnimInstance->PlayAttackBasicAnimMontage();
//
//    FOnMontageEnded OnMontageEndedDelegate;
//    OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
//    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackBasicAnimMontage);
//}

//void ASWKCharacter::CheckCanNextCombo()
//{
//    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
//    if (false == ::IsValid(AnimInstance))
//    {
//        return;
//    }
//
//    if (true == bIsAttackKeyPressed)
//    {
//        CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);
//
//        FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
//        AnimInstance->Montage_JumpToSection(NextSectionName, AnimInstance->AttackBasicAnimMontage);
//        bIsAttackKeyPressed = false;
//    }
//}

//void ASWKCharacter::MontageEnded(UAnimMontage* InAnimMontage, bool bInterrupted)
//{
//    //ensure(0 != CurrentComboCount);
//
//    bIsDash = false;
//
//    CurrentComboCount = 0;
//    bIsAttackKeyPressed = false;
//    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
//}

void ASWKCharacter::OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel)
{
    ParticleSystemComponent->Activate(true);
}

