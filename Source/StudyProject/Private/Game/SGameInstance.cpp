// Game/SGameInstance.cpp


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"// PrintString을 사용하기 위한 헤더
#include "SUnrealObjectClass.h"
#include "Examples/SFlyable.h"
#include "Examples/SPigeon.h"
#include "JsonObjectConverter.h"// Json관련 헤더
#include "UObject/SavePackage.h"// Json관련 헤더

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

#pragma region Property
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

	//USUnrealObjectClass* USObject1 = NewObject<USUnrealObjectClass>();
	//// 언리얼은 new 키워드가 아닌 NewObject<>() API를 사용함

	//// 1) 변수 실습

	//UE_LOG(LogTemp, Log, TEXT("USObject1's Name: %s"), *USObject1->GetName());
	//// 클래스에서 직접 정의해준 Getter()를 활용한 일반적인 FString 출력

	//FProperty* NameProperty = USUnrealObjectClass::StaticClass()->FindPropertyByName(TEXT("Name"));

	//FString CompiletimeUSObjectName;
	//if (nullptr != NameProperty)
	//{
	//	NameProperty->GetValue_InContainer(USObject1, &CompiletimeUSObjectName);
	//	UE_LOG(LogTemp, Log, TEXT("CompiletimeUSObjectName: %s"), *CompiletimeUSObjectName);
	//}
	//// 프로퍼티 시스템의 Getter()를 활용한 FString 출력

	//// 2) 함수 실습

	//USObject1->HelloUnreal();
	//// 일반적인 함수 호출 방법

	//UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	//if (nullptr != HelloUnrealFunction)
	//{
	//	USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	//}
	//// 프로퍼티 시스템을 활용한 함수 호출 방법
#pragma endregion Property

#pragma region Interface
	USPigeon* Pigeon1 = NewObject<USPigeon>();
	ISFlyable* Bird1 = Cast<ISFlyable>(Pigeon1);
	// 이처럼 대부분 인터페이스 개념은 업캐스팅을 하기 위해 사용함

	if(nullptr != Bird1)
	Bird1->Fly();

#pragma endregion Interface

