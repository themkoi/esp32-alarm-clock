#include "buttons.h"

inkButtonStates interruptedButton = None;

// No logs here because interrupts don't like them
void resumeButtonTask()
{
    //Serial.println("Resuming button task");
    eTaskState taskState = eTaskGetState(buttonTask);
    if(taskState == eSuspended) {
        vTaskResume(buttonTask);
    } 
#if DEBUG
    else {
        if(taskState != eRunning) {
            //Serial.println("Something is wrong with button task: " + String(taskState));
        }
    }
#endif
}

void backIntBut()
{
    interruptedButton = Back;
    resumeButtonTask();
}

void menuIntBut()
{
    interruptedButton = Menu;
    resumeButtonTask();
}

void upIntBut()
{
    interruptedButton = Up;
    resumeButtonTask();
}

void downIntBut()
{
    interruptedButton = Down;
    resumeButtonTask();
}

void turnOnInterrupts()
{
    attachInterrupt(digitalPinToInterrupt(BACK_PIN), backIntBut, BUTTON_INTER_COND);
    attachInterrupt(digitalPinToInterrupt(MENU_PIN), menuIntBut, BUTTON_INTER_COND);
    attachInterrupt(digitalPinToInterrupt(UP_PIN), upIntBut, BUTTON_INTER_COND);
    attachInterrupt(digitalPinToInterrupt(DOWN_PIN), downIntBut, BUTTON_INTER_COND);
    Serial.println("Attached button interrupts!");
}
