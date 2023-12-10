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
#include "Controllers/SPlayerController.h"

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

    //if (true == ::IsValid(StatComponent))
    //{
    //    if (false == StatComponent->OnOutOfCurrentHPDelegate.IsAlreadyBound(this, &ThisClass::OnCharacterDeath))
    //    {
    //        StatComponent->OnOutOfCurrentHPDelegate.AddDynamic(this, &ThisClass::OnCharacterDeath);
    //    }
    //}

    // Timer 시작 및 종료
    //GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnTimer, 1.0f, true);
    //GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
    // SprintTimer의 경우,
    // BeginPlay에서 관리하는 것이 아닌
    // Sprint가 시작할 때, SetTimer로 시작
    // Sprint가 끝나고 속도가 BaseWalkSpeed가 되었을 때, ClearTimer로 관리중
}

void ASRPGCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //// 줌 선형보간 (현재 미사용중)
    //if (FMath::Abs(SpringArmComponent->TargetArmLength - ExpectedSpringArmLength) > KINDA_SMALL_NUMBER)
    //{
    //    SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength, ExpectedSpringArmLength, 0.05f);
    //}

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), GetCharacterMovement()->MaxWalkSpeed));
}

void ASRPGCharacter::OnSprintTimer()
{
    SprintTimerCount += 0.25f;

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%f"), GetCharacterMovement()->MaxWalkSpeed));

    if (true == bIsSprintStarted)
    {
        bIsSprintCompleted = false;

        GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(
            GetCharacterMovement()->MaxWalkSpeed, 
            BaseSprintSpeed, 
            GetWorld()->GetDeltaSeconds(),
            SprintInterpSpeed);
    }

    else if (true == bIsSprintCompleted)
    {
        // 특정 애니메이션이 실행되면
        // Sprint 종료 및 SprintTimer 종료
        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == ::IsValid(AnimInstance))
        {
            if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage))
            {
                GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

                // SprintTimer 종료
                GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
                SprintTimerCount = 0.f;

                bIsSprintCompleted = false;

                return;
            }
        }

        GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpTo(
            GetCharacterMovement()->MaxWalkSpeed,
            BaseWalkSpeed,
            GetWorld()->GetDeltaSeconds(),
            SprintInterpSpeed);

        if (GetCharacterMovement()->MaxWalkSpeed < BaseWalkSpeed + 5.f)
        {
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

            // SprintTimer 종료
            GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
            SprintTimerCount = 0.f;

            bIsSprintCompleted = false;
        }
    }

    // TimerCount 활용해서 타이머 종료하는 방법
    //if (TimerCount >= 10)
    //{
    //    GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);

    //    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Clearing the timer.")));
    //}
}

void ASRPGCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // 현재 미사용중
    // 
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnAttackMontageEnded")));
    //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    //bIsAttacking = false;
}

float ASRPGCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 무적인 경우 TakeDamage 함수 조기종료
    if (true == bIsInvincible)
        return 0.0f;

    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    if (true == ::IsValid(DamageCauser))
    {
        // 현재 Actor와 DamageCauser의 각도(최대 360도, 오른팔 기준 0도 시작으로 시계방향으로 각도가 증가) 구하기
        FVector ActorForwardVector = GetActorForwardVector();
        FVector CauserForwardVector = DamageCauser->GetActorForwardVector();

        float DotProduct = FVector::DotProduct(ActorForwardVector, CauserForwardVector);
        float CrossProduct = FVector::CrossProduct(ActorForwardVector, CauserForwardVector).Z;

        float AngleRadians = FMath::Acos(DotProduct);
        float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

        if (CrossProduct > 0)
        {
            AngleDegrees = 360.0f - AngleDegrees;
        }

        //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Angle between hit direction and player forward vector : %f"), AngleDegrees));

        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == ::IsValid(AnimInstance))
        {
            // 타격을 받은 시점이
            // 이미 Sequence 도중에 SequenceEnded()가 호출되지 않은 시점인 경우,
            // 다시 Sequence 처음부터 실행시키기 위한 부분
            SequenceEnded();

            // 움직임 막기
            // 방법 1) MovementMode 변경으로 움직임 제한으로 변경
            // 넉백이 정상적으로 안이루어지는 버그가 발생
            // 움직임 막기와 넉백의 순서를 바꿔도 마찬가지
            //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

            // 방법 2) 컨트롤러 Move 막기
            APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (true == ::IsValid(PlayerController))
            {
                PlayerController->SetIgnoreMoveInput(true);
            }

            // 넉백
            FVector CharacterLocation = GetActorLocation();
            FVector NPCLocation = DamageCauser->GetActorLocation();
            FVector KnockbackDirection = (CharacterLocation - NPCLocation).GetSafeNormal();

            KnockbackDirection += DamageCauser->GetActorForwardVector();
            KnockbackDirection.Normalize();

            LaunchCharacter(KnockbackDirection * 1000.f, false, false);

            // 넉백 카메라 쉐이크
            //APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (true == ::IsValid(HitReactShake))
            {
                PlayerController->ClientStartCameraShake(HitReactShake);
            }

            // 현재 버전) 애니메이션 블랜드 스페이스
            // 애니메이션 블랜드 스페이스 실행 조건 변수 활성화
            AnimInstance->AngleDegree = AngleDegrees;
            AnimInstance->bIsHitReact = true;
            
            // 구버전) 애니메이션 몽타주
            //AnimInstance->PlayHitReactAnimMontage(AngleDegrees);

            //FOnMontageEnded OnMontageEndedDelegate;
            //OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactFwdAnimMontage);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactBwdAnimMontage);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactLeftAnimMontage);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactRightAnimMontage);
        }
    }

    // 체력 0에 가까운 값이되면 RagDoll
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
                // MeshComponent->SetPhysicsAsset(PhysicsAsset);// 오류가 발생할 경우, 초기화 해주는 부분
                MeshComponent->SetSimulatePhysics(true);
                // MeshComponent->SetAllBodiesBelowSimulatePhysics(NAME_None, true);// 땅과 상호작용하는 부분만 활성화하는 부분
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

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SprintStarted")));

    // SprintTimer 시작
    GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnSprintTimer, 0.25f, true);

    bIsSprintStarted = true;

    StatComponent->SetIsSprint(true);
}

