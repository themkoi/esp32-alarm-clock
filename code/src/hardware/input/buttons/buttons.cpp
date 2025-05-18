#include "buttons.h"

bool buttonsActivated = false;
inkButtonStates buttonPressed = None;
TaskHandle_t buttonTask = NULL;
std::mutex buttMut;

bool anyButtonCheck()
{
    if (buttonRead(BACK_PIN) == BUT_CLICK_STATE || buttonRead(MENU_PIN) == BUT_CLICK_STATE || buttonRead(UP_PIN) == BUT_CLICK_STATE || buttonRead(DOWN_PIN) == BUT_CLICK_STATE)
    {
        return true;
    }
    return false;
}

inkButtonStates useButtonBack()
{
    buttMut.lock();
    // Serial.println("useButtonBack state: " + getButtonString(buttonPressed));
    if (buttonPressed == Back || buttonPressed == LongBack)
    {
        inputDetected = true;
        inkButtonStates buttonPressedTmp = buttonPressed;
        buttonPressed = None;
        buttMut.unlock();
        return buttonPressedTmp;
    }
    buttMut.unlock();
    return None;
}

inkButtonStates useButton()
{
    buttMut.lock();
    inkButtonStates buttonPressedTmp = buttonPressed;
    buttonPressed = None;

    // Serial.println("Used button by UI: " + getButtonString(buttonPressedTmp));
    if (buttonPressedTmp != None)
    {
        inputDetected = true;
    }
    buttMut.unlock();
    return buttonPressedTmp;
}

// Should be used only in watchface
inkButtonStates useAllButtons()
{
    buttMut.lock();
    // if (buttonPressed == Back || buttonPressed == LongBack)
    // {
    //     buttMut.unlock();
    //     return None;
    // }
    inkButtonStates buttonPressedTmp = buttonPressed;

    // Serial.println("Used button by UI: " + getButtonString(buttonPressedTmp));
    if (buttonPressedTmp != None)
    {
        inputDetected = true;
    }
    buttMut.unlock();
    return buttonPressedTmp;
}

void useButtonBlank()
{
    buttMut.lock();
    if (buttonPressed != Back && buttonPressed != LongBack)
    {
        buttonPressed = None;
    }
    buttMut.unlock();
}

void setButton(inkButtonStates button)
{
    buttMut.lock();
    buttonPressed = button;
    buttMut.unlock();
    
    Serial.println("setButton done");
}

bool wasCombination = false;
void longButtonCheck(int buttonPin, inkButtonStates normalButton, inkButtonStates longButton)
{
    int startime = millis();
    int elapsedtime = 0;
    initCombinations();
    loopCombinations();
    while (buttonRead(buttonPin) == BUT_CLICK_STATE && elapsedtime <= BUTTON_LONG_PRESS_MS)
    {
        delay(SMALL_BUTTON_DELAY_MS);
        elapsedtime = millis() - startime;
        loopCombinations();
    }
    if (endCombinations(buttonPin) == true)
    {
        wasCombination = true;
        uint8_t counter = 0;
        while (counter <= 3)
        {
            if (anyButtonCheck() == false)
            {
                counter = counter + 1;
                delay((BUTTON_TASK_DELAY * 3));
            }
            delay(SMALL_BUTTON_DELAY_MS);
        }
        delay((BUTTON_TASK_DELAY * 3));
        return;
    }

    Serial.println("elapsed time: " + String(elapsedtime) + " BUTTON_LONG_PRESS_MS:" + String(BUTTON_LONG_PRESS_MS));
    if (elapsedtime >= BUTTON_LONG_PRESS_MS)
    {
        setButton(longButton);
        Serial.println("Vibrating long button now");
        while (buttonRead(buttonPin) == BUT_CLICK_STATE)
        {
            delay(SMALL_BUTTON_DELAY_MS);
        }
    }
    else
    {
        setButton(normalButton);
    }
    delay(BUTTON_TASK_DELAY / ADD_BUTTON_DELAY);
}

