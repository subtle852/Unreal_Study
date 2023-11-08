// STorch.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STorch.generated.h"

UCLASS()
class STUDYPROJECT_API ASTorch : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTorch();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTorch", Meta = (AllowPrivateAccess))
    TObjectPtr<class UBoxComponent> BoxComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTorch", Meta = (AllowPrivateAccess))
    TObjectPtr<class UStaticMeshComponent> BodyStaticMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTorch", Meta = (AllowPrivateAccess))
    TObjectPtr<class UPointLightComponent> PointLightComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ASTorch", Meta = (AllowPrivateAccess))
    TObjectPtr<class UParticleSystemComponent> ParticleSystemComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ASTorch", Meta = (AllowPrivateAccess))
    int32 ID;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ASTorch", Meta = (AllowPrivateAccess))
    int32 VisibleAnywhere;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ASTorch", Meta = (AllowPrivateAccess))
    int32 EditDefaultsOnly;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "ASTorch", Meta = (AllowPrivateAccess))
    int32 EditInstanceOnly;
};
