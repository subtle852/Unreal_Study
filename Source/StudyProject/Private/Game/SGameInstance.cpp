// Game/SGameInstance.cpp


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"// PrintString을 사용하기 위한 헤더파일
#include "SUnrealObjectClass.h"

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));

	Name = TEXT("USGameInstance Class Default Object");
	// CDO의 Name 속성에 저장됨.
	// 중단점을 걸어보면 언리얼 에디터가 실행되기 전에 호출됨을 알 수 있음.
}

void USGameInstance::Init()
{
	Super::Init();
	// virual로 만들어진 자식 클래스에서 부모 클래스를 지칭할 때, Super를 사용

	UE_LOG(LogTemp, Log, TEXT("Init() has been called (UE_LOG)."));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Init() has been called (PrintString)."));
	//GetWorld()는 UObject(월드 객체)를 전달하는 것 

	// UE_LOG는 OutputLog에서 확인이 가능하다 (서버 프로그래머들이 주로 사용)
	// PrintString은 ViewPort에서 확인이 가능하다 (클라 프로그래머들이 주로 사용)

	// 해당 함수들은 디버깅용으로
	// 디버깅이 끝나면 주석 처리를 해주는 것이 기본



	////////////////////////////////////////
	//실습1 프로퍼티를 이용하여 Name 출력하기//
	////////////////////////////////////////

	//UClass* RuntimeClassInfo = GetClass();// 런타임 단계 에서 만들어진 클래스 정보
	//UClass* CompiletimeClassInfo = StaticClass();// 컴파일 단계에서 만들어진 클래스 정보

	////check(RuntimeClassInfo != CompiletimeClassInfo); 주석 풀어서 결과 확인 필요
	////ensure(RuntimeClassInfo != CompiletimeClassInfo);
	////ensureMsgf(RuntimeClassInfo != CompiletimeClassInfo, TEXT("Intentional Error"));

	//UE_LOG(LogTemp, Log, TEXT("Class Name: %s"), *RuntimeClassInfo->GetName());

	//Name = TEXT("USGameInstance Object");
	//// CDO를 통해 생성된 개체의 Name 속성에 새롭게 대입되는 값.

	//UE_LOG(LogTemp, Log, TEXT("USGameInstance::Name: %s"), *(GetClass()->GetDefaultObject<USGameInstance>()->Name));
	//UE_LOG(LogTemp, Log, TEXT("USGameInstance::Name: %s"), *Name);



	/////////////////////////////////////////////////////////
	//실습2 프로퍼티를 이용하여 Name 출력 + Function 호출하기//
	/////////////////////////////////////////////////////////

	USUnrealObjectClass* USObject1 = NewObject<USUnrealObjectClass>();
	// 언리얼은 new 키워드가 아닌 NewObject<>() API를 사용함

	// 1) 변수 실습

	UE_LOG(LogTemp, Log, TEXT("USObject1's Name: %s"), *USObject1->GetName());
	// 클래스에서 직접 정의해준 Getter()를 활용한 일반적인 FString 출력

	FProperty* NameProperty = USUnrealObjectClass::StaticClass()->FindPropertyByName(TEXT("Name"));

	FString CompiletimeUSObjectName;
	if (nullptr != NameProperty)
	{
		NameProperty->GetValue_InContainer(USObject1, &CompiletimeUSObjectName);
		UE_LOG(LogTemp, Log, TEXT("CompiletimeUSObjectName: %s"), *CompiletimeUSObjectName);
	}
	// 프로퍼티 시스템의 Getter()를 활용한 FString 출력

	// 2) 함수 실습

	USObject1->HelloUnreal();
	// 일반적인 함수 호출 방법

	UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	if (nullptr != HelloUnrealFunction)
	{
		USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	}
	// 프로퍼티 시스템을 활용한 함수 호출 방법
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("Shutdown() has been called."));

}
