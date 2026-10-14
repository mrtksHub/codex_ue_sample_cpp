#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EndingController.generated.h"

class UUserWidget;

// Native counterpart of BP_EndingController.
UCLASS()
class SCREENFLOW_API AEndingController : public APlayerController
{
    GENERATED_BODY()
public:
    AEndingController();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<UUserWidget> EndingWidgetClass;
    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> EndingWidget;
};
