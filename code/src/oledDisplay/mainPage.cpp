#include "mainPage.h"

const uint8_t PROGMEM
    toastermask0[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
        0x00, 0x20, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00,
        0x00, 0x10, 0x00, 0x00, 0x00, 0x43, 0xF8, 0x00,
        0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80,
        0x01, 0xF1, 0xF8, 0xF0, 0x07, 0xC7, 0xC7, 0x00,
        0x0F, 0x1F, 0x08, 0x2B, 0x1E, 0x7C, 0x11, 0x00,
        0x04, 0xF0, 0x20, 0x56, 0x61, 0xE4, 0x22, 0x00,
        0x78, 0x48, 0x40, 0xA8, 0x4E, 0x10, 0x44, 0x00,
        0x53, 0x90, 0x81, 0xF8, 0x5C, 0xA0, 0x90, 0x58,
        0x7C, 0xA1, 0x07, 0x90, 0x74, 0xA1, 0x21, 0x38,
        0x7F, 0xB5, 0x0E, 0x30, 0x77, 0xB0, 0x90, 0x78,
        0x7F, 0xB4, 0x60, 0xF0, 0x77, 0xBB, 0x03, 0xE0,
        0x7F, 0xBC, 0x0F, 0xC0, 0x77, 0xBF, 0xFF, 0x00,
        0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00,
        0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00,
        0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    toastermask1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xF8, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80, 0x01, 0xF1, 0xF8, 0x00, 0x07, 0xC7, 0xC1, 0xE0, 0x0F, 0x1F, 0x06, 0x10, 0x1E, 0x7C, 0x00, 0x00, 0x04, 0xF0, 0x07, 0xF0, 0x61, 0xE4, 0x18, 0x08, 0x78, 0x48, 0x20, 0x80, 0x4E, 0x10, 0x40, 0x2B, 0x53, 0x90, 0x81, 0x00, 0x5C, 0xA0, 0x80, 0x58, 0x7C, 0xA1, 0x02, 0x00, 0x74, 0xA1, 0x28, 0xB8, 0x7F, 0xB5, 0x0F, 0xF0, 0x77, 0xB0, 0x90, 0x78, 0x7F, 0xB4, 0x60, 0xF0, 0x77, 0xBB, 0x03, 0xE0, 0x7F, 0xBC, 0x0F, 0xC0, 0x77, 0xBF, 0xFF, 0x00, 0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00, 0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, toastermask2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80, 0x01, 0xF1, 0xF8, 0x00, 0x07, 0xC7, 0xC1, 0xE0, 0x0F, 0x1F, 0x06, 0x10, 0x1E, 0x7C, 0x00, 0x00, 0x04, 0xF0, 0x00, 0x00, 0x61, 0xE4, 0x00, 0x08, 0x78, 0x48, 0x00, 0x10, 0x4E, 0x10, 0x00, 0x18, 0x53, 0x90, 0x60, 0x70, 0x5C, 0xA0, 0x9F, 0xC8, 0x7C, 0xA1, 0x04, 0x92, 0x74, 0xA1, 0x09, 0x24, 0x7F, 0xB5, 0x02, 0x48, 0x77, 0xB0, 0x80, 0x10, 0x7F, 0xB4, 0x41, 0x00, 0x77, 0xBB, 0x20, 0x40, 0x7F, 0xBC, 0x10, 0x00, 0x77, 0xBF, 0xF8, 0x00, 0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00, 0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, toastmask[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x30, 0x0C, 0x00, 0x00, 0xC2, 0x23, 0x00, 0x07, 0x00, 0x80, 0xC0, 0x08, 0x25, 0x50, 0x30, 0x10, 0x0B, 0xA8, 0x08, 0x21, 0x37, 0xF5, 0x04, 0x30, 0x4A, 0xE8, 0x0C, 0x3C, 0x15, 0x52, 0x34, 0x2B, 0x00, 0x80, 0xEC, 0x35, 0xC2, 0x23, 0x54, 0x1A, 0xB0, 0x0E, 0xAC, 0x0D, 0x5C, 0x15, 0x58, 0x03, 0xAB, 0xEA, 0xE0, 0x00, 0xD5, 0x55, 0x80, 0x00, 0x3A, 0xAE, 0x00, 0x00, 0x0D, 0x58, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, toaster0[] = {0x00, 0x30, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x01, 0xD8, 0x00, 0x00, 0x03, 0x74, 0xF0, 0x00, 0x07, 0xEF, 0xFE, 0x00, 0x07, 0xBC, 0x07, 0x80, 0x0F, 0xE0, 0x7F, 0xE0, 0x0F, 0x83, 0xC0, 0x70, 0x1E, 0x0E, 0x07, 0x08, 0x18, 0x38, 0x38, 0xFF, 0x30, 0xE0, 0xF7, 0xD4, 0x61, 0x83, 0xEE, 0xFF, 0x7B, 0x0F, 0xDF, 0xA8, 0x9E, 0x1B, 0xDD, 0xFE, 0x87, 0xB7, 0xBF, 0x50, 0xB1, 0xEF, 0xBB, 0xF8, 0xAC, 0x6F, 0x7E, 0x00, 0xA3, 0x5F, 0x6F, 0xA0, 0x83, 0x5E, 0xF8, 0x68, 0x8B, 0x5E, 0xDE, 0xC0, 0x80, 0x4A, 0xF1, 0xC8, 0x88, 0x4F, 0x6F, 0x80, 0x80, 0x4B, 0x9F, 0x08, 0x88, 0x44, 0xFC, 0x10, 0x80, 0x43, 0xF0, 0x20, 0x88, 0x40, 0x00, 0xC0, 0x80, 0x40, 0x03, 0x00, 0x40, 0x40, 0x0C, 0x00, 0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00, 0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00}, toaster1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xF0, 0x00, 0x00, 0xF7, 0xFE, 0x00, 0x01, 0xBC, 0x07, 0x80, 0x03, 0xE0, 0x7F, 0xE0, 0x07, 0x83, 0xC0, 0x70, 0x0E, 0x0E, 0x07, 0xF8, 0x18, 0x38, 0x3E, 0x18, 0x30, 0xE0, 0xF9, 0xE8, 0x61, 0x83, 0xFF, 0xF8, 0x7B, 0x0F, 0xF8, 0x08, 0x9E, 0x1B, 0xE7, 0xF0, 0x87, 0xB7, 0xDF, 0x7F, 0xB1, 0xEF, 0xBF, 0xD4, 0xAC, 0x6F, 0x7E, 0xFF, 0xA3, 0x5F, 0x7F, 0xA6, 0x83, 0x5E, 0xFD, 0xF8, 0x8B, 0x5E, 0xD7, 0x40, 0x80, 0x4A, 0xF0, 0x08, 0x88, 0x4F, 0x6F, 0x80, 0x80, 0x4B, 0x9F, 0x08, 0x88, 0x44, 0xFC, 0x10, 0x80, 0x43, 0xF0, 0x20, 0x88, 0x40, 0x00, 0xC0, 0x80, 0x40, 0x03, 0x00, 0x40, 0x40, 0x0C, 0x00, 0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00, 0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00}, toaster2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x3C, 0x07, 0x80, 0x00, 0xE0, 0x7F, 0xE0, 0x03, 0x83, 0xC0, 0x70, 0x0E, 0x0E, 0x07, 0xF8, 0x18, 0x38, 0x3E, 0x18, 0x30, 0xE0, 0xF9, 0xE8, 0x61, 0x83, 0xFF, 0xF8, 0x7B, 0x0F, 0xFF, 0xF8, 0x9E, 0x1B, 0xFF, 0xF0, 0x87, 0xB7, 0xFF, 0xE8, 0xB1, 0xEF, 0xFF, 0xE0, 0xAC, 0x6F, 0x9F, 0x88, 0xA3, 0x5F, 0x60, 0x36, 0x83, 0x5E, 0xFB, 0x6D, 0x8B, 0x5E, 0xF6, 0xDB, 0x80, 0x4A, 0xFD, 0xB6, 0x88, 0x4F, 0x7F, 0xEE, 0x80, 0x4B, 0xBE, 0xFC, 0x88, 0x44, 0xDF, 0xBC, 0x80, 0x43, 0xEF, 0xF8, 0x88, 0x40, 0x07, 0xF0, 0x80, 0x40, 0x03, 0xE0, 0x40, 0x40, 0x0C, 0xC0, 0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00, 0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00}, toast[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x3D, 0xDC, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x07, 0xDA, 0xAF, 0xC0, 0x0F, 0xF4, 0x57, 0xF0, 0x1E, 0xC8, 0x0A, 0xF8, 0x0F, 0xB5, 0x17, 0xF0, 0x03, 0xEA, 0xAD, 0xC8, 0x14, 0xFF, 0x7F, 0x10, 0x0A, 0x3D, 0xDC, 0xA8, 0x05, 0x4F, 0xF1, 0x50, 0x02, 0xA3, 0xEA, 0xA0, 0x00, 0x54, 0x15, 0x00, 0x00, 0x2A, 0xAA, 0x00, 0x00, 0x05, 0x50, 0x00, 0x00, 0x02, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t *const mask[] PROGMEM = {
    toastermask0, toastermask1, toastermask2, toastermask1, toastmask};
const uint8_t *const img[] PROGMEM = {
    toaster0, toaster1, toaster2, toaster1, toast};

unsigned long lastExecutionTime = 0;
int PageNumberToShow = 1;
int LastPageShown = 1;
bool displayedWeather = false;

unsigned long previousMillisFirstMenu = 0;
const long intervalFirstMenu = 1000;

bool isBeingHeld = false;
bool currentInputState = false;
bool previousInputState = false;
long lastDebounceTime = 0;
bool debouncedTouchState = false;

bool lastFirstSeg = false;
bool lastSecondSeg = false;
bool lastThirdSeg = false;
bool lastFourthSeg = false;
bool lastFifthSeg = false;

void checkTouchButtons()
{
    tone(BUZZER_PIN, NOTE_C7, 1000 / 16);
    if (lastFirstSeg || lastSecondSeg)
    {
        Serial.println("changing page down");
        cyclePagesDown();
    }
    else if (lastThirdSeg)
    {
        PageNumberToShow = 1;
        Serial.println("setting first page");
    }
    else if (lastFourthSeg || lastFifthSeg)
    {
        Serial.println("changing page up");
        cyclePagesUp();
    }
}

static unsigned long lastHeldCheck = 0;
const unsigned long debounceDelay = 5; // minimal debounce
const unsigned long heldCheckInterval = 10;
unsigned long lastCycle = 0;
const unsigned long cycleInterneval = 1000;

void showMainPage()
{
    static unsigned long lastDebounceTime = 0;
    touchStates tState = useTouch();

    bool firstSeg = tState == First_Seg;
    bool secondSeg = tState == Second_Seg;
    bool thirdSeg = tState == Third_Seg;
    bool fourthSeg = tState == Fourt_Seg;
    bool fifthSeg = tState == Fifth_Seg;

    bool anySeg = firstSeg || secondSeg || thirdSeg || fourthSeg || fifthSeg;

    if (anySeg && !isBeingHeld)
    {
        if (millis() - lastDebounceTime > debounceDelay)
        {
            lastDebounceTime = millis();
            lastFirstSeg = firstSeg;
            lastSecondSeg = secondSeg;
            lastThirdSeg = thirdSeg;
            lastFourthSeg = fourthSeg;
            lastFifthSeg = fifthSeg;

            debouncedTouchState = true;
            isBeingHeld = true;

            turnOffScreensaver();
            cyclePagesUp();
            lastCycle = millis();

            Serial.println("Held");
        }
    }

    if (isBeingHeld)
    {
        if (millis() - lastHeldCheck >= heldCheckInterval)
        {
            lastHeldCheck = millis();

            bool stillHeld = false;

            if (lastFirstSeg)
                stillHeld |= touchRead(TOUCH_1_Seg_PIN) < (checkPower() ? TOUCH_1_Seg_THRESHOLD : TOUCH_1_Seg_THRESHOLD_BAT);
            if (lastSecondSeg)
                stillHeld |= touchRead(TOUCH_2_Seg_PIN) < (checkPower() ? TOUCH_2_Seg_THRESHOLD : TOUCH_2_Seg_THRESHOLD_BAT);
            if (lastThirdSeg)
                stillHeld |= touchRead(TOUCH_3_Seg_PIN) < (checkPower() ? TOUCH_3_Seg_THRESHOLD : TOUCH_3_Seg_THRESHOLD_BAT);
            if (lastFourthSeg)
                stillHeld |= touchRead(TOUCH_4_Seg_PIN) < (checkPower() ? TOUCH_4_Seg_THRESHOLD : TOUCH_4_Seg_THRESHOLD_BAT);
            if (lastFifthSeg)
                stillHeld |= touchRead(TOUCH_5_Seg_PIN) < (checkPower() ? TOUCH_5_Seg_THRESHOLD : TOUCH_5_Seg_THRESHOLD_BAT);

            if (!stillHeld)
            {
                if (millis() - lastDebounceTime > debounceDelay)
                {
                    debouncedTouchState = false;
                    isBeingHeld = false;
                    Serial.println("Released");
                }
            }
        }
        if (millis() - lastCycle >= cycleInterneval)
        {
            lastCycle = millis();
            turnOffScreensaver();
            checkTouchButtons();
        }
    }

    if (debouncedTouchState && !previousInputState)
    {
        turnOffScreensaver();
        checkTouchButtons();
    }

    if (debouncedTouchState != previousInputState)
        lastDebounceTime = millis();

    previousInputState = debouncedTouchState;

    unsigned long currentTime = millis();
    if (PageNumberToShow == 1 || PageNumberToShow == 2 || PageNumberToShow == 3 || PageNumberToShow == 4 || PageNumberToShow == 5)
    {
        if (currentTime - lastExecutionTime >= MAIN_PAGE_DURATION)
        {
            lastExecutionTime = currentTime;
            PageNumberToShow = 0;
            previousMillisFirstMenu = millis() - intervalFirstMenu;
            displayedWeather = false;
            Serial.println("resetting menus");
            manager.sendOledAction(OLED_STOP_SCROLL);
            setupScreensaver();
        }
        else
        {
            if (PageNumberToShow == 1)
            {
                manager.sendOledAction(OLED_STOP_SCROLL);
                LastPageShown = 1;
                if (currentTime - previousMillisFirstMenu >= intervalFirstMenu)
                {
                    previousMillisFirstMenu = currentTime;
                    showFirstPage();
                }
            }
            else if (PageNumberToShow == 2 && displayedWeather == false)
            {
                displayedWeather = true;
                LastPageShown = 2;
                display.clearDisplay();
                manager.sendOledAction(OLED_DISPLAY);
                currentWeather();
            }
            else if (PageNumberToShow == 3)
            {
                LastPageShown = 3;
                if (currentTime - previousMillisFirstMenu >= intervalFirstMenu)
                {
                    previousMillisFirstMenu = currentTime;
                    showForecastPage();
                }
            }
            else if (PageNumberToShow == 4)
            {
                LastPageShown = 4;
                if (currentTime - previousMillisFirstMenu >= intervalFirstMenu)
                {
                    previousMillisFirstMenu = currentTime;
                    showInfoPage();
                }
            }
            else if (PageNumberToShow == 5)
            {
                LastPageShown = 5;
                if (currentTime - previousMillisFirstMenu >= intervalFirstMenu)
                {
                    previousMillisFirstMenu = currentTime;
                    showSensorPage();
                }
            }
        }
    }
    else
    {
        if (currentTime - lastExecutionTime >= SCREENSAVER_DURATION)
        {
            lastExecutionTime = currentTime;
            Serial.println("turn off screenshaver and cycle pages");
            cyclePages();
        }
        else
        {
            showScreensaver();
        }
    }
}

void turnOffScreensaver()
{
    displayedWeather = false;
    if (LastPageShown != 1)
    {
        manager.sendOledAction(OLED_STOP_SCROLL);
    }

    previousMillisFirstMenu = millis() - intervalFirstMenu;
}

void showFirstPage()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setFont(&DejaVu_Sans_Bold_16);
    centerText(String(day()) + "." + String(month()) + "." + String(year()), SCREEN_HEIGHT / 2 - 10);
    display.setTextSize(1);
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    centerText(getCurrentWeekdayName(), SCREEN_HEIGHT / 2);
    centerText(getCurrentMonthName(), SCREEN_HEIGHT / 2 + 10);
    centerText("Light: " + String(getLightLevel()) + " lux", SCREEN_HEIGHT / 2 + 23);
    display.drawLine(26 - 8, 45, 102 + 8, 45, WHITE);
    manager.sendOledAction(OLED_DISPLAY);
}

