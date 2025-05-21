#pragma once

#include "defines.h"
#include "freertos/queue.h"

typedef enum
{
    No_Seg = 0,
    First_Seg = 1,
    Second_Seg = 2,
    Third_Seg = 3,
    Fourt_Seg = 4,
    Fifth_Seg = 5,
} touchStates;

extern bool touchActivated;
extern std::mutex touchMut;
extern touchStates touchPressed;
extern TaskHandle_t touchTask;
extern QueueHandle_t touchQueue;

touchStates useTouch();
touchStates useAllTouch();

void setTouchInterrupt(uint8_t pin, uint16_t threshold);
void turnOnTouchInterrupts();
void turnOnTouch();