#pragma region Serialization
	// Src
	FBirdData SrcRawData(TEXT("Pigeon17"), 17);
	UE_LOG(LogTemp, Log, TEXT("[SrcRawData] Name: %s, ID: %d"), *SrcRawData.Name, SrcRawData.ID);

	const FString SavedDir = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));// 저장 경로에 Saved 폴더를 추가 
	UE_LOG(LogTemp, Log, TEXT("SavedDir: %s"), *SavedDir);

	const FString RawDataFileName(TEXT("RawData.bin"));// 직렬화해서 저장할 파일 이름
	FString AbsolutePathForRawData = FPaths::Combine(*SavedDir, *RawDataFileName);// SavedDir에 RawDataFileName을 붙여서 최종 저장 경로인 AbsolutePathForRawData 생성
	UE_LOG(LogTemp, Log, TEXT("Relative path for saved file: %s"), *AbsolutePathForRawData);
	FPaths::MakeStandardFilename(AbsolutePathForRawData);// 언리얼에 맞게 파일 이름 수정
	UE_LOG(LogTemp, Log, TEXT("Absolute path for saved file: %s"), *AbsolutePathForRawData);

	FArchive* RawFileWriterAr = IFileManager::Get().CreateFileWriter(*AbsolutePathForRawData);
	if (nullptr != RawFileWriterAr)
	{
		*RawFileWriterAr << SrcRawData;//RawFileWriterAr에 SrcRawData를 넘겨줌
		RawFileWriterAr->Close();
		delete RawFileWriterAr;
		RawFileWriterAr = nullptr;
	}

	// Dst
	FBirdData DstRawData;
	FArchive* RawFileReaderAr = IFileManager::Get().CreateFileReader(*AbsolutePathForRawData);
	if (nullptr != RawFileReaderAr)
	{
		*RawFileReaderAr << DstRawData;//RawFileReaderAr에서 DstRawData를 끌어냄
		RawFileReaderAr->Close();
		delete RawFileReaderAr;
		RawFileReaderAr = nullptr;

		UE_LOG(LogTemp, Log, TEXT("[DstRawData] Name: %s, ID: %d"), *DstRawData.Name, DstRawData.ID);
	}

	SerializedPigeon = NewObject<USPigeon>();
	SerializedPigeon->SetName(TEXT("Pigeon76"));
	SerializedPigeon->SetID(76);
	UE_LOG(LogTemp, Log, TEXT("[SerializedPigeon] Name: %s, ID: %d"), *SerializedPigeon->GetName(), SerializedPigeon->GetID());

	const FString ObjectDataFileName(TEXT("ObjectData.bin"));
	FString AbsolutePathForObjectData = FPaths::Combine(*SavedDir, *ObjectDataFileName);// 위에서 했던 것처럼 SavedDir에 파일이름 붙여서 최종 저장 경로 생성
	FPaths::MakeStandardFilename(AbsolutePathForObjectData);// 언리얼에 맞게 파일 이름 수정

	TArray<uint8> BufferArray;// 작성하기 쉽게 uint8 1바이트
	FMemoryWriter MemoryWriterAr(BufferArray);
	SerializedPigeon->Serialize(MemoryWriterAr);

	TUniquePtr<FArchive> ObjectDataFileWriterAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*AbsolutePathForObjectData));
	if (nullptr != ObjectDataFileWriterAr)
	{
		*ObjectDataFileWriterAr << BufferArray;
		ObjectDataFileWriterAr->Close();

		ObjectDataFileWriterAr = nullptr; //delete ObjectDataFileWriterAr; 와 같은 효과.
	}

	TArray<uint8> BufferArrayFromObjectDataFile;
	TUniquePtr<FArchive> ObjectDataFileReaderAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*AbsolutePathForObjectData));
	if (nullptr != ObjectDataFileReaderAr)
	{
		*ObjectDataFileReaderAr << BufferArrayFromObjectDataFile;
		ObjectDataFileReaderAr->Close();

		ObjectDataFileReaderAr = nullptr;
	}

	FMemoryReader MemoryReaderAr(BufferArrayFromObjectDataFile);
	USPigeon* Pigeon77 = NewObject<USPigeon>();
	Pigeon77->Serialize(MemoryReaderAr);
	UE_LOG(LogTemp, Log, TEXT("[Pigeon77] Name: %s, ID: %d"), *Pigeon77->GetName(), Pigeon77->GetID());

#pragma endregion Serialization

#pragma region Json
	// 저장
	const FString JsonDataFileName(TEXT("StudyJsonFile.txt"));
	FString AbsolutePathForJsonData = FPaths::Combine(*SavedDir, *JsonDataFileName);
	FPaths::MakeStandardFilename(AbsolutePathForJsonData);

	TSharedRef<FJsonObject> SrcJsonObject = MakeShared<FJsonObject>();// FJsonObject는 UObject 전용, FJsonValue는 값 전용 
	FJsonObjectConverter::UStructToJsonObject(SerializedPigeon->GetClass(), SerializedPigeon, SrcJsonObject);

	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriterAr = TJsonWriterFactory<TCHAR>::Create(&JsonOutString);// 안정성을 위해 TSharedRef 사용
	if (true == FJsonSerializer::Serialize(SrcJsonObject, JsonWriterAr))
	{
		FFileHelper::SaveStringToFile(JsonOutString, *AbsolutePathForJsonData);
	}

	// 읽기
	FString JsonInString;
	FFileHelper::LoadFileToString(JsonInString, *AbsolutePathForJsonData);
	TSharedRef<TJsonReader<TCHAR>> JsonReaderAr = TJsonReaderFactory<TCHAR>::Create(JsonInString);

	TSharedPtr<FJsonObject> DstJsonObject;
	if (true == FJsonSerializer::Deserialize(JsonReaderAr, DstJsonObject))// Deserialize 해서 DstJsonObject에 저장을 하는 것
	{
		USPigeon* Pigeon78 = NewObject<USPigeon>();
		if (true == FJsonObjectConverter::JsonObjectToUStruct(DstJsonObject.ToSharedRef(), Pigeon78->GetClass(), Pigeon78))
		{
			UE_LOG(LogTemp, Log, TEXT("[Pigeon78] Name: %s, ID: %d"), *Pigeon78->GetName(), Pigeon78->GetID());
		}
	}

#pragma endregion Json
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("Shutdown() has been called."));

}
