#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SampleTypes.h"
#include "SamplePlayerController.generated.h"

class USampleScreenWidget;
class USampleGameInstance;

UCLASS(Blueprintable)
class SCREENFLOW_API ASamplePlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupInputComponent() override;

    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void StartGame();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void ClearGame();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void FinishEnding();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void ResumeGame();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void RequestReturnToTitle();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void RequestQuitGame();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void ConfirmYes();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void ConfirmNo();
    UFUNCTION(BlueprintCallable, Category="Sample|Commands") void HandleEscape();
    UFUNCTION(BlueprintPure, Category="Sample|State") bool IsPauseMenuOpen() const { return PauseWidget != nullptr; }
    UFUNCTION(BlueprintPure, Category="Sample|State") bool IsConfirmOpen() const { return ConfirmWidget != nullptr; }

    // Widget preview input consumes a handled key before the focused Button sees it.
    bool HandleUIKey(const FKey& Key, bool bRepeat);

private:
    UPROPERTY(Transient) TObjectPtr<USampleScreenWidget> ScreenWidget;
    UPROPERTY(Transient) TObjectPtr<USampleScreenWidget> PauseWidget;
    UPROPERTY(Transient) TObjectPtr<USampleScreenWidget> ConfirmWidget;
    ESampleConfirmAction PendingConfirm = ESampleConfirmAction::None;

    USampleGameInstance* Model() const;
    bool CanUseScreen(ESampleScreen Screen) const;
    void HandleAnyKey();
    void OpenPauseMenu();
    void OpenConfirm(ESampleConfirmAction Action);
    void FocusTopWidget();
    UFUNCTION() void SyncFlowAvailability();
    void CleanupUI();
    bool TravelTo(ESampleScreen Target);
    USampleScreenWidget* CreateView(TSoftClassPtr<USampleScreenWidget> Class, int32 ZOrder);
};
