#include "touch.h"

bool touchActivated = false;
touchStates touchPressed = No_Seg;
TaskHandle_t touchTask = NULL;
std::mutex touchMut;

touchStates useTouch()
{
    touchMut.lock();
    touchStates touchPressedTmp = touchPressed;
    touchPressed = No_Seg;

    if (touchPressedTmp != No_Seg)
    {
        inputDetected = true;
    }
    touchMut.unlock();
    return touchPressedTmp;
}

touchStates useAllTouch()
{
    touchMut.lock();
    touchStates touchPressedTmp = touchPressed;

    if (touchPressedTmp != No_Seg)
    {
        inputDetected = true;
    }
    touchMut.unlock();
    return touchPressedTmp;
}

typedef struct {
    gpio_num_t pin;
    uint8_t threshold;
} TouchConfig;

TouchConfig getTouchConfig(touchStates state) {
    bool onBattery = !powerConnected;

    switch (state) {
        case First_Seg: {
            uint8_t thresh = onBattery ? TOUCH_1_Seg_THRESHOLD_BAT : TOUCH_1_Seg_THRESHOLD;
            return (TouchConfig){ TOUCH_1_Seg_PIN, thresh };
        }
        case Second_Seg: {
            uint8_t thresh = onBattery ? TOUCH_2_Seg_THRESHOLD_BAT : TOUCH_2_Seg_THRESHOLD;
            return (TouchConfig){ TOUCH_2_Seg_PIN, thresh };
        }
        case Third_Seg: {
            uint8_t thresh = onBattery ? TOUCH_3_Seg_THRESHOLD_BAT : TOUCH_3_Seg_THRESHOLD;
            return (TouchConfig){ TOUCH_3_Seg_PIN, thresh };
        }
        case Fourt_Seg: {
            uint8_t thresh = onBattery ? TOUCH_4_Seg_THRESHOLD_BAT : TOUCH_4_Seg_THRESHOLD;
            return (TouchConfig){ TOUCH_4_Seg_PIN, thresh };
        }
        case Fifth_Seg: {
            uint8_t thresh = onBattery ? TOUCH_5_Seg_THRESHOLD_BAT : TOUCH_5_Seg_THRESHOLD;
            return (TouchConfig){ TOUCH_5_Seg_PIN, thresh };
        }
        default:
            return (TouchConfig){ GPIO_NUM_NC, 0 };
    }
}


void setTouch(touchStates touch)
{
    touchMut.lock();
    touchPressed = touch;
    touchMut.unlock();

    Serial.println("setTouch done" + String(touch));
    TouchConfig currentTouch = getTouchConfig(touch);
    while (touchRead(currentTouch.pin) < currentTouch.threshold)
    {
        delay(SMALL_BUTTON_DELAY_MS);
    }
}

void loopTouchTask(void *parameter)
{
    touchActivated = true;
    interruptedTouch = No_Seg;

    while (true)
    {
        touchStates interruptedTouchCopy;

        touchMut.lock();
        interruptedTouchCopy = interruptedTouch;
        touchMut.unlock();

        if (interruptedTouchCopy != No_Seg)
        {
            setTouch(interruptedTouchCopy);
        }

        touchMut.lock();
        if (interruptedTouchCopy == interruptedTouch)
        {
            interruptedTouch = No_Seg;
            touchMut.unlock();
            Serial.println("Touch task going to sleep!");
            vTaskSuspend(NULL);
        }
        else
        {
            touchMut.unlock();
            Serial.println("Another touch segment activated...");
        }
    }
}


void initTouchTask()
{
    xTaskCreate(
        loopTouchTask,
        "touchTask",
        4700, // Too much but handling fs logging takes a bit more
        NULL,
        12,
        &touchTask);
}

void turnOnTouch()
{
    if (touchActivated == false)
    {
    initTouchTask();
    turnOnTouchInterrupts();
    }
}