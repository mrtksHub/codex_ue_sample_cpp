#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitleController.generated.h"

class UUserWidget;
class USampleConfirmDialogWidget;

// Native counterpart of BP_TitleController.
UCLASS()
class SCREENFLOW_API ATitleController : public APlayerController
{
    GENERATED_BODY()
public:
    ATitleController();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupInputComponent() override;

    // Invoked by WBP_MainTitle.Btn_ExitGame.OnClicked.
    UFUNCTION(BlueprintCallable, Category="Sample|Title")
    void RequestExitGame();

private:
    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<UUserWidget> TitleWidgetClass;
    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<USampleConfirmDialogWidget> ConfirmWidgetClass;

    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> TitleWidget;
    UPROPERTY(Transient)
    TObjectPtr<USampleConfirmDialogWidget> ConfirmWidget;

    void HandleEscape();
    void FocusTopWidget();

    UFUNCTION()
    void HandleExitConfirmed();
    UFUNCTION()
    void HandleExitCancelled();
};
