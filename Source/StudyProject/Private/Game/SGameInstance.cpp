// Game/SGameInstance.cpp


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"// PrintString�� ����ϱ� ���� �������
#include "SUnrealObjectClass.h"

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

	USUnrealObjectClass* USObject1 = NewObject<USUnrealObjectClass>();
	// �𸮾��� new Ű���尡 �ƴ� NewObject<>() API�� �����

	// 1) ���� �ǽ�

	UE_LOG(LogTemp, Log, TEXT("USObject1's Name: %s"), *USObject1->GetName());
	// Ŭ�������� ���� �������� Getter()�� Ȱ���� �Ϲ����� FString ���

	FProperty* NameProperty = USUnrealObjectClass::StaticClass()->FindPropertyByName(TEXT("Name"));

	FString CompiletimeUSObjectName;
	if (nullptr != NameProperty)
	{
		NameProperty->GetValue_InContainer(USObject1, &CompiletimeUSObjectName);
		UE_LOG(LogTemp, Log, TEXT("CompiletimeUSObjectName: %s"), *CompiletimeUSObjectName);
	}
	// ������Ƽ �ý����� Getter()�� Ȱ���� FString ���

	// 2) �Լ� �ǽ�

	USObject1->HelloUnreal();
	// �Ϲ����� �Լ� ȣ�� ���

	UFunction* HelloUnrealFunction = USObject1->GetClass()->FindFunctionByName(TEXT("HelloUnreal"));
	if (nullptr != HelloUnrealFunction)
	{
		USObject1->ProcessEvent(HelloUnrealFunction, nullptr);
	}
	// ������Ƽ �ý����� Ȱ���� �Լ� ȣ�� ���
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("Shutdown() has been called."));

}
