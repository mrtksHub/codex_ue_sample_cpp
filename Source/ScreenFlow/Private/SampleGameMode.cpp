#include "SampleGameMode.h"

#include "OpeningController.h"
#include "TitleController.h"
#include "GameController.h"
#include "EndingController.h"

ASampleGameModeBase::ASampleGameModeBase()
{
    DefaultPawnClass = nullptr;
}

AOpeningGameMode::AOpeningGameMode()
{
    PlayerControllerClass = AOpeningController::StaticClass();
}

AMainTitleGameMode::AMainTitleGameMode()
{
    PlayerControllerClass = ATitleController::StaticClass();
}

AGameGameMode::AGameGameMode()
{
    PlayerControllerClass = AGameController::StaticClass();
}

AEndingGameMode::AEndingGameMode()
{
    PlayerControllerClass = AEndingController::StaticClass();
}
