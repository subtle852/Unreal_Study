// SW_HPBar.h

#pragma once

#include "CoreMinimal.h"
#include "UI/SW_Bar.h"
#include "SW_HPBar.generated.h"

/**
 *
 */
UCLASS()
class STUDYPROJECT_API USW_HPBar : public USW_Bar
{
    GENERATED_BODY()

public:
    void SetMaxHP(float InMaxHP);

    void InitializeHPBarWidget(class USStatComponent* NewStatComponent);

    UFUNCTION()
    void OnMaxHPChange(float InOldMaxHP, float InNewMaxHP);

    UFUNCTION()
    void OnCurrentHPChange(float InOldHP, float InNewHP);

protected:
    virtual void NativeConstruct() override;// Viewport�� Add �� ������ ȣ��

};