auto formatTemperature = [](float minTemp, float maxTemp)
{
    char tempStr[6];
    float avgTemp = (minTemp + maxTemp) / 2.0;
    dtostrf(avgTemp, 4, 1, tempStr);
    return String(tempStr) + "C";
};

void showForecastPage()
{
    int y = 20;
    int x = 5;
    display.clearDisplay();
    delay(10);

    display.setCursor(x + 3, y);
    display.print(getShortNextDay(0));
    display.setCursor(x + 44 + 3, y);
    display.print(getShortNextDay(1));
    display.setCursor(x + 88 + 3, y);
    display.print(getShortNextDay(2));
    displaySmallWidget(weatherDailyForecastData[0].weatherConditionId, x + 0, y);
    displaySmallWidget(weatherDailyForecastData[1].weatherConditionId, x + 44, y);
    displaySmallWidget(weatherDailyForecastData[2].weatherConditionId, x + 88, y);
    display.setFont(&DejaVu_LGC_Sans_Bold_9);
    display.setCursor(x - 3, y + 3 + 40);
    display.print(formatTemperature(weatherDailyForecastData[0].minTemp, weatherDailyForecastData[0].maxTemp));

    display.setCursor(x + 44 - 3, y + 3 + 40);
    display.print(formatTemperature(weatherDailyForecastData[1].minTemp, weatherDailyForecastData[1].maxTemp));

    display.setCursor(x + 88 - 3, y + 3 + 40);
    display.print(formatTemperature(weatherDailyForecastData[2].minTemp, weatherDailyForecastData[2].maxTemp));

    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    centerText(String(day()) + "." + String(month()) + "." + String(year()), 10);
    delay(10);
    manager.sendOledAction(OLED_DISPLAY);
}

