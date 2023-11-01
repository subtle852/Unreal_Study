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
	//Ctrl + . 로 Create Declaration/ Definition 가능

	virtual void Init() override;
	
	virtual void Shutdown() override;

private:
	UPROPERTY()
	FString Name;

	UPROPERTY()
	TObjectPtr<class USPigeon> SerializedPigeon;
	// class 전방선언
	// 헤더 파일에서 다른 헤더 파일을 참조하면, 다른 헤더 파일이 수정되었을 때 이 헤더 파일도 함께 컴파일 되니
	// 꼭 필요한 경우가 아니라면 헤더 파일에서 다른 헤더파일을 인클루드 하지 않고 전방선언을 활용하는 것이 좋음
};
