#include "TitleController.h"

#include "SampleConfirmDialogWidget.h"
#include "SampleGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/InputComponent.h"
#include "Components/Widget.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"

ATitleController::ATitleController()
{
    TitleWidgetClass = TSoftClassPtr<UUserWidget>(
        FSoftObjectPath(TEXT("/Game/UI/MainTitle/WBP_MainTitle.WBP_MainTitle_C")));
    ConfirmWidgetClass = TSoftClassPtr<USampleConfirmDialogWidget>(
        FSoftObjectPath(TEXT("/Game/UI/Common/WBP_ConfirmDialog.WBP_ConfirmDialog_C")));
}

void ATitleController::BeginPlay()
{
    Super::BeginPlay();

    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
    {
        Instance->NotifyScreenReady(ESampleScreen::MainTitle);
    }

    bShowMouseCursor = true;
    if (UClass* Class = TitleWidgetClass.LoadSynchronous())
    {
        TitleWidget = CreateWidget<UUserWidget>(this, Class);
        if (TitleWidget) TitleWidget->AddToViewport();
    }
    FocusTopWidget();
}

void ATitleController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ATitleController::HandleEscape);
}

void ATitleController::HandleEscape()
{
    if (ConfirmWidget)
    {
        HandleExitCancelled();
    }
    else
    {
        RequestExitGame();
    }
}

void ATitleController::RequestExitGame()
{
    if (ConfirmWidget) return;

    UClass* Class = ConfirmWidgetClass.LoadSynchronous();
    if (!Class) return;

    ConfirmWidget = CreateWidget<USampleConfirmDialogWidget>(this, Class);
    if (!ConfirmWidget) return;

    ConfirmWidget->OnConfirmed.AddUniqueDynamic(this, &ATitleController::HandleExitConfirmed);
    ConfirmWidget->OnCancelled.AddUniqueDynamic(this, &ATitleController::HandleExitCancelled);
    ConfirmWidget->SetConfirmMessage(
        NSLOCTEXT("ScreenFlow", "ExitGame", "Exit the game?"));
    ConfirmWidget->AddToViewport(200);
    FocusTopWidget();
}

void ATitleController::HandleExitConfirmed()
{
    UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}

void ATitleController::HandleExitCancelled()
{
    if (!ConfirmWidget) return;
    ConfirmWidget->RemoveFromParent();
    ConfirmWidget = nullptr;
    FocusTopWidget();
}

void ATitleController::FocusTopWidget()
{
    if (TitleWidget) TitleWidget->SetIsEnabled(!ConfirmWidget);

    UUserWidget* Top = ConfirmWidget
        ? static_cast<UUserWidget*>(ConfirmWidget.Get())
        : TitleWidget.Get();

    FInputModeGameAndUI Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    Mode.SetHideCursorDuringCapture(false);

    if (Top)
    {
        const FName ButtonName = ConfirmWidget ? TEXT("Btn_No") : TEXT("Btn_Start");
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

void ATitleController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (ConfirmWidget) ConfirmWidget->RemoveFromParent();
    if (TitleWidget) TitleWidget->RemoveFromParent();
    ConfirmWidget = nullptr;
    TitleWidget = nullptr;
    Super::EndPlay(EndPlayReason);
}