void ASRPGCharacter::SprintCompleted(const FInputActionValue& InValue)
{
    // 점프 중 달리기 버튼을 해제하는 경우도 있기에 아래 부분은 제외
    //if (GetCharacterMovement()->IsFalling() == true)
    //    return;

    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SprintCompleted")));

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
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AttackBasic() has been called.")));

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    // AttackAir
    if (true == AnimInstance->bIsFalling)
    {
        AttackAir(InValue);

        return;
    }

    if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashAnimMontage)
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage)
        || true == AnimInstance->bIsHitReact)
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

void ASRPGCharacter::AttackAir(const FInputActionValue& InValue)
{
    // 땅으로 빨리 떨어지도록 설정 
    // 방법 1) LaunchCharacter
    //FVector ForceDirection = FVector(0.0f, 0.0f, -1.0f);
    //float ForceMagnitude = 500.0f;
    //LaunchCharacter(ForceDirection * ForceMagnitude, false, false);

    // 방법 2) GravityScale
    GetCharacterMovement()->GravityScale = 3.0f;

    // 공격 카메라 쉐이크
    ASPlayerController* PlayerController = Cast<ASPlayerController>(GetController());
    if (true == ::IsValid(PlayerController) && true == ::IsValid(AttackShake))
    {
        PlayerController->ClientStartCameraShake(AttackShake);
    }

    AttackRange = 200.f;
    AttackRadius = 50.f;

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance) && true == ::IsValid(AnimInstance->AttackAirAnimMontage))
    {
        AnimInstance->PlayAttackAirAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->AttackAirAnimMontage);
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
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage)
        || true == AnimInstance->bIsHitReact)
    {
        return;
    }

    AttackRange = 450.f;
    AttackRadius = 50.f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

    // 스킬 사용 직전에 상하좌우 입력 방향으로 회전 시켜서 조작 편하도록 수정해주는 부분
    // 스킬 사용 직전에 상하좌우 입력 방향이 없다면 굳이 회전하지 않음
    // 추후에 이 부분이 어색하다면,
    // TargetDirection으로 Lerp나 FInterpoTo 이용해도 됨
    // 또한, 원신처럼 가장 가까운 몬스터를 쳐다보도록 현재 엑터의 방향을 설정해주도록 수정해도 됨
    const FVector TargetDirection = GetLastMovementInputVector();
    if (false == TargetDirection.IsNearlyZero())
    {
        FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
        SetActorRotation(TargetRotation);
    }

    AnimInstance->PlayAttackSkillAAnimMontage();

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
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage)
        || true == AnimInstance->bIsHitReact)
    {
        return;
    }

    AttackRange = 600.f;
    AttackRadius = 50.f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
        || true == AnimInstance->Montage_IsPlaying(AnimInstance->DashBwdAnimMontage)
        || true == AnimInstance->bIsHitReact)
    {
        return;
    }

    bIsInvincible = true;

    AttackRange = 250.f;
    AttackRadius = 100.f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
        FRotator Rotator = GetActorForwardVector().Rotation();
        Rotator.Pitch += 25.0f;// 각도 아래로 Pitch 회전
        FVector DiagnolVector = FRotationMatrix(Rotator).GetUnitAxis(EAxis::X);

        LaunchCharacter(DiagnolVector * 500.0f, false, false);
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
        || true == AnimInstance->bIsHitReact)
    {
        return;
    }

    bIsDashStarted = true;
    bIsInvincible = true;

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    const FVector TargetDirection = GetLastMovementInputVector();
    if (false == TargetDirection.IsNearlyZero())
    {
        GetCharacterMovement()->MaxWalkSpeed = BaseSprintSpeed;

        AnimInstance->PlayDashAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->DashAnimMontage);
    }
    else
    {
        LaunchCharacter(GetActorForwardVector() * -1 * 2000.0f, false, false);

        AnimInstance->PlayDashBwdAnimMontage();

        FOnMontageEnded OnMontageEndedDelegate;
        OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
        AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->DashBwdAnimMontage);
    }

}

