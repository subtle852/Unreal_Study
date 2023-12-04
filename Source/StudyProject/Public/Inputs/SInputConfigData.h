// SInputConfigData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SInputConfigData.generated.h"

/**
 *
 */
UCLASS()
class STUDYPROJECT_API USInputConfigData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> ZoomAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> SprintStartedAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> SprintCompletedAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> ChangeViewAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UInputAction> AttackBasicAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> AttackSkillAAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> AttackSkillBAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> AttackSuperAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInputAction> DashAction;

};