void displayWiFiSignal(int x, int y)
{
    int32_t rssi = WiFi.RSSI();

    const uint8_t *wifiIcon;

    if (WiFi.status() != WL_CONNECTED)
    {
        wifiIcon = wifi_off_svgrepo_com_48x48;
    }
    else if (rssi >= -50)
    {
        wifiIcon = wifi_4_svgrepo_com_48x48;
    }
    else if (rssi >= -60)
    {
        wifiIcon = wifi_3_svgrepo_com_48x48;
    }
    else if (rssi >= -70)
    {
        wifiIcon = wifi_2_svgrepo_com_48x48;
    }
    else if (rssi >= -80)
    {
        wifiIcon = wifi_1_svgrepo_com_48x48;
    }
    else
    {
        wifiIcon = wifi_no_signal_svgrepo_com_48x48;
    }

    // Display the selected icon
    display.drawBitmap(x, y, wifiIcon, 48, 48, BLACK, WHITE);
}

void showInfoPage()
{
    display.clearDisplay();
    delay(10);
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    displayWiFiSignal(0, 24);
    if (charging == true)
    {
        display.drawBitmap(32 - 24, 8, battery_charging_full_90deg_24x24, 24, 24, BLACK, WHITE);
    }
    else
    {
        display.drawBitmap(32 - 24, 8, battery_0_bar_90deg_24x24, 24, 24, BLACK, WHITE);
        display.fillRect((32 - 24) + 4, 8 + 9, map(getBatteryPercentage(), 0, 100, 0, 14), 6, SSD1306_WHITE);
    }
    centerText(String(day()) + "." + String(month()) + "." + String(year()), 10);
    display.fillRect(5 + 32, 26, SCREEN_WIDTH, 1, WHITE);
    display.setCursor(48, 37);
    display.println("WiFi SSID:");
    display.setCursor(48, 45);
    display.setFont(&DejaVu_LGC_Sans_Bold_8);
    display.println(String(WiFi.SSID()));
    display.setCursor(48, 55);
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    String signalQuality = getSignalQuality(WiFi.RSSI());
    display.print("Sig:" + signalQuality);
    display.setCursor(48, 64);
    display.print("Channel: " + String(WiFi.channel()));
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    display.setCursor(5 + 32, 25);
    display.println(String(getBatteryPercentage()) + "%");
    display.setCursor(5 + 35 + 50, 25);
    display.print(String(getBatteryVoltage()) + "V");
    delay(10);
    manager.sendOledAction(OLED_DISPLAY);
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
}

