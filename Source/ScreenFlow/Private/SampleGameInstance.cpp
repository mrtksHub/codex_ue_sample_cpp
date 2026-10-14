#include "SampleGameInstance.h"

#include "Kismet/GameplayStatics.h"

FName USampleGameInstance::MapPackage(ESampleScreen Screen)
{
    switch (Screen)
    {
    case ESampleScreen::Opening:   return TEXT("/Game/Maps/L_Opening");
    case ESampleScreen::MainTitle: return TEXT("/Game/Maps/L_MainTitle");
    case ESampleScreen::Game:      return TEXT("/Game/Maps/L_Game");
    case ESampleScreen::Ending:    return TEXT("/Game/Maps/L_Ending");
    default:                       return NAME_None;
    }
}

void USampleGameInstance::GoToScreen(ESampleScreen Target)
{
    if (bTransitionInProgress)
    {
        return;
    }

    const FName LevelName = MapPackage(Target);
    if (LevelName.IsNone())
    {
        return;
    }

    bTransitionInProgress = true;
    UGameplayStatics::OpenLevel(this, LevelName);
}

void USampleGameInstance::NotifyScreenReady(ESampleScreen Screen)
{
    GameState = Screen;
    bTransitionInProgress = false;
}
