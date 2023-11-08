// STorch.cpp

#include "WorldStatics/STorch.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"

ASTorch::ASTorch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

    BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
    BodyStaticMeshComponent->SetupAttachment(GetRootComponent());
    BodyStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -30.f));

    PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
    PointLightComponent->SetupAttachment(GetRootComponent());
    PointLightComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.f));

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
    ParticleSystemComponent->SetupAttachment(GetRootComponent());
    ParticleSystemComponent->SetRelativeLocation(FVector(0.f, 0.f, 500.0f));

}
