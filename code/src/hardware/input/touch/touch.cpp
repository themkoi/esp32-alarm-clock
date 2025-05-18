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