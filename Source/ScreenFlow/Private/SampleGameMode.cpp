#include "SampleGameMode.h"
#include "SamplePlayerController.h"
#include "SampleScreenWidget.h"

ASampleGameMode::ASampleGameMode()
{
    DefaultPawnClass = nullptr;
    PlayerControllerClass = ASamplePlayerController::StaticClass();
    PauseWidgetClass = TSoftClassPtr<USampleScreenWidget>(FSoftObjectPath(TEXT("/Game/UI/Common/WBP_PauseMenu.WBP_PauseMenu_C")));
    ConfirmWidgetClass = TSoftClassPtr<USampleScreenWidget>(FSoftObjectPath(TEXT("/Game/UI/Common/WBP_ConfirmDialog.WBP_ConfirmDialog_C")));
}

ASampleOpeningGameMode::ASampleOpeningGameMode()
{
    Screen = ESampleScreen::Opening;
    ScreenWidgetClass = TSoftClassPtr<USampleScreenWidget>(FSoftObjectPath(TEXT("/Game/UI/Opening/WBP_Opening.WBP_Opening_C")));
}

ASampleTitleGameMode::ASampleTitleGameMode()
{
    Screen = ESampleScreen::MainTitle;
    ScreenWidgetClass = TSoftClassPtr<USampleScreenWidget>(FSoftObjectPath(TEXT("/Game/UI/MainTitle/WBP_MainTitle.WBP_MainTitle_C")));
}

ASampleGameGameMode::ASampleGameGameMode()
{
    Screen = ESampleScreen::Game;
    ScreenWidgetClass = TSoftClassPtr<USampleScreenWidget>(FSoftObjectPath(TEXT("/Game/UI/Game/WBP_GameHUD.WBP_GameHUD_C")));
}

ASampleEndingGameMode::ASampleEndingGameMode()
{
    Screen = ESampleScreen::Ending;
    ScreenWidgetClass = TSoftClassPtr<USampleScreenWidget>(FSoftObjectPath(TEXT("/Game/UI/Ending/WBP_Ending.WBP_Ending_C")));
}
