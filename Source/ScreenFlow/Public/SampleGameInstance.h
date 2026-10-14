#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/EngineBaseTypes.h"
#include "SampleTypes.h"
#include "SampleGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSampleDataChanged);

UCLASS(BlueprintType, Blueprintable)
class SCREENFLOW_API USampleGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

    UPROPERTY(BlueprintReadOnly, Category="Sample|State") ESampleScreen CurrentScreen = ESampleScreen::Opening;
    UPROPERTY(BlueprintReadOnly, Category="Sample|State") bool bTransitionInProgress = false;
    UPROPERTY(BlueprintReadOnly, Category="Sample|Data") int32 CurrentChapter = 0;
    UPROPERTY(BlueprintReadOnly, Category="Sample|Data") FSamplePlayerData PlayerData;
    UPROPERTY(BlueprintReadOnly, Category="Sample|Data") FSampleSettings Settings;
    UPROPERTY(BlueprintReadOnly, Category="Sample|Data") FSampleSaveData SaveData;
    UPROPERTY(BlueprintAssignable, Category="Sample|Data") FSampleDataChanged OnDataChanged;

    UFUNCTION(BlueprintCallable, Category="Sample|Flow") bool GoToScreen(ESampleScreen Target);
    UFUNCTION(BlueprintCallable, Category="Sample|Data") void SetPlayerData(const FSamplePlayerData& Data);
    UFUNCTION(BlueprintCallable, Category="Sample|Data") void SetSettings(const FSampleSettings& Data);
    UFUNCTION(BlueprintCallable, Category="Sample|Data") void SetCurrentChapter(int32 Chapter);

    // C++ controller calls this after the destination level starts.
    bool NotifyScreenReady(ESampleScreen Screen);
    static FString MapPackage(ESampleScreen Screen);

private:
    ESampleScreen PendingScreen = ESampleScreen::Opening;
    FDelegateHandle TravelFailureHandle;
    void HandleTravelFailure(UWorld* World, ETravelFailure::Type Type, const FString& Error);
};
