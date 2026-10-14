#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SampleTypes.h"
#include "SampleGameMode.generated.h"

class USampleScreenWidget;

UCLASS(Abstract, Blueprintable)
class SCREENFLOW_API ASampleGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ASampleGameMode();
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Sample") ESampleScreen Screen = ESampleScreen::Opening;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sample|UI") TSoftClassPtr<USampleScreenWidget> ScreenWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sample|UI") TSoftClassPtr<USampleScreenWidget> PauseWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sample|UI") TSoftClassPtr<USampleScreenWidget> ConfirmWidgetClass;
};

UCLASS()
class SCREENFLOW_API ASampleOpeningGameMode : public ASampleGameMode
{
    GENERATED_BODY()
public: ASampleOpeningGameMode();
};

UCLASS()
class SCREENFLOW_API ASampleTitleGameMode : public ASampleGameMode
{
    GENERATED_BODY()
public: ASampleTitleGameMode();
};

UCLASS()
class SCREENFLOW_API ASampleGameGameMode : public ASampleGameMode
{
    GENERATED_BODY()
public: ASampleGameGameMode();
};

UCLASS()
class SCREENFLOW_API ASampleEndingGameMode : public ASampleGameMode
{
    GENERATED_BODY()
public: ASampleEndingGameMode();
};
