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
#include "PhysicsEngine/PhysicsAsset.h"

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
    GetCharacterMovement()->RotationRate = FRotator(0.f, 600.f, 0.f);

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

    ////줌 선형보간
    //if (FMath::Abs(SpringArmComponent->TargetArmLength - ExpectedSpringArmLength) > KINDA_SMALL_NUMBER)
    //{
    //    SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, ExpectedSpringArmLength, 0.05f);
    //}

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
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnAttackMontageEnded")));
    //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    //bIsAttacking = false;
}

float ASRPGCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (true == bIsInvincible)
        return 0.0f;

    //USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    //if (true == ::IsValid(AnimInstance))
    //{
    //    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
    //    {
    //        return 0.0f;
    //    }
    //}

    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    if (true == ::IsValid(DamageCauser))
    {
        FVector DirectionToHit = (DamageCauser->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector ForwardVector = GetActorForwardVector();
        float DotProduct = FVector::DotProduct(DirectionToHit, ForwardVector);

        float AngleRadians = FMath::Acos(DotProduct);
        float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
        float CrossProduct = FVector::CrossProduct(DirectionToHit, ForwardVector).Z;
        if (CrossProduct < 0)
        {
            AngleDegrees = 360.0f - AngleDegrees;
        }

        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == ::IsValid(AnimInstance))
        {
            // Case 1. 입력을 강제로 막고 애니메이션 실행
            // 이부분을 해주지 않으면 Jog 모션과 함께 HitReact가 진행이 됨
            // GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);을 해도 똑같이 오류 발생
            APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (true == ::IsValid(PlayerController))
            {
                PlayerController->SetIgnoreMoveInput(true);
            }

            // Case 2. 넉백
            FVector CharacterLocation = GetActorLocation();
            FVector NPCLocation = DamageCauser->GetActorLocation();
            FVector KnockbackDirection = (CharacterLocation - NPCLocation).GetSafeNormal();

            KnockbackDirection += DamageCauser->GetActorForwardVector();
            KnockbackDirection.Normalize();

            LaunchCharacter(KnockbackDirection * 1000.f, false, false);

            AnimInstance->PlayHitReactAnimMontage(AngleDegrees);

            FOnMontageEnded OnMontageEndedDelegate;
            OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
            AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactFwdAnimMontage);
            AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactBwdAnimMontage);
            AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactLeftAnimMontage);
            AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactRightAnimMontage);
        }

        UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Angle between hit direction and player forward vector : %f"), AngleDegrees));
    }


    if (false == ::IsValid(GetStatComponent()))
    {
        return FinalDamageAmount;
    }

    if (GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
    {
        // Ragdoll을 활성화하기 전에 PhysicsAsset이 있는지 확인
        if (USkeletalMeshComponent* MeshComponent = GetMesh())
        {
            if (UPhysicsAsset* PhysicsAsset = MeshComponent->GetPhysicsAsset())
            {
                // PhysicsAsset을 사용하여 Ragdoll 설정
                //MeshComponent->SetPhysicsAsset(PhysicsAsset);
                MeshComponent->SetSimulatePhysics(true);

                // 땅과 상호작용하는 부분만 활성화
                //MeshComponent->SetAllBodiesBelowSimulatePhysics(NAME_None, true);
            }
        }
    }
    
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
        AttackRange = 200.f;
        AttackRadius = 50.f;

        AnimInstance->PlayAttackAirAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackAirAnimMontage);

        return;
    }

    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactFwdAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactBwdAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactLeftAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactRightAnimMontage))
    {
        return;
    }

    AttackRange = 200.f;
    AttackRadius = 50.f;

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

    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
    {
        return;
    }

    AttackRange = 450.f;
    AttackRadius = 50.f;

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    // 추후에 이 부분이 어색하다면,
    // 원신처럼 가장 가까운 몬스터를 쳐다보도록 현재 엑터의 방향을 설정해주면 됨
    const FVector TargetDirection = GetLastMovementInputVector();

    if (false == TargetDirection.IsNearlyZero())
    {
        FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
        SetActorRotation(TargetRotation);
    }

    AnimInstance->PlayAttackSkillAAnimMontage();

    //FOnMontageBlendingOutStarted OnMontageBlendingOutDelegate;
    //OnMontageBlendingOutDelegate.BindUObject(this, &ThisClass::MontageBlendingOut);
    //AnimInstance->Montage_SetBlendingOutDelegate(OnMontageBlendingOutDelegate, AnimInstance->AttackSkillAAnimMontage);

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
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

    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
    {
        return;
    }

    AttackRange = 600.f;
    AttackRadius = 50.f;

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    const FVector TargetDirection = GetLastMovementInputVector();

    if (false == TargetDirection.IsNearlyZero())
    {
        FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
        SetActorRotation(TargetRotation);
    }

    AnimInstance->PlayAttackSkillBAnimMontage();

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
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

    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
    {
        return;
    }

    bIsInvincible = true;

    AttackRange = 250.f;
    AttackRadius = 100.f;

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    const FVector TargetDirection = GetLastMovementInputVector();

    if (false == TargetDirection.IsNearlyZero())
    {
        FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
        SetActorRotation(TargetRotation);
    }

    AnimInstance->PlayAttackSuperAnimMontage();

    FOnMontageEnded OnMontageEndedDelegate;
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
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
        LaunchCharacter(GetActorForwardVector() * 500.0f, false, false);
        AnimInstance->PlayDashAnimMontage();
        //bIsDashStarted = true;

        return;
    }

    if (true == AnimInstance->bIsSprint)
    {
        //bIsDashStarted = true;
        LaunchCharacter(GetActorForwardVector() * 1500.0f, false, false);

        return;
    }

    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactFwdAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactBwdAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactLeftAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->HitReactRightAnimMontage))
    {
        return;
    }

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    const FVector TargetDirection = GetLastMovementInputVector();

    //const FVector TargetTransform = GetActorLocation() + GetActorForwardVector() * 50.f;
    //SetActorRelativeLocation(TargetVector);

    if (false == TargetDirection.IsNearlyZero())
    {
        bIsDashStarted = true;
        bIsInvincible = true;

        GetCharacterMovement()->MaxWalkSpeed = BaseSprintSpeed;

        AnimInstance->PlayDashAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->DashAnimMontage);
    }
    else
    {
        bIsDashStarted = true;
        bIsInvincible = true;

        LaunchCharacter(GetActorForwardVector() * -1 * 2000.0f, false, false);

        AnimInstance->PlayDashBwdAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
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

	//bool bResult = GetWorld()->LineTraceMultiByChannel(
	//	TraceResults,
	//	GetActorLocation(),
	//	GetActorLocation() + GetActorForwardVector() * AttackRange,
	//	ECollisionChannel::ECC_EngineTraceChannel2,
	//	Params
	//);

    if (true == bResult)
    {
        if (true == ::IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

            FDamageEvent DamageEvent;
            HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);
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
    OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
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
        const FVector TargetDirection = GetLastMovementInputVector();

        if (false == TargetDirection.IsNearlyZero())
        {
            FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
            SetActorRotation(TargetRotation);
        }

        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

        CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

        FName NextSectionName = *FString::Printf(TEXT("%s%d"), *AttackAnimMontageSectionName, CurrentComboCount);
        AnimInstance->Montage_JumpToSection(NextSectionName, AnimInstance->AttackBasicAnimMontage);
        bIsAttackKeyPressed = false;
    }
}

