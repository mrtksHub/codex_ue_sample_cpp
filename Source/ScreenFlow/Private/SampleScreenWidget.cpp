#include "SampleScreenWidget.h"
#include "SampleGameInstance.h"
#include "SamplePlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

USampleScreenWidget::USampleScreenWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void USampleScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ASamplePlayerController* Controller = Cast<ASamplePlayerController>(GetOwningPlayer()))
    {
        if (Btn_Start) Btn_Start->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::StartGame);
        if (Btn_ExitGame) Btn_ExitGame->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::RequestQuitGame);
        if (Btn_GameClear) Btn_GameClear->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::ClearGame);
        if (Btn_Finish) Btn_Finish->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::FinishEnding);
        if (Btn_Resume) Btn_Resume->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::ResumeGame);
        if (Btn_ReturnToTitle) Btn_ReturnToTitle->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::RequestReturnToTitle);
        if (Btn_Yes) Btn_Yes->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::ConfirmYes);
        if (Btn_No) Btn_No->OnClicked.AddUniqueDynamic(Controller, &ASamplePlayerController::ConfirmNo);
    }
    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
    {
        Instance->OnDataChanged.AddUniqueDynamic(this, &USampleScreenWidget::RefreshFromModel);
        RefreshFromModel();
    }
}

void USampleScreenWidget::NativeDestruct()
{
    if (USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance()))
        Instance->OnDataChanged.RemoveDynamic(this, &USampleScreenWidget::RefreshFromModel);
    Super::NativeDestruct();
}

void USampleScreenWidget::RefreshFromModel()
{
    USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance());
    if (!Instance) return;
    if (Text_CurrentScreen)
        Text_CurrentScreen->SetText(FText::FromString(StaticEnum<ESampleScreen>()->GetNameStringByValue(static_cast<int64>(Instance->CurrentScreen))));
    if (Text_PlayerName) Text_PlayerName->SetText(FText::FromString(Instance->PlayerData.PlayerName));
    if (Text_PlayerLevel) Text_PlayerLevel->SetText(FText::AsNumber(Instance->PlayerData.PlayerLevel));
    if (Text_CurrentChapter) Text_CurrentChapter->SetText(FText::AsNumber(Instance->CurrentChapter));
    OnModelChanged(Instance);
}

UWidget* USampleScreenWidget::GetInitialFocusTarget_Implementation()
{
    // Prefer Cancel in a destructive-action confirmation.
    if (Btn_No) return Btn_No;
    if (Btn_Resume) return Btn_Resume;
    if (Btn_Start) return Btn_Start;
    if (Btn_GameClear) return Btn_GameClear;
    if (Btn_Finish) return Btn_Finish;
    return this;
}

void USampleScreenWidget::SetConfirmMessage(const FText& Message)
{
    if (Text_Message) Text_Message->SetText(Message);
}

FReply USampleScreenWidget::NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent)
{
    if (ASamplePlayerController* Controller = Cast<ASamplePlayerController>(GetOwningPlayer()))
        if (Controller->HandleUIKey(KeyEvent.GetKey(), KeyEvent.IsRepeat())) return FReply::Handled();
    return Super::NativeOnPreviewKeyDown(Geometry, KeyEvent);
}

FReply USampleScreenWidget::NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
    if (ASamplePlayerController* Controller = Cast<ASamplePlayerController>(GetOwningPlayer()))
    {
        const USampleGameInstance* Instance = Cast<USampleGameInstance>(GetGameInstance());
        if (Instance && Instance->CurrentScreen == ESampleScreen::Opening &&
            Controller->HandleUIKey(MouseEvent.GetEffectingButton(), false)) return FReply::Handled();
    }
    return Super::NativeOnPreviewMouseButtonDown(Geometry, MouseEvent);
}
