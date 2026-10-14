#include "GameController.h"

#include "SampleConfirmDialogWidget.h"
#include "SampleGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "Components/Widget.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AGameController::AGameController()
{
    GameWidgetClass = TSoftClassPtr<UUserWidget>(
        FSoftObjectPath(TEXT("/Game/UI/Game/WBP_GameHUD.WBP_GameHUD_C")));
    PauseWidgetClass = TSoftClassPtr<UUserWidget>(
        FSoftObjectPath(TEXT("/Game/UI/Common/WBP_PauseMenu.WBP_PauseMenu_C")));
    ConfirmWidgetClass = TSoftClassPtr<USampleConfirmDialogWidget>(
        FSoftObjectPath(TEXT("/Game/UI/Common/WBP_ConfirmDialog.WBP_ConfirmDialog_C")));
}

void AGameController::BeginPlay()
{
    Super::BeginPlay();

    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
    {
        Instance->NotifyScreenReady(ESampleScreen::Game);
    }

    bShowMouseCursor = true;
    if (UClass* Class = GameWidgetClass.LoadSynchronous())
    {
        GameWidget = CreateWidget<UUserWidget>(this, Class);
        if (GameWidget) GameWidget->AddToViewport();
    }
    FocusTopWidget();
}

void AGameController::SetupInputComponent()
{
    Super::SetupInputComponent();
    FInputKeyBinding& Escape = InputComponent->BindKey(
        EKeys::Escape, IE_Pressed, this, &AGameController::HandleEscape);
    Escape.bExecuteWhenPaused = true;
}

void AGameController::HandleEscape()
{
    // Exactly one overlay is handled per Esc press: Confirm > Pause > Game.
    if (ConfirmWidget)
    {
        HandleConfirmCancelled();
        return;
    }
    if (PauseWidget)
    {
        ResumeGame();
        return;
    }
    ShowPauseMenu();
}

void AGameController::ShowPauseMenu()
{
    if (PauseWidget || ConfirmWidget) return;

    UClass* Class = PauseWidgetClass.LoadSynchronous();
    if (!Class) return;

    PauseWidget = CreateWidget<UUserWidget>(this, Class);
    if (!PauseWidget) return;

    PauseWidget->AddToViewport(100);
    if (!UGameplayStatics::SetGamePaused(this, true))
    {
        PauseWidget->RemoveFromParent();
        PauseWidget = nullptr;
        return;
    }
    FocusTopWidget();
}

void AGameController::ResumeGame()
{
    if (!PauseWidget || ConfirmWidget) return;

    UGameplayStatics::SetGamePaused(this, false);
    PauseWidget->RemoveFromParent();
    PauseWidget = nullptr;
    FocusTopWidget();
}

void AGameController::RequestReturnToTitle()
{
    if (PauseWidget && !ConfirmWidget)
    {
        ShowConfirmDialog(ESampleConfirmAction::ReturnToTitle);
    }
}

void AGameController::RequestQuitGame()
{
    if (PauseWidget && !ConfirmWidget)
    {
        ShowConfirmDialog(ESampleConfirmAction::QuitGame);
    }
}

void AGameController::ShowConfirmDialog(ESampleConfirmAction Action)
{
    if (ConfirmWidget) return;

    UClass* Class = ConfirmWidgetClass.LoadSynchronous();
    if (!Class) return;

    ConfirmWidget = CreateWidget<USampleConfirmDialogWidget>(this, Class);
    if (!ConfirmWidget) return;

    PendingConfirm = Action;
    ConfirmWidget->OnConfirmed.AddUniqueDynamic(this, &AGameController::HandleConfirmAccepted);
    ConfirmWidget->OnCancelled.AddUniqueDynamic(this, &AGameController::HandleConfirmCancelled);
    ConfirmWidget->SetConfirmMessage(Action == ESampleConfirmAction::ReturnToTitle
        ? NSLOCTEXT("ScreenFlow", "ReturnToTitle", "Return to title?")
        : NSLOCTEXT("ScreenFlow", "ExitGame", "Exit the game?"));
    ConfirmWidget->AddToViewport(200);
    FocusTopWidget();
}

void AGameController::HandleConfirmAccepted()
{
    if (!ConfirmWidget) return;

    const ESampleConfirmAction Action = PendingConfirm;
    if (Action == ESampleConfirmAction::ReturnToTitle)
    {
        UGameplayStatics::SetGamePaused(this, false);
        if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
        {
            Instance->GoToScreen(ESampleScreen::MainTitle);
        }
    }
    else if (Action == ESampleConfirmAction::QuitGame)
    {
        UGameplayStatics::SetGamePaused(this, false);
        UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
    }
}

void AGameController::HandleConfirmCancelled()
{
    if (!ConfirmWidget) return;

    ConfirmWidget->RemoveFromParent();
    ConfirmWidget = nullptr;
    PendingConfirm = ESampleConfirmAction::None;
    // Do not unpause: the original Confirm Cancel returns to the paused game.
    FocusTopWidget();
}

void AGameController::FocusTopWidget()
{
    if (GameWidget) GameWidget->SetIsEnabled(!PauseWidget && !ConfirmWidget);
    if (PauseWidget) PauseWidget->SetIsEnabled(!ConfirmWidget);

    UUserWidget* Top = ConfirmWidget
        ? static_cast<UUserWidget*>(ConfirmWidget.Get())
        : (PauseWidget ? PauseWidget.Get() : GameWidget.Get());

    FInputModeGameAndUI Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    Mode.SetHideCursorDuringCapture(false);

    if (Top)
    {
        const FName ButtonName = ConfirmWidget ? FName(TEXT("Btn_No"))
            : (PauseWidget ? FName(TEXT("Btn_Resume")) : FName(TEXT("Btn_GameClear")));
        if (UWidget* Focus = Top->GetWidgetFromName(ButtonName))
        {
            Mode.SetWidgetToFocus(Focus->TakeWidget());
            SetInputMode(Mode);
            Focus->SetUserFocus(this);
            return;
        }
    }
    SetInputMode(Mode);
}

void AGameController::CleanupUI()
{
    if (ConfirmWidget) ConfirmWidget->RemoveFromParent();
    if (PauseWidget) PauseWidget->RemoveFromParent();
    if (GameWidget) GameWidget->RemoveFromParent();

    ConfirmWidget = nullptr;
    PauseWidget = nullptr;
    GameWidget = nullptr;
    PendingConfirm = ESampleConfirmAction::None;
}

void AGameController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupUI();
    Super::EndPlay(EndPlayReason);
}
