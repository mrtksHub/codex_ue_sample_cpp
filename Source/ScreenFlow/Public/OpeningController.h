#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OpeningController.generated.h"

class UUserWidget;

// Native counterpart of BP_OpeningController. No shared project controller.
UCLASS()
class SCREENFLOW_API AOpeningController : public APlayerController
{
    GENERATED_BODY()
public:
    AOpeningController();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupInputComponent() override;

private:
    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<UUserWidget> OpeningWidgetClass;

    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> OpeningWidget;

    void HandleAnyKey();
};
