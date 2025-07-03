#include "touch.h"

touchStates interruptedTouch = No_Seg;

// No logs here because interrupts don't like them
void resumeTouchTask()
{
    //Serial.println("Resuming button task");
    eTaskState taskState = eTaskGetState(touchTask);
    if(taskState == eSuspended) {
        vTaskResume(touchTask);
    } 
#if DEBUG
    else {
        if(taskState != eRunning) {
            //Serial.println("Something is wrong with button task: " + String(taskState));
        }
    }
#endif
}

void firstSegInt()
{
    interruptedTouch = First_Seg;
    resumeTouchTask();
}

void secondSegInt()
{
    interruptedTouch = Second_Seg;
    resumeTouchTask();
}

void thirdSegInt()
{
    interruptedTouch = Third_Seg;
    resumeTouchTask();
}

void fourthSegInt()
{
    interruptedTouch = Fourt_Seg;
    resumeTouchTask();
}

void fifthSegInt()
{
    interruptedTouch = Fifth_Seg;
    resumeTouchTask();
}

void turnOnTouchInterrupts()
{
    touchAttachInterrupt(TOUCH_1_Seg_PIN, firstSegInt, TOUCH_1_Seg_THRESHOLD);
    touchAttachInterrupt(TOUCH_2_Seg_PIN, secondSegInt, TOUCH_2_Seg_THRESHOLD);
    touchAttachInterrupt(TOUCH_3_Seg_PIN, thirdSegInt, TOUCH_3_Seg_THRESHOLD);
    touchAttachInterrupt(TOUCH_4_Seg_PIN, fourthSegInt, TOUCH_4_Seg_THRESHOLD);
    touchAttachInterrupt(TOUCH_5_Seg_PIN, fifthSegInt, TOUCH_5_Seg_THRESHOLD);
    Serial.println("Attached touch interrupts!");
}

void setTouchInterrupt(uint8_t pin, uint16_t threshold)
{
    switch (pin)
    {
        case TOUCH_1_Seg_PIN:
            touchAttachInterrupt(pin, firstSegInt, threshold);
            break;
        case TOUCH_2_Seg_PIN:
            touchAttachInterrupt(pin, secondSegInt, threshold);
            break;
        case TOUCH_3_Seg_PIN:
            touchAttachInterrupt(pin, thirdSegInt, threshold);
            break;
        case TOUCH_4_Seg_PIN:
            touchAttachInterrupt(pin, fourthSegInt, threshold);
            break;
        case TOUCH_5_Seg_PIN:
            touchAttachInterrupt(pin, fifthSegInt, threshold);
            break;
        default:
            Serial.println("wtf are you doing");
            break;
    }
}