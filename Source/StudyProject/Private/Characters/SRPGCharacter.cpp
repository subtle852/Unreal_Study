// SRPGCharacter.cpp


#include "Characters/SRPGCharacter.h"
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

ASRPGCharacter::ASRPGCharacter()
    : bIsAttacking(false)
{
    PrimaryActorTick.bCanEverTick = true;

    CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->bDoCollisionTest = true;
    SpringArmComponent->bInheritPitch = true;
    SpringArmComponent->bInheritYaw = true;
    SpringArmComponent->bInheritRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

    GetCapsuleComponent()->SetCollisionProfileName(TEXT("SCharacter"));

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetCapsuleComponent());
    ParticleSystemComponent->SetAutoActivate(false);

}

void ASRPGCharacter::BeginPlay()
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
        AnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnAttackMontageEnded);
        AnimInstance->OnCheckHitDelegate.AddDynamic(this, &ThisClass::CheckHit);
        AnimInstance->OnCheckCanNextComboDelegate.AddDynamic(this, &ThisClass::CheckCanNextCombo);
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

void ASRPGCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (true == bIsSprintStarted || true == bIsSprintCompleted)
    {
        CharDeltaSeconds = DeltaSeconds;
    }
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));

    ////ÁÜ ¼±Çüº¸°£
    //if (FMath::Abs(SpringArmComponent->TargetArmLength - ExpectedSpringArmLength) > KINDA_SMALL_NUMBER)
    //{
    //    SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, ExpectedSpringArmLength, 0.05f);
    //}
    if (true == bIsDashStarted)
    {
        const FVector TargetDirection = GetLastMovementInputVector();

        if (false == TargetDirection.IsNearlyZero())
        {
            //DashSpeed = FMath::Lerp(
            //    DashSpeed,
            //    20.f,
            //    0.5f);

            LaunchCharacter(TargetDirection * 15.0f, false, false);
        }
        else
        {
            //DashSpeed = FMath::Lerp(
            //    DashSpeed,
            //    30.f,
            //    2.0f);

            LaunchCharacter(GetActorForwardVector() * -1 * 20.0f, false, false);
        }
    }
}

void ASRPGCharacter::OnSprintTimer()
{
    SprintTimerCount += 0.25f;

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Timer Tick: %f"), SprintTimerCount));

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MaxWalkSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed));

    if (true == bIsSprintStarted)
    {
        bIsSprintCompleted = false;

        GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(
            GetCharacterMovement()->MaxWalkSpeed,
            BaseSprintSpeed,
            0.05f);

        //GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(
        //    GetCharacterMovement()->MaxWalkSpeed,
        //    BaseSprintSpeed,
        //    CharDeltaSeconds,
        //    SprintInterpSpeed);
    }

    if (true == bIsSprintCompleted)
    {
        GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(
            GetCharacterMovement()->MaxWalkSpeed,
            BaseWalkSpeed,
            0.05f);

        //GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(
        //    GetCharacterMovement()->MaxWalkSpeed,
        //    BaseWalkSpeed,
        //    CharDeltaSeconds,
        //    SprintInterpSpeed);

        if (GetCharacterMovement()->MaxWalkSpeed < BaseWalkSpeed + 5.f)
        {
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

            GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
            SprintTimerCount = 0.f;

            bIsSprintCompleted = false;
        }
    }

    // Are we done counting?
    //if (TimerCount >= 10)
    //{
    //    // Clear the timer handle so it won't keep triggering events
    //    GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);

    //    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Clearing the timer.")));
    //}
}

void ASRPGCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    bIsAttacking = false;
}

float ASRPGCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    //CurrentHP = FMath::Clamp(CurrentHP - FinalDamageAmount, 0.f, MaxHP);

    //if (CurrentHP < KINDA_SMALL_NUMBER)
    //{
    //    bIsDead = true;
    //    CurrentHP = 0.f;
    //    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
    //}

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f / %.1f]"), *GetName(), CurrentHP, MaxHP));

    return FinalDamageAmount;
}

void ASRPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (true == ::IsValid(EnhancedInputComponent))
    {
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::Zoom);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->SprintStartedAction, ETriggerEvent::Started, this, &ThisClass::SprintStarted);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->SprintCompletedAction, ETriggerEvent::Completed, this, &ThisClass::SprintCompleted);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackBasicAction, ETriggerEvent::Started, this, &ThisClass::AttackBasic);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackSkillAAction, ETriggerEvent::Started, this, &ThisClass::AttackSkillA);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackSkillBAction, ETriggerEvent::Started, this, &ThisClass::AttackSkillB);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->AttackSuperAction, ETriggerEvent::Started, this, &ThisClass::AttackSuper);
        EnhancedInputComponent->BindAction(PlayerCharacterInputConfigData->DashAction, ETriggerEvent::Started, this, &ThisClass::Dash);
    }
}

void ASRPGCharacter::Zoom(const FInputActionValue& InValue)
{
    FVector2D ZoomVector = InValue.Get<FVector2D>();
    float InputValue = ZoomVector.X;

    if (InputValue >= KINDA_SMALL_NUMBER)
    {
        //ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength + 30.0f, 250.0f, 1000.0f);
        SpringArmComponent->TargetArmLength = FMath::Clamp<float>(SpringArmComponent->TargetArmLength + 30.0f, 250.0f, 1000.0f);
    }

    else
    {
        //ExpectedSpringArmLength = FMath::Clamp<float>(ExpectedSpringArmLength - 30.0f, 250.0f, 1000.0f);
        SpringArmComponent->TargetArmLength = FMath::Clamp<float>(SpringArmComponent->TargetArmLength - 30.0f, 250.0f, 1000.0f);
    }
}

