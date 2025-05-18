#pragma once

#include "defines.h"

typedef enum
{
    Unknown = 0, // For the task to look for answers, used on the yatchy
    None = 1,
    Back = 2,
    Menu = 3,
    Up = 4,
    Down = 5,
    LongBack = 6,
    LongMenu = 7,
    LongUp = 8,
    LongDown = 9,
} inkButtonStates;

extern bool buttonsActivated;
extern std::mutex buttMut;
extern inkButtonStates buttonPressed;
extern TaskHandle_t buttonTask;

inkButtonStates useButtonBack();
inkButtonStates useButton();
inkButtonStates useAllButtons();
void loopButtonsTask(void *parameter);

void turnOnButtons();
void initButtonTask();
void deInitButtonTask();
void setButton(inkButtonStates button);
void longButtonCheck(int buttonPin, inkButtonStates normalButton, inkButtonStates longButton);
void turnOnInterrupts();
void resumeButtonTask();
extern inkButtonStates interruptedButton;
bool buttonRead(uint8_t pin); // Wrapper for digitalRead for buttons
