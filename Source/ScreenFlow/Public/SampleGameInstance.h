#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SampleTypes.h"
#include "SampleGameInstance.generated.h"

// Equivalent to BP_GameInstance. The screen state is not Unreal's AGameState.
UCLASS(BlueprintType, Blueprintable)
class SCREENFLOW_API USampleGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category="Sample|State")
    ESampleScreen GameState = ESampleScreen::Opening;

    UPROPERTY(BlueprintReadOnly, Category="Sample|State")
    bool bTransitionInProgress = false;

    UPROPERTY(BlueprintReadWrite, Category="Sample|Data")
    int32 CurrentChapter = 0;

    UPROPERTY(BlueprintReadWrite, Category="Sample|Data")
    FSamplePlayerData PlayerData;

    UPROPERTY(BlueprintReadWrite, Category="Sample|Data")
    FSampleSettings Settings;

    UPROPERTY(BlueprintReadWrite, Category="Sample|Data")
    FSampleSaveData SaveData;

    // Matches BP_GameInstance.GoToScreen: only a transition-in-progress guard.
    UFUNCTION(BlueprintCallable, Category="Sample|Flow")
    void GoToScreen(ESampleScreen Target);

    // Matches BP_GameInstance.NotifyScreenReady; called by each controller's BeginPlay.
    UFUNCTION(BlueprintCallable, Category="Sample|Flow")
    void NotifyScreenReady(ESampleScreen Screen);

    static FName MapPackage(ESampleScreen Screen);
};
