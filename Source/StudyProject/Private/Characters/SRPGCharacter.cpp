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

    // Timer ���� �� ����
    //GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnTimer, 1.0f, true);
    //GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
    // SprintTimer�� ���,
    // BeginPlay���� �����ϴ� ���� �ƴ�
    // Sprint�� ������ ��, SetTimer�� ����
    // Sprint�� ������ �ӵ��� BaseWalkSpeed�� �Ǿ��� ��, ClearTimer�� ������
}

void ASRPGCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //// �� �������� (���� �̻����)
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
        // Ư�� �ִϸ��̼��� ����Ǹ�
        // Sprint ���� �� SprintTimer ����
        USAnimInstance* AnimInstance = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
        if (true == ::IsValid(AnimInstance))
        {
            if (true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackBasicAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillAAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSkillBAnimMontage)
                || true == AnimInstance->Montage_IsPlaying(AnimInstance->AttackSuperAnimMontage))
            {
                GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

                // SprintTimer ����
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

            // SprintTimer ����
            GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
            SprintTimerCount = 0.f;

            bIsSprintCompleted = false;
        }
    }

    // TimerCount Ȱ���ؼ� Ÿ�̸� �����ϴ� ���
    //if (TimerCount >= 10)
    //{
    //    GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);

    //    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Clearing the timer.")));
    //}
}

void ASRPGCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // ���� �̻����
    // 
    //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("OnAttackMontageEnded")));
    //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    //bIsAttacking = false;
}

float ASRPGCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // ������ ��� TakeDamage �Լ� ��������
    if (true == bIsInvincible)
        return 0.0f;

    float FinalDamageAmount = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

    if (true == ::IsValid(DamageCauser))
    {
        // ���� Actor�� DamageCauser�� ����(�ִ� 360��, ������ ���� 0�� �������� �ð�������� ������ ����) ���ϱ�
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
            // Ÿ���� ���� ������
            // �̹� Sequence ���߿� SequenceEnded()�� ȣ����� ���� ������ ���,
            // �ٽ� Sequence ó������ �����Ű�� ���� �κ�
            SequenceEnded();

            // ������ ����
            // ��� 1) MovementMode �������� ������ �������� ����
            // �˹��� ���������� ���̷������ ���װ� �߻�
            // ������ ����� �˹��� ������ �ٲ㵵 ��������
            //GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

            // ��� 2) ��Ʈ�ѷ� Move ����
            APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (true == ::IsValid(PlayerController))
            {
                PlayerController->SetIgnoreMoveInput(true);
            }

            // �˹�
            FVector CharacterLocation = GetActorLocation();
            FVector NPCLocation = DamageCauser->GetActorLocation();
            FVector KnockbackDirection = (CharacterLocation - NPCLocation).GetSafeNormal();

            KnockbackDirection += DamageCauser->GetActorForwardVector();
            KnockbackDirection.Normalize();

            LaunchCharacter(KnockbackDirection * 1000.f, false, false);

            // �˹� ī�޶� ����ũ
            //APlayerController* PlayerController = Cast<APlayerController>(GetController());
            if (true == ::IsValid(HitReactShake))
            {
                PlayerController->ClientStartCameraShake(HitReactShake);
            }

            // ���� ����) �ִϸ��̼� ���� �����̽�
            // �ִϸ��̼� ���� �����̽� ���� ���� ���� Ȱ��ȭ
            AnimInstance->AngleDegree = AngleDegrees;
            AnimInstance->bIsHitReact = true;
            
            // ������) �ִϸ��̼� ��Ÿ��
            //AnimInstance->PlayHitReactAnimMontage(AngleDegrees);

            //FOnMontageEnded OnMontageEndedDelegate;
            //OnMontageEndedDelegate.BindUObject(this, &ThisClass::MontageEnded);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactFwdAnimMontage);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactBwdAnimMontage);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactLeftAnimMontage);
            //AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, AnimInstance->HitReactRightAnimMontage);
        }
    }

    // ü�� 0�� ����� ���̵Ǹ� RagDoll
    if (false == ::IsValid(GetStatComponent()))
    {
        return FinalDamageAmount;
    }

    if (GetStatComponent()->GetCurrentHP() < KINDA_SMALL_NUMBER)
    {
        // Ragdoll�� Ȱ��ȭ�ϱ� ���� PhysicsAsset�� �ִ��� Ȯ��
        if (USkeletalMeshComponent* MeshComponent = GetMesh())
        {
            if (UPhysicsAsset* PhysicsAsset = MeshComponent->GetPhysicsAsset())
            {
                // MeshComponent->SetPhysicsAsset(PhysicsAsset);// ������ �߻��� ���, �ʱ�ȭ ���ִ� �κ�
                MeshComponent->SetSimulatePhysics(true);
                // MeshComponent->SetAllBodiesBelowSimulatePhysics(NAME_None, true);// ���� ��ȣ�ۿ��ϴ� �κи� Ȱ��ȭ�ϴ� �κ�
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

    // SprintTimer ����
    GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &ThisClass::OnSprintTimer, 0.25f, true);

    bIsSprintStarted = true;

    StatComponent->SetIsSprint(true);
}

void ASRPGCharacter::SprintCompleted(const FInputActionValue& InValue)
{
    // ���� �� �޸��� ��ư�� �����ϴ� ��쵵 �ֱ⿡ �Ʒ� �κ��� ����
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
    // ������ ���� ���������� ���� 
    // ��� 1) LaunchCharacter
    //FVector ForceDirection = FVector(0.0f, 0.0f, -1.0f);
    //float ForceMagnitude = 500.0f;
    //LaunchCharacter(ForceDirection * ForceMagnitude, false, false);

    // ��� 2) GravityScale
    GetCharacterMovement()->GravityScale = 3.0f;

    // ���� ī�޶� ����ũ
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

    // ��ų ��� ������ �����¿� �Է� �������� ȸ�� ���Ѽ� ���� ���ϵ��� �������ִ� �κ�
    // ��ų ��� ������ �����¿� �Է� ������ ���ٸ� ���� ȸ������ ����
    // ���Ŀ� �� �κ��� ����ϴٸ�,
    // TargetDirection���� Lerp�� FInterpoTo �̿��ص� ��
    // ����, ����ó�� ���� ����� ���͸� �Ĵٺ����� ���� ������ ������ �������ֵ��� �����ص� ��
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
        Rotator.Pitch += 25.0f;// ���� �Ʒ��� Pitch ȸ��
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
    // �浹 �˻�
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

    // �浹�� ���͵��� TakeDamage ȣ��
    if (true == bResult)
    {
        if (true == ::IsValid(HitResult.GetActor()))
        {
            //UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));

            FDamageEvent DamageEvent;
            HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);
        }
    }

    // �浹 ����� 
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
    // MontageEnded�� �ٸ��� ���ͷ�Ʈ ���� �Ǵ��� �Ұ����ϱ� ������
    // Sequence�� �����ߴٸ�,
    // SequenceEnded�� ȣ���� �ȵǴ� ��찡 �־�� �ȵ�
    // 
    // �ذ� ��� 1) Sequence�� ����� �ȵǴ� ���
    // �ش� Sequence�� ȣ���ϴ� TakeDamage �Լ�����
    // Sequence�� ���� ���ǿ� �Ʒ��� �߰�
    // AnimInstance->AngleDegree = -1;// �� ���� Sequence�� ����ǵ��� 
    // AnimInstance->bIsHitReact = false;// �� ���� Sequence�� ����ǵ��� 
    // 
    // �ذ� ��� 2) Sequence�� ���ܵ� �Ǵ� ���
    // �ش� Sequence�� ȣ���ϴ� TakeDamage �Լ�����
    // Sequence�� �����ϱ� ����
    // SequenceEnded();
    // ���� ȣ��

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

    // ī�޶� ����ũ
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
        // ���� ��� ����
        const FVector TargetDirection = GetLastMovementInputVector();
        if (false == TargetDirection.IsNearlyZero())
        {
            FRotator TargetRotation = FRotationMatrix::MakeFromX(TargetDirection).Rotator();
            SetActorRotation(TargetRotation);
        }

        // ī�޶� ����ũ
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

            // ���� 1
            // �ִϸ��̼� �Լ� �浹 ���ϵ��� ������ ��

            // ���� 2
            // A�ִϸ��̼� ���� �ٷ� B�ִϸ��̼� ���۽�ų ��,
            // A�ִϸ��̼� MontageEnded ���� B�ִϸ��̼� �Լ��� ���� ������ �Ǹ�
            // Move_Walking ���·� B�ִϸ��̼��� ����Ǵ� ��찡 �߻� 
 
            // �̷��� �������� �ذ��ϱ� ���ؼ�
            // �Լ��� ��Ģ�� ����� ��
            // �ִϸ��̼� ���� ���� > �ִϸ��̼� ���� �÷��� ���� true > �ִϸ��̼� ����� ���õ� ���� > �ִϸ��̼� ���� 
            // >>> MontageEnded���� �ִϸ��̼� ����� ���õ� ���� > MontageEnded���� �ִϸ��̼� ���� �÷��� ���� false
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
        // ��Ÿ�ְ� �߰��� ���� ���
    }
    else
    {
        // ��Ÿ�ְ� ������ ����� ���
    }
}

void ASRPGCharacter::OnCurrentLevelChanged(int32 InOldCurrentLevel, int32 InNewCurrentLevel)
{
    ParticleSystemComponent->Activate(true);
}

