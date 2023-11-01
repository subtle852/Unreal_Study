// Game/SGameInstance.cpp


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"// PrintString�� ����ϱ� ���� ���
#include "SUnrealObjectClass.h"
#include "Examples/SFlyable.h"
#include "Examples/SPigeon.h"
#include "JsonObjectConverter.h"// Json���� ���
#include "UObject/SavePackage.h"// Json���� ���

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));

	Name = TEXT("USGameInstance Class Default Object");
	// CDO�� Name �Ӽ��� �����.
	// �ߴ����� �ɾ�� �𸮾� �����Ͱ� ����Ǳ� ���� ȣ����� �� �� ����.
}

void USGameInstance::Init()
{
	Super::Init();
	// virual�� ������� �ڽ� Ŭ�������� �θ� Ŭ������ ��Ī�� ��, Super�� ���

	UE_LOG(LogTemp, Log, TEXT("Init() has been called (UE_LOG)."));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Init() has been called (PrintString)."));
	//GetWorld()�� UObject(���� ��ü)�� �����ϴ� �� 

	// UE_LOG�� OutputLog���� Ȯ���� �����ϴ� (���� ���α׷��ӵ��� �ַ� ���)
	// PrintString�� ViewPort���� Ȯ���� �����ϴ� (Ŭ�� ���α׷��ӵ��� �ַ� ���)

	// �ش� �Լ����� ����������
	// ������� ������ �ּ� ó���� ���ִ� ���� �⺻

#pragma region Property
	////////////////////////////////////////
	//�ǽ�1 ������Ƽ�� �̿��Ͽ� Name ����ϱ�//
	////////////////////////////////////////

	//UClass* RuntimeClassInfo = GetClass();// ��Ÿ�� �ܰ� ���� ������� Ŭ���� ����
	//UClass* CompiletimeClassInfo = StaticClass();// ������ �ܰ迡�� ������� Ŭ���� ����

	////check(RuntimeClassInfo != CompiletimeClassInfo); �ּ� Ǯ� ��� Ȯ�� �ʿ�
	////ensure(RuntimeClassInfo != CompiletimeClassInfo);
	////ensureMsgf(RuntimeClassInfo != CompiletimeClassInfo, TEXT("Intentional Error"));

	//UE_LOG(LogTemp, Log, TEXT("Class Name: %s"), *RuntimeClassInfo->GetName());

	//Name = TEXT("USGameInstance Object");
	//// CDO�� ���� ������ ��ü�� Name �Ӽ��� ���Ӱ� ���ԵǴ� ��.

	//UE_LOG(LogTemp, Log, TEXT("USGameInstance::Name: %s"), *(GetClass()->GetDefaultObject<USGameInstance>()->Name));
	//UE_LOG(LogTemp, Log, TEXT("USGameInstance::Name: %s"), *Name);



	/////////////////////////////////////////////////////////
	//�ǽ�2 ������Ƽ�� �̿��Ͽ� Name ��� + Function ȣ���ϱ�//
	/////////////////////////////////////////////////////////

	//USUnrealObjectClass* USObject1 = NewObject<USUnrealObjectClass>();
	//// �𸮾��� new Ű���尡 �ƴ� NewObject<>() API�� �����

	//// 1) ���� �ǽ�

	//UE_LOG(LogTemp, Log, TEXT("USObject1's Name: %s"), *USObject1->GetName());
	//// Ŭ�������� ���� �������� Getter()�� Ȱ���� �Ϲ����� FString ���

	//FProperty* NameProperty = USUnrealObjectClass::StaticClass()->FindPropertyByName(TEXT("Name"));

	//FString CompiletimeUSObjectName;
	//if (nullptr != NameProperty)
	//{
	//	NameProperty->GetValue_InContainer(USObject1, &CompiletimeUSObjectName);
	//	UE_LOG(LogTemp, Log, TEXT("CompiletimeUSObjectName: %s"), *CompiletimeUSObjectName);
	//}
	//// ������Ƽ �ý����� Getter()�� Ȱ���� FString ���

	//// 2) �Լ� �ǽ�

	//USObject1->HelloUnreal();
	//// �Ϲ����� �Լ� ȣ�� ���

	//UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	//if (nullptr != HelloUnrealFunction)
	//{
	//	USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	//}
	//// ������Ƽ �ý����� Ȱ���� �Լ� ȣ�� ���
#pragma endregion Property

#pragma region Interface
	USPigeon* Pigeon1 = NewObject<USPigeon>();
	ISFlyable* Bird1 = Cast<ISFlyable>(Pigeon1);
	// ��ó�� ��κ� �������̽� ������ ��ĳ������ �ϱ� ���� �����

	if(nullptr != Bird1)
	Bird1->Fly();

#pragma endregion Interface

#pragma region Serialization
	// Src
	FBirdData SrcRawData(TEXT("Pigeon17"), 17);
	UE_LOG(LogTemp, Log, TEXT("[SrcRawData] Name: %s, ID: %d"), *SrcRawData.Name, SrcRawData.ID);

	const FString SavedDir = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));// ���� ��ο� Saved ������ �߰� 
	UE_LOG(LogTemp, Log, TEXT("SavedDir: %s"), *SavedDir);

	const FString RawDataFileName(TEXT("RawData.bin"));// ����ȭ�ؼ� ������ ���� �̸�
	FString AbsolutePathForRawData = FPaths::Combine(*SavedDir, *RawDataFileName);// SavedDir�� RawDataFileName�� �ٿ��� ���� ���� ����� AbsolutePathForRawData ����
	UE_LOG(LogTemp, Log, TEXT("Relative path for saved file: %s"), *AbsolutePathForRawData);
	FPaths::MakeStandardFilename(AbsolutePathForRawData);// �𸮾� �°� ���� �̸� ����
	UE_LOG(LogTemp, Log, TEXT("Absolute path for saved file: %s"), *AbsolutePathForRawData);

	FArchive* RawFileWriterAr = IFileManager::Get().CreateFileWriter(*AbsolutePathForRawData);
	if (nullptr != RawFileWriterAr)
	{
		*RawFileWriterAr << SrcRawData;//RawFileWriterAr�� SrcRawData�� �Ѱ���
		RawFileWriterAr->Close();
		delete RawFileWriterAr;
		RawFileWriterAr = nullptr;
	}

	// Dst
	FBirdData DstRawData;
	FArchive* RawFileReaderAr = IFileManager::Get().CreateFileReader(*AbsolutePathForRawData);
	if (nullptr != RawFileReaderAr)
	{
		*RawFileReaderAr << DstRawData;//RawFileReaderAr���� DstRawData�� ���
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
	FString AbsolutePathForObjectData = FPaths::Combine(*SavedDir, *ObjectDataFileName);// ������ �ߴ� ��ó�� SavedDir�� �����̸� �ٿ��� ���� ���� ��� ����
	FPaths::MakeStandardFilename(AbsolutePathForObjectData);// �𸮾� �°� ���� �̸� ����

	TArray<uint8> BufferArray;// �ۼ��ϱ� ���� uint8 1����Ʈ
	FMemoryWriter MemoryWriterAr(BufferArray);
	SerializedPigeon->Serialize(MemoryWriterAr);

	TUniquePtr<FArchive> ObjectDataFileWriterAr = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*AbsolutePathForObjectData));
	if (nullptr != ObjectDataFileWriterAr)
	{
		*ObjectDataFileWriterAr << BufferArray;
		ObjectDataFileWriterAr->Close();

		ObjectDataFileWriterAr = nullptr; //delete ObjectDataFileWriterAr; �� ���� ȿ��.
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
	// ����
	const FString JsonDataFileName(TEXT("StudyJsonFile.txt"));
	FString AbsolutePathForJsonData = FPaths::Combine(*SavedDir, *JsonDataFileName);
	FPaths::MakeStandardFilename(AbsolutePathForJsonData);

	TSharedRef<FJsonObject> SrcJsonObject = MakeShared<FJsonObject>();// FJsonObject�� UObject ����, FJsonValue�� �� ���� 
	FJsonObjectConverter::UStructToJsonObject(SerializedPigeon->GetClass(), SerializedPigeon, SrcJsonObject);

	FString JsonOutString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriterAr = TJsonWriterFactory<TCHAR>::Create(&JsonOutString);// �������� ���� TSharedRef ���
	if (true == FJsonSerializer::Serialize(SrcJsonObject, JsonWriterAr))
	{
		FFileHelper::SaveStringToFile(JsonOutString, *AbsolutePathForJsonData);
	}

	// �б�
	FString JsonInString;
	FFileHelper::LoadFileToString(JsonInString, *AbsolutePathForJsonData);
	TSharedRef<TJsonReader<TCHAR>> JsonReaderAr = TJsonReaderFactory<TCHAR>::Create(JsonInString);

	TSharedPtr<FJsonObject> DstJsonObject;
	if (true == FJsonSerializer::Deserialize(JsonReaderAr, DstJsonObject))// Deserialize �ؼ� DstJsonObject�� ������ �ϴ� ��
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