void showSensorPage()
{
    display.clearDisplay();
    centerText(String(day()) + "." + String(month()) + "." + String(year()), 10);
    display.drawBitmap(SCREEN_WIDTH - 24 - 10, 18, house_thermometer_24x24, 24, 24, BLACK, WHITE);
    display.setCursor(2, 35);
    String tempText = "Temp: " + String(readTemperature()) + "C";
    display.print(tempText);
    int16_t x, y;
    uint16_t w, h;
    display.getTextBounds(tempText, 0, 0, &x, &y, &w, &h);
    display.drawLine(2, 26, 2 + w, 26, SSD1306_WHITE);
    uint16_t tempLenght = w;
    display.getTextBounds("SHT40 Sens", 0, 0, &x, &y, &w, &h);
    display.setCursor(tempLenght / 2 - w / 2, 24);
    display.print("SHT40 Sens");
    display.setCursor(2, 35 + 24);
    display.print("Hum: " + String(readHumidity()) + "%");
    display.drawBitmap(SCREEN_WIDTH - 24 - 10, 18 + 24, house_raindrops_24x24, 24, 24, BLACK, WHITE);

    manager.sendOledAction(OLED_DISPLAY);
}

void cyclePages()
{
    manager.sendOledAction(OLED_STOP_SCROLL);
    if (LastPageShown == 1)
    {
        PageNumberToShow = 2;
    }
    else if (LastPageShown == 2)
    {
        PageNumberToShow = 3;
    }
    else if (LastPageShown == 3)
    {
        PageNumberToShow = 4;
    }
    else if (LastPageShown == 4)
    {
        PageNumberToShow = 5;
    }
    else if (LastPageShown == 5)
    {
        PageNumberToShow = 1;
    }
}