void ASRPGCharacter::Move(const FInputActionValue& InValue)
{
    FVector2D MovementVector = InValue.Get<FVector2D>();
    ForwardInputValue = MovementVector.X;
    RightInputValue = MovementVector.Y;

    const FRotator CurrentControlRotation = GetController()->GetControlRotation();
    const FRotator CurrentControlRotationYaw(0.f, CurrentControlRotation.Yaw, 0.f);

    FVector ForwardDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::X);
    FVector RightDirection = FRotationMatrix(CurrentControlRotationYaw).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.X);
    AddMovementInput(RightDirection, MovementVector.Y);
}

void ASRPGCharacter::SprintStarted(const FInputActionValue& InValue)
{
    if (GetCharacterMovement()->IsFalling() == true)
        return;

    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SprintStarted")));

    bIsSprintStarted = true;

    GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnSprintTimer, 0.25f, true);

    StatComponent->SetIsSprint(true);
}

void ASRPGCharacter::SprintCompleted(const FInputActionValue& InValue)
{
    //if (GetCharacterMovement()->IsFalling() == true)
    //    return;

    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SprintCompleted")));

    bIsSprintStarted = false;
    bIsSprintCompleted = true;

    StatComponent->SetIsSprint(false);
}

void ASRPGCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D LookAxisVector = InValue.Get<FVector2D>();

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void ASRPGCharacter::AttackBasic(const FInputActionValue& InValue)
{
   //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Attack() has been called.")));

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == AnimInstance->bIsFalling)
    {
        return;
    }

    if (0 == CurrentComboCount)
    {
        BeginCombo();
        return;
    }
    else
    {
        ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
        bIsAttackKeyPressed = true;
    }
}

void ASRPGCharacter::AttackSkillA(const FInputActionValue& InValue)
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == AnimInstance->bIsFalling)
    {
        return;
    }

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    AnimInstance->PlayAttackSkillAAnimMontage();

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackSkillAAnimMontage);
}

void ASRPGCharacter::AttackSkillB(const FInputActionValue& InValue)
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == AnimInstance->bIsFalling)
    {
        return;
    }

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    AnimInstance->PlayAttackSkillBAnimMontage();

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackSkillBAnimMontage);
}

void ASRPGCharacter::AttackSuper(const FInputActionValue& InValue)
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == AnimInstance->bIsFalling)
    {
        return;
    }

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    AnimInstance->PlayAttackSuperAnimMontage();

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackSuperAnimMontage);
}

void ASRPGCharacter::Dash(const FInputActionValue& InValue)
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == AnimInstance->bIsFalling)
    {
        return;
    }

    //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    const FVector TargetDirection = GetLastMovementInputVector();

    //const FVector TargetTransform = GetActorLocation() + GetActorForwardVector() * 50.f;
    //SetActorRelativeLocation(TargetVector);

    if (false == TargetDirection.IsNearlyZero())
    {
        bIsDashStarted = true;

        //LaunchCharacter(TargetDirection * DashSpeed, false, false);

        AnimInstance->PlayDashAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->DashAnimMontage);
    }
    else
    {
        bIsDashStarted = true;

        //LaunchCharacter(GetActorForwardVector() * -1 * DashSpeed, false, false);

        AnimInstance->PlayDashBwdAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->DashBwdAnimMontage);
    }

}

void ASRPGCharacter::CheckHit()
{
    FHitResult HitResult;
    FCollisionQueryParams Params(NAME_None, false, this);

    bool bResult = GetWorld()->SweepSingleByChannel(
        HitResult,
        GetActorLocation(),
        GetActorLocation() + GetActorForwardVector() * AttackRange,
        FQuat::Identity,
        ECollisionChannel::ECC_EngineTraceChannel2,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

	//bool bResult = GetWorld()->LineTraceSingleByChannel(
	//	HitResult,
	//	GetActorLocation(),
	//	GetActorLocation() + GetActorForwardVector() * AttackRange,
	//	ECollisionChannel::ECC_EngineTraceChannel2,
    //  Params
	//);

    if (true == bResult)
    {
        if (true == ::IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

            FDamageEvent DamageEvent;
            HitResult.GetActor()->TakeDamage(50.f, DamageEvent, GetController(), this);
        }
    }

#pragma region CollisionDebugDrawing
    FVector TraceVec = GetActorForwardVector() * AttackRange;
    FVector Center = GetActorLocation() + TraceVec * 0.5f;
    float HalfHeight = AttackRange * 0.5f + AttackRadius;
    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
    FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
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
#pragma endregion
}

void ASRPGCharacter::BeginCombo()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    CurrentComboCount = 1;

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    AnimInstance->PlayAttackBasicAnimMontage();

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::EndCombo);
    AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackBasicAnimMontage);
}

void ASRPGCharacter::CheckCanNextCombo()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    if (true == bIsAttackKeyPressed)
    {
        CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

        FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
        AnimInstance->Montage_JumpToSection(NextSectionName, AnimInstance->AttackBasicAnimMontage);
        bIsAttackKeyPressed = false;
    }
}

void ASRPGCharacter::EndCombo(UAnimMontage* InAnimMontage, bool bInterrupted)
{
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("EndCombo has been called.")));

    //ensure(0 != CurrentComboCount);

    bIsDashStarted = false;

    CurrentComboCount = 0;
    bIsAttackKeyPressed = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ASRPGCharacter::OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel)
{
    ParticleSystemComponent->Activate(true);
}