void loopButtonsTask(void *parameter)
{
    buttonsActivated = true;
    // Wait for all buttons to drop down, helpfull for manageButtonWakeUp
    while (buttonRead(BACK_PIN) == BUT_CLICK_STATE || buttonRead(MENU_PIN) == BUT_CLICK_STATE || buttonRead(UP_PIN) == BUT_CLICK_STATE || buttonRead(DOWN_PIN) == BUT_CLICK_STATE)
    {
        delay(SMALL_BUTTON_DELAY_MS);
    }
    interruptedButton = None;
    while (true)
    {
        wasCombination = false;
        // Serial.println("Button task looping...");
        inkButtonStates interruptedButtonCopy = interruptedButton;
        // Serial.println("interruptedButtonCopy: " + getButtonString(interruptedButtonCopy));
        // Serial.println("buttonPressed: " + getButtonString(buttonPressed));

        buttMut.lock();
        if (interruptedButtonCopy == Back && buttonPressed != LongBack)
        {
            buttMut.unlock();
            longButtonCheck(BACK_PIN, Back, LongBack);
            buttMut.lock();
        }
        else if (interruptedButtonCopy == Menu && buttonPressed != Back && buttonPressed != LongBack && buttonPressed != LongMenu)
        {
            buttMut.unlock();
            longButtonCheck(MENU_PIN, Menu, LongMenu);
            buttMut.lock();
        }
        else if (interruptedButtonCopy == Up && buttonPressed != Menu && buttonPressed != Back && buttonPressed != LongBack && buttonPressed != LongMenu && buttonPressed != LongUp)
        {
            buttMut.unlock();
            longButtonCheck(UP_PIN, Up, LongUp);
            buttMut.lock();
        }
        else if (interruptedButtonCopy == Down && buttonPressed != Up && buttonPressed != Menu && buttonPressed != Back && buttonPressed != LongUp && buttonPressed != LongBack && buttonPressed != LongDown && buttonPressed != LongMenu)
        {
            buttMut.unlock();
            longButtonCheck(DOWN_PIN, Down, LongDown);
            buttMut.lock();
        }
        buttMut.unlock();
        if (interruptedButtonCopy == interruptedButton || wasCombination == true)
        {
            interruptedButton = None;
            wasCombination = false;
            Serial.println("Button task going to sleep!"); // That's normal and very efficient
            vTaskSuspend(NULL);
        }
        else
        {
            Serial.println("Another button clicked...");
        }
    }
}

void initButtonTask()
{
    xTaskCreate(
        loopButtonsTask,
        "buttonTask",
        4700, // Too much but handling fs logging takes a bit more
        NULL,
        12,
        &buttonTask);
}
/*
Idk why this fuction doesn't work
==================== CURRENT THREAD STACK =====================
#0  0x400838dd in panic_abort (details=0x3ffb4090 "assert failed: eTaskGetState tasks.c:1696 (pxTCB)") at /root/.platformio/packages/framework-espidf/components/esp_system/panic.c:452
#1  0x4008bd88 in esp_system_abort (details=0x3ffb4090 "assert failed: eTaskGetState tasks.c:1696 (pxTCB)") at /root/.platformio/packages/framework-espidf/components/esp_system/port/esp_system_chip.c:84
#2  0x40092ff4 in __assert_func (file=<optimized out>, line=<optimized out>, func=<optimized out>, expr=<optimized out>) at /root/.platformio/packages/framework-espidf/components/newlib/assert.c:81
#3  0x4008d0fc in eTaskGetState (xTask=0x0) at /root/.platformio/packages/framework-espidf/components/freertos/FreeRTOS-Kernel/tasks.c:1696
#4  0x400d40b4 in deInitButtonTask () at src/hardware/buttons.cpp:182
#5  0x400d5426 in goSleep () at src/hardware/sleep.cpp:62
#6  0x400d551a in manageSleep () at src/hardware/sleep.cpp:148
#7  0x400d55bd in loop () at src/main.cpp:164
#8  0x400fa684 in loopTask (pvParameters=<optimized out>) at /root/.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp:74
#9  0x4008ed1d in vPortTaskWrapper (pxCode=0x400fa650 <loopTask(void*)>, pvParameters=0x0) at /root/.platformio/packages/framework-espidf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:162

For now i will not care

MAYBE it's because this task is never initialized when doing a RTC wakeup...
*/
void deInitButtonTask()
{
    eTaskState taskState = eTaskGetState(buttonTask);
    if (taskState == eRunning || taskState == eSuspended)
    {
        Serial.println("Shutting down button task");
        vTaskDelete(buttonTask);
    }
    else
    {
        Serial.println("Not shutting down button task, it's state is: " + String(taskState));
    }
}

void turnOnButtons()
{
    if (buttonsActivated == false)
    {
        initButtonTask();
        turnOnInterrupts();
    }
}

bool buttonRead(uint8_t pin)
{
    return digitalRead(pin);
}