void ASRPGCharacter::MontageEnded(UAnimMontage* InAnimMontage, bool bInterrupted)
{
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("MontageEnded has been called.")));

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        if (AnimInstance->AttackBasicAnimMontage == InAnimMontage)
        {
            ensure(0 != CurrentComboCount);
            CurrentComboCount = 0;
            bIsAttackKeyPressed = false;

            if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
            {
                return;
            }

            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

            // 문제 1
            // 애니메이션 함수 충돌 안하도록 만들어야 함

            // 문제 2
            // A애니메이션 직후 바로 B애니메이션 동작시킬 때,
            // A애니메이션 MontageEnded 보다 B애니메이션 함수가 먼저 실행이 되면
            // Move_Walking 상태로 B애니메이션이 진행되는 경우가 발생 
        }

        if (AnimInstance->AttackSkillAAnimMontage == InAnimMontage)
        {
            if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
            {
                return;
            }

            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        }

        if (AnimInstance->AttackSkillBAnimMontage == InAnimMontage)
        {
            if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
            {
                return;
            }

            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        }

        if (AnimInstance->AttackSuperAnimMontage == InAnimMontage)
        {
            if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage))
            {
                return;
            }

            bIsInvincible = false;
            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        }

        if (AnimInstance->DashAnimMontage == InAnimMontage)
        {
            bIsDashStarted = false;
            bIsInvincible = false;
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
        }

        if (AnimInstance->DashBwdAnimMontage == InAnimMontage)
        {
            bIsDashStarted = false;
            bIsInvincible = false;
        }

        if (AnimInstance->HitReactFwdAnimMontage == InAnimMontage
            || AnimInstance->HitReactBwdAnimMontage == InAnimMontage
            || AnimInstance->HitReactLeftAnimMontage == InAnimMontage
            || AnimInstance->HitReactRightAnimMontage == InAnimMontage)
        {
            APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (true == ::IsValid(PlayerController))
            {
                PlayerController->SetIgnoreMoveInput(false);
            }

            //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        }
    }

}

void ASRPGCharacter::MontageBlendingOut(UAnimMontage* InAnimMontage, bool bInterrupted)
{
    if (bInterrupted)
    {
        // 몽타주가 중간에 끊긴 경우
    }
    else
    {
        // 몽타주가 완전히 종료된 경우
    }
}

void ASRPGCharacter::OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel)
{
    ParticleSystemComponent->Activate(true);
}

