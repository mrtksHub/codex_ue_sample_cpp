#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SampleGameMode.generated.h"

// One-to-one translation of BP_GameModeBase and its four Blueprint children.
UCLASS()
class SCREENFLOW_API ASampleGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ASampleGameModeBase();
};

UCLASS()
class SCREENFLOW_API AOpeningGameMode : public ASampleGameModeBase
{
    GENERATED_BODY()
public:
    AOpeningGameMode();
};

UCLASS()
class SCREENFLOW_API AMainTitleGameMode : public ASampleGameModeBase
{
    GENERATED_BODY()
public:
    AMainTitleGameMode();
};

UCLASS()
class SCREENFLOW_API AGameGameMode : public ASampleGameModeBase
{
    GENERATED_BODY()
public:
    AGameGameMode();
};

UCLASS()
class SCREENFLOW_API AEndingGameMode : public ASampleGameModeBase
{
    GENERATED_BODY()
public:
    AEndingGameMode();
};
