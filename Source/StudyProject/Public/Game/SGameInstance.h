// SGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "SGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSStatTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    FSStatTableRow()
    {
    }

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxEXP;

};

/**
 *
 */
UCLASS()
class STUDYPROJECT_API USGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    virtual void Shutdown() override;

    const UDataTable* GetCharacterStatDataTable() { return CharacterStatDataTable; }

    FSStatTableRow* USGameInstance::GetCharacterStatDataTableRow(int32 InLevel);
    // InLevel�� Name�� �ǹ�

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "USGameInstance", Meta = (AllowPrivateAccess))
    class UDataTable* CharacterStatDataTable;

};
