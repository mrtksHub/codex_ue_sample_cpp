#include "OpeningController.h"

#include "SampleGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

AOpeningController::AOpeningController()
{
    OpeningWidgetClass = TSoftClassPtr<UUserWidget>(
        FSoftObjectPath(TEXT("/Game/UI/Opening/WBP_Opening.WBP_Opening_C")));
}

void AOpeningController::BeginPlay()
{
    Super::BeginPlay();

    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
    {
        Instance->NotifyScreenReady(ESampleScreen::Opening);
    }

    bShowMouseCursor = true;
    if (UClass* Class = OpeningWidgetClass.LoadSynchronous())
    {
        OpeningWidget = CreateWidget<UUserWidget>(this, Class);
        if (OpeningWidget) OpeningWidget->AddToViewport();
    }

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}

void AOpeningController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::AnyKey, IE_Pressed, this, &AOpeningController::HandleAnyKey);
}

void AOpeningController::HandleAnyKey()
{
    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
    {
        Instance->GoToScreen(ESampleScreen::MainTitle);
    }
}

void AOpeningController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (OpeningWidget)
    {
        OpeningWidget->RemoveFromParent();
        OpeningWidget = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}
