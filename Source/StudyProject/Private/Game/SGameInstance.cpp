// Game/SGameInstance.cpp


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"// PrintString을 사용하기 위한 헤더파일

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));
}

void USGameInstance::Init()
{
	Super::Init();
	// virual로 만들어진 자식 클래스에서 부모 클래스를 지칭할 때, Super를 사용

	UE_LOG(LogTemp, Log, TEXT("Init() has been called by UE_LOG."));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Init() has been called by PrintString."));
	//GetWorld()는 UObject(월드 개체)를 전달하는 것 

	// UE_LOG는 OutputLog에서 확인이 가능하다 (서버 프로그래머들이 주로 사용)
	// PrintString은 ViewPort에서 확인이 가능하다 (클라 프로그래머들이 주로 사용)

	// 해당 함수들은 디버깅용으로
	// 디버깅이 끝나면 주석 처리를 해주는 것이 기본
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("Shutdown() has been called."));

}
