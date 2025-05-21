#include "touch.h"

bool touchActivated = false;
touchStates touchPressed = No_Seg;
TaskHandle_t touchTask = NULL;
std::mutex touchMut;
QueueHandle_t touchQueue = NULL;

touchStates useTouch()
{
    touchMut.lock();
    touchStates touchPressedTmp = touchPressed;
    touchPressed = No_Seg;

    if (touchPressedTmp != No_Seg)
        inputDetected = true;

    touchMut.unlock();
    return touchPressedTmp;
}

touchStates useAllTouch()
{
    touchMut.lock();
    touchStates touchPressedTmp = touchPressed;

    if (touchPressedTmp != No_Seg)
        inputDetected = true;

    touchMut.unlock();
    return touchPressedTmp;
}

typedef struct {
    gpio_num_t pin;
    uint8_t threshold;
} TouchConfig;

TouchConfig getTouchConfig(touchStates state) {
    bool onBattery = !checkPower();

    switch (state) {
        case First_Seg:
            return { TOUCH_1_Seg_PIN, (uint8_t)(onBattery ? TOUCH_1_Seg_THRESHOLD_BAT : TOUCH_1_Seg_THRESHOLD) };
        case Second_Seg:
            return { TOUCH_2_Seg_PIN, (uint8_t)(onBattery ? TOUCH_2_Seg_THRESHOLD_BAT : TOUCH_2_Seg_THRESHOLD) };
        case Third_Seg:
            return { TOUCH_3_Seg_PIN, (uint8_t)(onBattery ? TOUCH_3_Seg_THRESHOLD_BAT : TOUCH_3_Seg_THRESHOLD) };
        case Fourt_Seg:
            return { TOUCH_4_Seg_PIN, (uint8_t)(onBattery ? TOUCH_4_Seg_THRESHOLD_BAT : TOUCH_4_Seg_THRESHOLD) };
        case Fifth_Seg:
            return { TOUCH_5_Seg_PIN, (uint8_t)(onBattery ? TOUCH_5_Seg_THRESHOLD_BAT : TOUCH_5_Seg_THRESHOLD) };
        default:
            return { GPIO_NUM_NC, 0 };
    }
}


void setTouch(touchStates touch)
{
    touchMut.lock();
    touchPressed = touch;
    touchMut.unlock();

    Serial.println("setTouch done");
}

void loopTouchTask(void *parameter)
{
    touchActivated = true;
    touchStates touched;

    while (true)
    {
        if (xQueueReceive(touchQueue, &touched, portMAX_DELAY) == pdTRUE)
        {
            setTouch(touched);
            Serial.println("Touch task going to sleep!");
            vTaskSuspend(NULL);
        }
    }
}

void initTouchTask()
{
    touchQueue = xQueueCreate(10, sizeof(touchStates));
    xTaskCreate(loopTouchTask, "touchTask", 4700, NULL, 12, &touchTask);
}

void turnOnTouch()
{
    if (!touchActivated)
    {
        initTouchTask();
        turnOnTouchInterrupts();
    }
}
