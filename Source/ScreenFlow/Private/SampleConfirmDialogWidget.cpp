#include "SampleConfirmDialogWidget.h"

#include "Components/TextBlock.h"

void USampleConfirmDialogWidget::ConfirmYes()
{
    OnConfirmed.Broadcast();
}

void USampleConfirmDialogWidget::ConfirmNo()
{
    OnCancelled.Broadcast();
}

void USampleConfirmDialogWidget::SetConfirmMessage(const FText& Message)
{
    if (Text_Message)
    {
        Text_Message->SetText(Message);
    }
}