void ASRPGCharacter::CheckHit()
{
    // 충돌 검사
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

    // 충돌된 액터들의 TakeDamage 호출
    if (true == bResult)
    {
        if (true == ::IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

            FDamageEvent DamageEvent;
            HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);
        }
    }

    // 충돌 디버그 
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

void ASRPGCharacter::SequenceEnded()
{
    // MontageEnded와 다르게 인터럽트 여부 판단이 불가능하기 때문에
    // Sequence가 시작했다면,
    // SequenceEnded가 호출이 안되는 경우가 있어서는 안됨
    // 
    // 해결 방법 1) Sequence가 끊기면 안되는 경우
    // 해당 Sequence를 호출하는 TakeDamage 함수에서
    // Sequence를 시작 조건에 아래를 추가
    // AnimInstance->AngleDegree = -1;// 일 때만 Sequence가 실행되도록 
    // AnimInstance->bIsHitReact = false;// 일 때만 Sequence가 실행되도록 
    // 
    // 해결 방법 2) Sequence가 끊겨도 되는 경우
    // 해당 Sequence를 호출하는 TakeDamage 함수에서
    // Sequence를 실행하기 전에
    // SequenceEnded();
    // 강제 호출

    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (true == ::IsValid(AnimInstance))
    {
        AnimInstance->AngleDegree = -1;
        AnimInstance->bIsHitReact = false;
    }

    //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (true == ::IsValid(PlayerController))
    {
        PlayerController->SetIgnoreMoveInput(false);
    }
}

void ASRPGCharacter::BeginCombo()
{
    USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
    if (false == ::IsValid(AnimInstance))
    {
        return;
    }

    CurrentComboCount = 1;

    // 카메라 쉐이크
    ASPlayerController* PlayerController = Cast<ASPlayerController>(GetController());
    if (true == ::IsValid(PlayerController) && true == ::IsValid(AttackShake))
    {
        PlayerController->ClientStartCameraShake(AttackShake);
    }

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
        // 방향 즉시 변경
        const FVector TargetDirection = GetLastMovementInputVector();
        if (false == TargetDirection.IsNearlyZero())
        {
            FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
            SetActorRotation(TargetRotation);
        }

        // 카메라 쉐이크
        ASPlayerController* PlayerController = Cast<ASPlayerController>(GetController());
        if (true == ::IsValid(PlayerController) && true == ::IsValid(AttackShake))
        {
            PlayerController->ClientStartCameraShake(AttackShake);
        }

        GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

            // 문제 1
            // 애니메이션 함수 충돌 안하도록 만들어야 함

            // 문제 2
            // A애니메이션 직후 바로 B애니메이션 동작시킬 때,
            // A애니메이션 MontageEnded 보다 B애니메이션 함수가 먼저 실행이 되면
            // Move_Walking 상태로 B애니메이션이 진행되는 경우가 발생 
 
            // 이러한 문제들을 해결하기 위해선
            // 함수내 규칙을 따라야 함
            // 애니메이션 실행 조건 > 애니메이션 실행 플래그 변수 true > 애니메이션 실행과 관련된 설정 > 애니메이션 실행 
            // >>> MontageEnded에서 애니메이션 종료와 관련된 설정 > MontageEnded에서 애니메이션 실행 플래그 변수 false
        }

        if (AnimInstance->AttackAirAnimMontage == InAnimMontage)
        {
            GetCharacterMovement()->GravityScale = 1.0f;
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
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
            GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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

        //if (AnimInstance->HitReactFwdAnimMontage == InAnimMontage
        //    || AnimInstance->HitReactBwdAnimMontage == InAnimMontage
        //    || AnimInstance->HitReactLeftAnimMontage == InAnimMontage
        //    || AnimInstance->HitReactRightAnimMontage == InAnimMontage)
        //{
        //    APlayerController* PlayerController = Cast<APlayerController>(GetController());
        //    if (true == ::IsValid(PlayerController))
        //    {
        //        PlayerController->SetIgnoreMoveInput(false);
        //    }

        //    //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        //}
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