void cyclePagesDown()
{
    manager.sendOledAction(OLED_STOP_SCROLL);
    if (LastPageShown == 1)
    {
        PageNumberToShow = 2;
    }
    else if (LastPageShown == 2)
    {
        PageNumberToShow = 3;
    }
    else if (LastPageShown == 3)
    {
        PageNumberToShow = 4;
    }
    else if (LastPageShown == 4)
    {
        PageNumberToShow = 5;
    }
    else if (LastPageShown == 5)
    {
        PageNumberToShow = 1;
    }
}

void cyclePagesUp()
{
    manager.sendOledAction(OLED_STOP_SCROLL);
    if (LastPageShown == 1)
    {
        PageNumberToShow = 5;
    }
    else if (LastPageShown == 2)
    {
        PageNumberToShow = 1;
    }
    else if (LastPageShown == 3)
    {
        PageNumberToShow = 2;
    }
    else if (LastPageShown == 4)
    {
        PageNumberToShow = 3;
    }
    else if (LastPageShown == 5)
    {
        PageNumberToShow = 4;
    }
}

struct Flyer
{
    int16_t x, y;
    int8_t depth;
    uint8_t frame;
} flyer[N_FLYERS];

static int compare(const void *a, const void *b)
{
    return ((struct Flyer *)a)->depth - ((struct Flyer *)b)->depth;
}

