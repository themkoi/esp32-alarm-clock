#include "touch.h"

void resumeTouchTaskFromISR(BaseType_t *hpw) {
    xTaskResumeFromISR(touchTask);
    portYIELD_FROM_ISR(*hpw);
}

void sendTouch(touchStates s) {
    BaseType_t hpw = pdFALSE;
    xQueueSendFromISR(touchQueue, &s, &hpw);
    resumeTouchTaskFromISR(&hpw);
}

void firstSegInt()  { sendTouch(First_Seg); }
void secondSegInt() { sendTouch(Second_Seg); }
void thirdSegInt()  { sendTouch(Third_Seg); }
void fourthSegInt() { sendTouch(Fourt_Seg); }
void fifthSegInt()  { sendTouch(Fifth_Seg); }

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
        case TOUCH_1_Seg_PIN: touchAttachInterrupt(pin, firstSegInt, threshold); break;
        case TOUCH_2_Seg_PIN: touchAttachInterrupt(pin, secondSegInt, threshold); break;
        case TOUCH_3_Seg_PIN: touchAttachInterrupt(pin, thirdSegInt, threshold); break;
        case TOUCH_4_Seg_PIN: touchAttachInterrupt(pin, fourthSegInt, threshold); break;
        case TOUCH_5_Seg_PIN: touchAttachInterrupt(pin, fifthSegInt, threshold); break;
        default: Serial.println("wtf are you doing"); break;
    }
}
