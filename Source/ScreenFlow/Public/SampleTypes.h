#pragma once

#include "CoreMinimal.h"
#include "SampleTypes.generated.h"

UENUM(BlueprintType)
enum class ESampleScreen : uint8
{
    Opening,
    MainTitle,
    Game,
    Ending
};

UENUM(BlueprintType)
enum class ESampleConfirmAction : uint8
{
    None,
    ReturnToTitle,
    QuitGame
};

USTRUCT(BlueprintType)
struct SCREENFLOW_API FSamplePlayerData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString PlayerName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 PlayerLevel = 1;
};

USTRUCT(BlueprintType)
struct SCREENFLOW_API FSampleSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float MasterVolume = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float BGMVolume = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SEVolume = 1.f;
};

// Reserved in-memory data only; disk persistence is outside this sample.
USTRUCT(BlueprintType)
struct SCREENFLOW_API FSampleSaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SaveVersion = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SavedChapter = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasSave = false;
};
