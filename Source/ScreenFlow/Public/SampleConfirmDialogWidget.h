#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SampleConfirmDialogWidget.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSampleDialogConfirmed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSampleDialogCancelled);

// Typed C++ bridge for the two dispatchers of the original WBP_ConfirmDialog.
// Its buttons and visual layout remain in Widget Blueprint.
UCLASS(Blueprintable)
class SCREENFLOW_API USampleConfirmDialogWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, Category="Sample|Dialog")
    FSampleDialogConfirmed OnConfirmed;

    UPROPERTY(BlueprintAssignable, Category="Sample|Dialog")
    FSampleDialogCancelled OnCancelled;

    UFUNCTION(BlueprintCallable, Category="Sample|Dialog")
    void ConfirmYes();

    UFUNCTION(BlueprintCallable, Category="Sample|Dialog")
    void ConfirmNo();

    UFUNCTION(BlueprintCallable, Category="Sample|Dialog")
    void SetConfirmMessage(const FText& Message);

protected:
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> Text_Message;
};
