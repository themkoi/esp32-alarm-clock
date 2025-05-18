#include "combinations.h"

#define BACK_INDEX 0
#define MENU_INDEX 1
#define UP_INDEX 2
#define DOWN_INDEX 3

int8_t combinationsChecks[4];

void initCombinations()
{
    for (uint8_t i = 0; i < 4; i++)
    {
        combinationsChecks[i] = -1;
    }
}

void loopCombinations()
{
    if (buttonRead(BACK_PIN) == BUT_CLICK_STATE)
    {
        combinationsChecks[BACK_INDEX] = BACK_PIN;
    }
    if (buttonRead(MENU_PIN) == BUT_CLICK_STATE)
    {
        combinationsChecks[MENU_INDEX] = MENU_PIN;
    }
    if (buttonRead(UP_PIN) == BUT_CLICK_STATE)
    {
        combinationsChecks[UP_INDEX] = UP_PIN;
    }
    if (buttonRead(DOWN_PIN) == BUT_CLICK_STATE)
    {
        combinationsChecks[DOWN_INDEX] = DOWN_PIN;
    }
}

bool wasClicked(uint8_t pin)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (combinationsChecks[i] == pin)
        {
            return true;
        }
    }
    return false;
}

void executeCombination()
{
    if (wasClicked(BACK_PIN) == true && wasClicked(UP_PIN) == true && wasClicked(MENU_PIN) == false && wasClicked(DOWN_PIN) == false)
    {   
        return;
    }
}

bool endCombinations(uint8_t currentPin)
{
    if (wasClicked(currentPin) == false)
    {
        Serial.println("The current pin was not clicked");
        return false;
    }

    uint8_t count = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (combinationsChecks[i] != -1)
        {
            count = count + 1;
            if (count >= 2)
            {
                break;
            }
        }
    }
    if (count < 2)
    {
        Serial.println("Less than 2 buttons were clicked");
        return false;
    }
    executeCombination();

    Serial.println("So there was a combination");

    return true;
}