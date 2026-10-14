#include "SamplePlayerController.h"
#include "SampleGameInstance.h"
#include "SampleGameMode.h"
#include "SampleScreenWidget.h"
#include "Components/InputComponent.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

USampleGameInstance* ASamplePlayerController::Model() const
{
    return Cast<USampleGameInstance>(GetGameInstance());
}

void ASamplePlayerController::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = true;
    const ASampleGameMode* Mode = GetWorld()->GetAuthGameMode<ASampleGameMode>();
    USampleGameInstance* Instance = Model();
    if (!Mode || !Instance || !Instance->NotifyScreenReady(Mode->Screen))
    {
        UE_LOG(LogTemp, Error, TEXT("ScreenFlow: check GameInstance and map GameMode settings"));
        return;
    }
    UGameplayStatics::SetGamePaused(this, false);
    Instance->OnDataChanged.AddUniqueDynamic(this, &ASamplePlayerController::SyncFlowAvailability);
    ScreenWidget = CreateView(Mode->ScreenWidgetClass, 0);
    FocusTopWidget();
}

void ASamplePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (USampleGameInstance* Instance = Model())
        Instance->OnDataChanged.RemoveDynamic(this, &ASamplePlayerController::SyncFlowAvailability);
    CleanupUI();
    Super::EndPlay(EndPlayReason);
}

void ASamplePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    auto& Escape = InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ASamplePlayerController::HandleEscape);
    Escape.bExecuteWhenPaused = true;
    Escape.bConsumeInput = true;
    auto& AnyKey = InputComponent->BindKey(EKeys::AnyKey, IE_Pressed, this, &ASamplePlayerController::HandleAnyKey);
    AnyKey.bExecuteWhenPaused = true;
}

USampleScreenWidget* ASamplePlayerController::CreateView(TSoftClassPtr<USampleScreenWidget> Class, int32 ZOrder)
{
    UClass* Loaded = Class.LoadSynchronous();
    if (!Loaded)
    {
        UE_LOG(LogTemp, Error, TEXT("ScreenFlow: missing Widget Blueprint %s; see docs/EDITOR_SETUP.md"), *Class.ToString());
        return nullptr;
    }
    USampleScreenWidget* Widget = CreateWidget<USampleScreenWidget>(this, Loaded);
    if (Widget) Widget->AddToViewport(ZOrder);
    return Widget;
}

bool ASamplePlayerController::CanUseScreen(ESampleScreen Screen) const
{
    const USampleGameInstance* Instance = Model();
    return Instance && ScreenWidget && !Instance->bTransitionInProgress &&
        Instance->CurrentScreen == Screen && !PauseWidget && !ConfirmWidget;
}

bool ASamplePlayerController::TravelTo(ESampleScreen Target)
{
    USampleGameInstance* Instance = Model();
    if (!Instance || !Instance->GoToScreen(Target)) return false;
    // Retain outgoing views until EndPlay so failed travel can restore them.
    SyncFlowAvailability();
    return true;
}

void ASamplePlayerController::HandleAnyKey()
{
    if (CanUseScreen(ESampleScreen::Opening)) TravelTo(ESampleScreen::MainTitle);
}

bool ASamplePlayerController::HandleUIKey(const FKey& Key, bool bRepeat)
{
    if (CanUseScreen(ESampleScreen::Opening))
    {
        if (!bRepeat) HandleAnyKey();
        return true;
    }
    if (Key == EKeys::Escape)
    {
        if (!bRepeat) HandleEscape();
        return true;
    }
    return false;
}

void ASamplePlayerController::StartGame()
{
    if (CanUseScreen(ESampleScreen::MainTitle)) TravelTo(ESampleScreen::Game);
}

void ASamplePlayerController::ClearGame()
{
    if (CanUseScreen(ESampleScreen::Game)) TravelTo(ESampleScreen::Ending);
}

void ASamplePlayerController::FinishEnding()
{
    if (CanUseScreen(ESampleScreen::Ending)) TravelTo(ESampleScreen::MainTitle);
}

void ASamplePlayerController::HandleEscape()
{
    if (!Model() || Model()->bTransitionInProgress || !ScreenWidget) return;
    if (ConfirmWidget) { ConfirmNo(); return; }
    if (PauseWidget) { ResumeGame(); return; }
    switch (Model()->CurrentScreen)
    {
    case ESampleScreen::Opening: HandleAnyKey(); break;
    case ESampleScreen::MainTitle: RequestQuitGame(); break;
    case ESampleScreen::Game: OpenPauseMenu(); break;
    default: break;
    }
}