void setupScreensaver()
{
    display.clearDisplay();
    for (uint8_t i = 0; i < N_FLYERS; i++)
    {
        flyer[i].x = (-32 + random(160)) * 16;
        flyer[i].y = (-32 + random(96)) * 16;
        flyer[i].frame = random(3) ? random(4) : 255;
        flyer[i].depth = 10 + random(16);
    }
    qsort(flyer, N_FLYERS, sizeof(struct Flyer), compare);
}

void showScreensaver()
{
    uint8_t i, f;
    int16_t x, y;
    boolean resort = false;

    display.clearDisplay();

    for (i = 0; i < N_FLYERS; i++ && PageNumberToShow == false)
    {
        delay(3);
        if (inputDetected == true)
        {
            turnOffScreensaver();
            break;
        }

        f = (flyer[i].frame == 255) ? 4 : (flyer[i].frame++ & 3);
        x = flyer[i].x / 16;
        y = flyer[i].y / 16;
        display.drawBitmap(x, y, mask[f], 32, 32, BLACK);
        display.drawBitmap(x, y, img[f], 32, 32, WHITE);

        flyer[i].x -= flyer[i].depth * 2;
        flyer[i].y += flyer[i].depth;
        if ((flyer[i].y >= (64 * 16)) || (flyer[i].x <= (-32 * 16)))
        {
            if (random(7) < 5)
            {
                flyer[i].x = random(160) * 16;
                flyer[i].y = -32 * 16;
            }
            else
            {
                flyer[i].x = 128 * 16;
                flyer[i].y = random(64) * 16;
            }
            flyer[i].frame = random(3) ? random(4) : 255;
            flyer[i].depth = 10 + random(16);
            resort = true;
        }
    }

    if (resort)
    {
        qsort(flyer, N_FLYERS, sizeof(struct Flyer), compare);
    }
    manager.sendOledAction(OLED_DISPLAY);
}
