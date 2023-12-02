// SW_Bar.h

#pragma once

#include "CoreMinimal.h"
#include "UI/StudyUserWidget.h"
#include "SW_Bar.generated.h"

/**
 *
 */
UCLASS()
class STUDYPROJECT_API USW_Bar : public UStudyUserWidget
{
    GENERATED_BODY()

public:
    USW_Bar(const FObjectInitializer& ObjectInitializer);
    // ���� Widget Ŭ�������� �����ڿ� ������ �ۼ��Ѵٸ� �� �� �����ڸ� ����/�����������.

    void SetMaxFigure(float InMaxFigure);

protected:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USW_Bar")
    TObjectPtr<class UProgressBar> Bar;// UI ���

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "USW_Bar")
    float MaxFigure;

};