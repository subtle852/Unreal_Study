// STorch.cpp

#include "WorldStatics/STorch.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

ASTorch::ASTorch()
{
    // Tick �Լ� �۵� ���θ� �����ϴ� ����
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
    BoxComponent->SetRelativeLocation(FVector(850.f, 2505.f, 32.f));

    BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
    BodyStaticMeshComponent->SetupAttachment(GetRootComponent());
    BodyStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyStaticMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Pillar_50x500.Pillar_50x500'"));
    // ��� ������ �÷��� �߿��� ������� �����Ƿ�, static Ű���� ���
    if (true == BodyStaticMesh.Succeeded())
    {
        BodyStaticMeshComponent->SetStaticMesh(BodyStaticMesh.Object);
    }

    PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
    PointLightComponent->SetupAttachment(GetRootComponent());
    PointLightComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.f));

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    ParticleSystemComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.0f));
    static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleTemplate(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Fire.P_Fire'"));
    if (true == ParticleTemplate.Succeeded())
    {
        ParticleSystemComponent->SetTemplate(ParticleTemplate.Object);
    }

    RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));

}

void ASTorch::BeginPlay()
{
    Super::BeginPlay();

    RotationSpeed = 300.f;

    RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed, 0.f);// BeginPlay���� RotatingMovementComponent�� Ȱ���ϴ� ���
}

void ASTorch::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    AddActorWorldRotation(FRotator(0.f, RotationSpeed * DeltaSeconds, 0.f));// Tick�Լ����� AddActorWorldRotation�� Ȱ���ϴ� ���
}
