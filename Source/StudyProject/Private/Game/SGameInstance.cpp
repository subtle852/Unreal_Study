// Game/SGameInstance.cpp


#include "Game/SGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"// PrintString�� ����ϱ� ���� �������

USGameInstance::USGameInstance()
{
	UE_LOG(LogTemp, Log, TEXT("USGameInstance() has been called."));
}

void USGameInstance::Init()
{
	Super::Init();
	// virual�� ������� �ڽ� Ŭ�������� �θ� Ŭ������ ��Ī�� ��, Super�� ���

	UE_LOG(LogTemp, Log, TEXT("Init() has been called by UE_LOG."));

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Init() has been called by PrintString."));
	//GetWorld()�� UObject(���� ��ü)�� �����ϴ� �� 

	// UE_LOG�� OutputLog���� Ȯ���� �����ϴ� (���� ���α׷��ӵ��� �ַ� ���)
	// PrintString�� ViewPort���� Ȯ���� �����ϴ� (Ŭ�� ���α׷��ӵ��� �ַ� ���)

	// �ش� �Լ����� ����������
	// ������� ������ �ּ� ó���� ���ִ� ���� �⺻
}

void USGameInstance::Shutdown()
{
	Super::Shutdown();

	UE_LOG(LogTemp, Log, TEXT("Shutdown() has been called."));

}
