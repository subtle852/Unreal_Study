// Game/SGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USGameInstance();
	//Ctrl + . �� Create Declaration/ Definition ����

	virtual void Init() override;
	
	virtual void Shutdown() override;

private:
	UPROPERTY()
	FString Name;

};
