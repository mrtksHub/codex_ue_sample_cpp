#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SampleTypes.h"
#include "GameController.generated.h"

class UUserWidget;
class USampleConfirmDialogWidget;

// Native counterpart of BP_GameController. Handles Game-specific Pause and Confirm.
UCLASS()
class SCREENFLOW_API AGameController : public APlayerController
{
    GENERATED_BODY()
public:
    AGameController();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupInputComponent() override;

    // Called from WBP_PauseMenu button OnClicked graphs.
    UFUNCTION(BlueprintCallable, Category="Sample|Game")
    void ResumeGame();

    UFUNCTION(BlueprintCallable, Category="Sample|Game")
    void RequestReturnToTitle();

    UFUNCTION(BlueprintCallable, Category="Sample|Game")
    void RequestQuitGame();

private:
    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<UUserWidget> GameWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<UUserWidget> PauseWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="Sample|UI")
    TSoftClassPtr<USampleConfirmDialogWidget> ConfirmWidgetClass;

    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> GameWidget;
    UPROPERTY(Transient)
    TObjectPtr<UUserWidget> PauseWidget;
    UPROPERTY(Transient)
    TObjectPtr<USampleConfirmDialogWidget> ConfirmWidget;

    ESampleConfirmAction PendingConfirm = ESampleConfirmAction::None;

    void HandleEscape();
    void ShowPauseMenu();
    void ShowConfirmDialog(ESampleConfirmAction Action);
    void FocusTopWidget();
    void CleanupUI();

    UFUNCTION()
    void HandleConfirmAccepted();

    UFUNCTION()
    void HandleConfirmCancelled();
};