void ASamplePlayerController::OpenPauseMenu()
{
    if (!CanUseScreen(ESampleScreen::Game)) return;
    const ASampleGameMode* Mode = GetWorld()->GetAuthGameMode<ASampleGameMode>();
    if (!Mode) return;
    PauseWidget = CreateView(Mode->PauseWidgetClass, 100);
    if (!PauseWidget) return;
    if (!UGameplayStatics::SetGamePaused(this, true))
    {
        PauseWidget->RemoveFromParent();
        PauseWidget = nullptr;
        return;
    }
    FocusTopWidget();
}

void ASamplePlayerController::ResumeGame()
{
    if (!PauseWidget || ConfirmWidget) return;
    if (!UGameplayStatics::SetGamePaused(this, false)) return;
    PauseWidget->RemoveFromParent();
    PauseWidget = nullptr;
    FocusTopWidget();
}

void ASamplePlayerController::RequestReturnToTitle()
{
    if (Model() && Model()->CurrentScreen == ESampleScreen::Game && PauseWidget && !ConfirmWidget)
        OpenConfirm(ESampleConfirmAction::ReturnToTitle);
}

void ASamplePlayerController::RequestQuitGame()
{
    if (CanUseScreen(ESampleScreen::MainTitle) ||
        (Model() && Model()->CurrentScreen == ESampleScreen::Game && PauseWidget && !ConfirmWidget))
        OpenConfirm(ESampleConfirmAction::QuitGame);
}

void ASamplePlayerController::OpenConfirm(ESampleConfirmAction Action)
{
    if (ConfirmWidget || !Model() || Model()->bTransitionInProgress) return;
    const ASampleGameMode* Mode = GetWorld()->GetAuthGameMode<ASampleGameMode>();
    if (!Mode) return;
    ConfirmWidget = CreateView(Mode->ConfirmWidgetClass, 200);
    if (!ConfirmWidget) return;
    PendingConfirm = Action;
    ConfirmWidget->SetConfirmMessage(Action == ESampleConfirmAction::ReturnToTitle
        ? NSLOCTEXT("ScreenFlow", "ReturnToTitle", "Return to title?")
        : NSLOCTEXT("ScreenFlow", "ExitGame", "Exit the game?"));
    FocusTopWidget();
}

void ASamplePlayerController::ConfirmNo()
{
    if (!ConfirmWidget) return;
    ConfirmWidget->RemoveFromParent();
    ConfirmWidget = nullptr;
    PendingConfirm = ESampleConfirmAction::None;
    FocusTopWidget();
}

void ASamplePlayerController::ConfirmYes()
{
    if (!ConfirmWidget || !Model() || Model()->bTransitionInProgress) return;
    if (PendingConfirm == ESampleConfirmAction::ReturnToTitle)
    {
        // Rejected travel leaves the dialog and the pause state intact.
        TravelTo(ESampleScreen::MainTitle);
    }
    else if (PendingConfirm == ESampleConfirmAction::QuitGame)
    {
        UGameplayStatics::SetGamePaused(this, false);
        // Keep the screen for platforms/PIE where QuitGame may be ignored.
        ConfirmNo();
        if (PauseWidget) ResumeGame();
        UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
    }
}

void ASamplePlayerController::FocusTopWidget()
{
    const bool bLocked = !Model() || Model()->bTransitionInProgress;
    if (ScreenWidget) ScreenWidget->SetIsEnabled(!bLocked && !PauseWidget && !ConfirmWidget);
    if (PauseWidget) PauseWidget->SetIsEnabled(!bLocked && !ConfirmWidget);
    if (ConfirmWidget) ConfirmWidget->SetIsEnabled(!bLocked);
    if (bLocked) return;
    USampleScreenWidget* Top = ConfirmWidget ? ConfirmWidget.Get() : (PauseWidget ? PauseWidget.Get() : ScreenWidget.Get());
    if (!Top) return;
    UWidget* Focus = Top->GetInitialFocusTarget();
    if (!Focus) Focus = Top;
    FInputModeGameAndUI Mode;
    Mode.SetWidgetToFocus(Focus->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    Mode.SetHideCursorDuringCapture(false);
    SetInputMode(Mode);
    Focus->SetUserFocus(this);
}

void ASamplePlayerController::SyncFlowAvailability()
{
    if (!Model() || Model()->bTransitionInProgress)
    {
        FocusTopWidget();
        return;
    }
    // A failed travel must also restore the originating pause state.
    if (PauseWidget) UGameplayStatics::SetGamePaused(this, true);
    FocusTopWidget();
}

void ASamplePlayerController::CleanupUI()
{
    if (ConfirmWidget) ConfirmWidget->RemoveFromParent();
    if (PauseWidget) PauseWidget->RemoveFromParent();
    if (ScreenWidget) ScreenWidget->RemoveFromParent();
    ConfirmWidget = nullptr;
    PauseWidget = nullptr;
    ScreenWidget = nullptr;
    PendingConfirm = ESampleConfirmAction::None;
}
