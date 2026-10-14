#include "SampleGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"

void USampleGameInstance::Init()
{
    Super::Init();
    if (GEngine)
    {
        TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &USampleGameInstance::HandleTravelFailure);
    }
}

void USampleGameInstance::Shutdown()
{
    if (GEngine) GEngine->OnTravelFailure().Remove(TravelFailureHandle);
    Super::Shutdown();
}

FString USampleGameInstance::MapPackage(ESampleScreen Screen)
{
    switch (Screen)
    {
    case ESampleScreen::Opening: return TEXT("/Game/Maps/L_Opening");
    case ESampleScreen::MainTitle: return TEXT("/Game/Maps/L_MainTitle");
    case ESampleScreen::Game: return TEXT("/Game/Maps/L_Game");
    case ESampleScreen::Ending: return TEXT("/Game/Maps/L_Ending");
    default: return FString();
    }
}

bool USampleGameInstance::GoToScreen(ESampleScreen Target)
{
    if (bTransitionInProgress || !GetWorld()) return false;
    const bool bAllowed =
        (CurrentScreen == ESampleScreen::Opening && Target == ESampleScreen::MainTitle) ||
        (CurrentScreen == ESampleScreen::MainTitle && Target == ESampleScreen::Game) ||
        (CurrentScreen == ESampleScreen::Game && (Target == ESampleScreen::Ending || Target == ESampleScreen::MainTitle)) ||
        (CurrentScreen == ESampleScreen::Ending && Target == ESampleScreen::MainTitle);
    const FString Package = MapPackage(Target);
    if (!bAllowed || Package.IsEmpty()) return false;
    if (!FPackageName::DoesPackageExist(Package))
    {
        UE_LOG(LogTemp, Error, TEXT("ScreenFlow: missing destination map %s; see docs/EDITOR_SETUP.md"), *Package);
        return false;
    }
    // Commit the lock before issuing travel. State changes only on destination readiness.
    PendingScreen = Target;
    bTransitionInProgress = true;
    UGameplayStatics::SetGamePaused(this, false);
    UGameplayStatics::OpenLevel(this, FName(*Package));
    return true;
}

bool USampleGameInstance::NotifyScreenReady(ESampleScreen Screen)
{
    const FString MapName = UGameplayStatics::GetCurrentLevelName(this, true);
    if (MapName != FPackageName::GetShortName(MapPackage(Screen)) ||
        (bTransitionInProgress && PendingScreen != Screen))
    {
        bTransitionInProgress = false;
        UE_LOG(LogTemp, Error, TEXT("ScreenFlow: map/GameMode mismatch on %s; check World Settings"), *MapName);
        return false;
    }
    // Also supports directly starting any of the four maps in PIE.
    CurrentScreen = Screen;
    bTransitionInProgress = false;
    OnDataChanged.Broadcast();
    return true;
}

void USampleGameInstance::HandleTravelFailure(UWorld* World, ETravelFailure::Type Type, const FString& Error)
{
    if (World && World->GetGameInstance() == this)
    {
        bTransitionInProgress = false;
        UE_LOG(LogTemp, Error, TEXT("ScreenFlow: travel failed (%d): %s"), static_cast<int32>(Type), *Error);
        OnDataChanged.Broadcast();
    }
}

void USampleGameInstance::SetPlayerData(const FSamplePlayerData& Data)
{
    PlayerData = Data;
    PlayerData.PlayerLevel = FMath::Max(1, PlayerData.PlayerLevel);
    OnDataChanged.Broadcast();
}

void USampleGameInstance::SetSettings(const FSampleSettings& Data)
{
    Settings = Data;
    Settings.MasterVolume = FMath::Clamp(Settings.MasterVolume, 0.f, 1.f);
    Settings.BGMVolume = FMath::Clamp(Settings.BGMVolume, 0.f, 1.f);
    Settings.SEVolume = FMath::Clamp(Settings.SEVolume, 0.f, 1.f);
    OnDataChanged.Broadcast();
}

void USampleGameInstance::SetCurrentChapter(int32 Chapter)
{
    CurrentChapter = FMath::Max(0, Chapter);
    OnDataChanged.Broadcast();
}
