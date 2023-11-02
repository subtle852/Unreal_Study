// SFlyable.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SFlyable.generated.h"

USTRUCT()
struct FBirdData
{
    GENERATED_BODY()

public:
    FBirdData() {}

    FBirdData(const FString& InName, int32 InID)
        : Name(InName)
        , ID(InID)
    {
    }

    friend FArchive& operator<<(FArchive& Ar, FBirdData& InBirdData)
    {
        Ar << InBirdData.Name;
        Ar << InBirdData.ID;
        return Ar;
    }

    // TMap을 사용하기 위해 작성해야 하는 비교 연산자
    bool operator==(const FBirdData& InBirdData) const
    {
        return ID == InBirdData.ID;
    }

    // TMap을 사용하기 위해 작성해야 하는 GetTypeHash
    friend uint32 GetTypeHash(const FBirdData& InBirdData)
    {
        return GetTypeHash(InBirdData.ID);
    }

    UPROPERTY()
    FString Name = TEXT("DefaultBirdName");

    UPROPERTY()
    int32 ID = 0;

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USFlyable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STUDYPROJECT_API ISFlyable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:
	virtual void Fly() = 0;// 일반적으로 IsFlyable에서 구현을 하지 않음 

};
