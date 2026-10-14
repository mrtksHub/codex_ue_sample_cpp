#include "EndingController.h"

#include "SampleGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"

AEndingController::AEndingController()
{
    EndingWidgetClass = TSoftClassPtr<UUserWidget>(
        FSoftObjectPath(TEXT("/Game/UI/Ending/WBP_Ending.WBP_Ending_C")));
}

void AEndingController::BeginPlay()
{
    Super::BeginPlay();

    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
    {
        Instance->NotifyScreenReady(ESampleScreen::Ending);
    }

    bShowMouseCursor = true;
    if (UClass* Class = EndingWidgetClass.LoadSynchronous())
    {
        EndingWidget = CreateWidget<UUserWidget>(this, Class);
        if (EndingWidget) EndingWidget->AddToViewport();
    }

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    if (EndingWidget)
    {
        if (UWidget* Focus = EndingWidget->GetWidgetFromName(TEXT("Btn_Finish")))
        {
            InputMode.SetWidgetToFocus(Focus->TakeWidget());
            SetInputMode(InputMode);
            Focus->SetUserFocus(this);
            return;
        }
    }
    SetInputMode(InputMode);
}

void AEndingController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (EndingWidget)
    {
        EndingWidget->RemoveFromParent();
        EndingWidget = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}
