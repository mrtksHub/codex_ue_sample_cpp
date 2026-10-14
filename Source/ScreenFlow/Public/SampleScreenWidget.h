#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SampleScreenWidget.generated.h"

class UButton;
class UTextBlock;
class USampleGameInstance;

// All six WBP assets derive from this. Optional bindings let each view own its layout.
UCLASS(Abstract, Blueprintable)
class SCREENFLOW_API USampleScreenWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    USampleScreenWidget(const FObjectInitializer& ObjectInitializer);
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent) override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;

    UFUNCTION(BlueprintNativeEvent, Category="Sample|UI") UWidget* GetInitialFocusTarget();
    UFUNCTION(BlueprintImplementableEvent, Category="Sample|UI") void OnModelChanged(USampleGameInstance* Model);
    UFUNCTION(BlueprintCallable, Category="Sample|UI") void SetConfirmMessage(const FText& Message);

protected:
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_Start;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_ExitGame;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_GameClear;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_Finish;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_Resume;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_ReturnToTitle;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_Yes;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UButton> Btn_No;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Text_Message;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Text_CurrentScreen;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Text_PlayerName;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Text_PlayerLevel;
    UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> Text_CurrentChapter;

private:
    UFUNCTION() void RefreshFromModel();
};
