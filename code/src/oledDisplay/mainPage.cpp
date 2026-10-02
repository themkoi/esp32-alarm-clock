#include "mainPage.h"

#define NUMBER_OF_PAGES 6
void showFirstPage();
void showForecastPage();
void showInfoPage();
void showEnvSensorPage();
void showOptSensorPage();
void showScreensaver();
void setupScreensaver();
void turnOffScreensaver();
void drawPageIndicator(int startX = -1, int startY = 4);

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
    toastermask1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xF8, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80, 0x01, 0xF1, 0xF8, 0x00, 0x07, 0xC7, 0xC1, 0xE0, 0x0F, 0x1F, 0x06, 0x10, 0x1E, 0x7C, 0x00, 0x00, 0x04, 0xF0, 0x07, 0xF0, 0x61, 0xE4, 0x18, 0x08, 0x78, 0x48, 0x20, 0x80, 0x4E, 0x10, 0x40, 0x2B, 0x53, 0x90, 0x81, 0x00, 0x5C, 0xA0, 0x80, 0x58, 0x7C, 0xA1, 0x02, 0x00, 0x74, 0xA1, 0x28, 0xB8, 0x7F, 0xB5, 0x0F, 0xF0, 0x77, 0xB0, 0x90, 0x78, 0x7F, 0xB4, 0x60, 0xF0, 0x77, 0xBB, 0x03, 0xE0, 0x7F, 0xBC, 0x0F, 0xC0, 0x77, 0xBF, 0xFF, 0x00, 0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00, 0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, toastermask2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x7C, 0x3F, 0x80, 0x01, 0xF1, 0xF8, 0x00, 0x07, 0xC7, 0xC1, 0xE0, 0x0F, 0x1F, 0x06, 0x10, 0x1E, 0x7C, 0x00, 0x00, 0x04, 0xF0, 0x00, 0x00, 0x61, 0xE4, 0x00, 0x08, 0x78, 0x48, 0x00, 0x10, 0x4E, 0x10, 0x00, 0x18, 0x53, 0x90, 0x60, 0x70, 0x5C, 0xA0, 0x9F, 0xC8, 0x7C, 0xA1, 0x04, 0x92, 0x74, 0xA1, 0x09, 0x24, 0x7F, 0xB5, 0x02, 0x48, 0x77, 0xB0, 0x80, 0x10, 0x7F, 0xB4, 0x41, 0x00, 0x77, 0xBB, 0x20, 0x40, 0x7F, 0xBC, 0x10, 0x00, 0x77, 0xBF, 0xF8, 0x00, 0x7F, 0xBF, 0xFC, 0x00, 0x3F, 0xBF, 0xF0, 0x00, 0x1F, 0xBF, 0xC0, 0x00, 0x07, 0xBE, 0x00, 0x00, 0x01, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, toastmask[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x0C, 0x30, 0x00, 0x00, 0x30, 0x0C, 0x00, 0x00, 0xC2, 0x23, 0x00, 0x07, 0x00, 0x80, 0xC0, 0x08, 0x25, 0x50, 0x30, 0x10, 0x0B, 0xA8, 0x08, 0x21, 0x37, 0xF5, 0x04, 0x30, 0x4A, 0xE8, 0x0C, 0x3C, 0x15, 0x52, 0x34, 0x2B, 0x00, 0x80, 0xEC, 0x35, 0xC2, 0x23, 0x54, 0x1A, 0xB0, 0x0E, 0xAC, 0x0D, 0x5C, 0x15, 0x58, 0x03, 0xAB, 0xEA, 0xE0, 0x00, 0xD5, 0x55, 0x80, 0x00, 0x3A, 0xAE, 0x00, 0x00, 0x0D, 0x58, 0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t PROGMEM
    toaster0[] = {0x00, 0x30, 0x00, 0x00, 0x00, 0xE8, 0x00, 0x00, 0x01, 0xD8, 0x00, 0x00, 0x03, 0x74, 0xF0, 0x00, 0x07, 0xEF, 0xFE, 0x00, 0x07, 0xBC, 0x07, 0x80, 0x0F, 0xE0, 0x7F, 0xE0, 0x0F, 0x83, 0xC0, 0x70, 0x1E, 0x0E, 0x07, 0x08, 0x18, 0x38, 0x38, 0xFF, 0x30, 0xE0, 0xF7, 0xD4, 0x61, 0x83, 0xEE, 0xFF, 0x7B, 0x0F, 0xDF, 0xA8, 0x9E, 0x1B, 0xDD, 0xFE, 0x87, 0xB7, 0xBF, 0x50, 0xB1, 0xEF, 0xBB, 0xF8, 0xAC, 0x6F, 0x7E, 0x00, 0xA3, 0x5F, 0x6F, 0xA0, 0x83, 0x5E, 0xF8, 0x68, 0x8B, 0x5E, 0xDE, 0xC0, 0x80, 0x4A, 0xF1, 0xC8, 0x88, 0x4F, 0x6F, 0x80, 0x80, 0x4B, 0x9F, 0x08, 0x88, 0x44, 0xFC, 0x10, 0x80, 0x43, 0xF0, 0x20, 0x88, 0x40, 0x00, 0xC0, 0x80, 0x40, 0x03, 0x00, 0x40, 0x40, 0x0C, 0x00, 0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00, 0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00},
    toaster1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xF0, 0x00, 0x00, 0xF7, 0xFE, 0x00, 0x01, 0xBC, 0x07, 0x80, 0x03, 0xE0, 0x7F, 0xE0, 0x07, 0x83, 0xC0, 0x70, 0x0E, 0x0E, 0x07, 0xF8, 0x18, 0x38, 0x3E, 0x18, 0x30, 0xE0, 0xF9, 0xE8, 0x61, 0x83, 0xFF, 0xF8, 0x7B, 0x0F, 0xF8, 0x08, 0x9E, 0x1B, 0xE7, 0xF0, 0x87, 0xB7, 0xDF, 0x7F, 0xB1, 0xEF, 0xBF, 0xD4, 0xAC, 0x6F, 0x7E, 0xFF, 0xA3, 0x5F, 0x7F, 0xA6, 0x83, 0x5E, 0xFD, 0xF8, 0x8B, 0x5E, 0xD7, 0x40, 0x80, 0x4A, 0xF0, 0x08, 0x88, 0x4F, 0x6F, 0x80, 0x80, 0x4B, 0x9F, 0x08, 0x88, 0x44, 0xFC, 0x10, 0x80, 0x43, 0xF0, 0x20, 0x88, 0x40, 0x00, 0xC0, 0x80, 0x40, 0x03, 0x00, 0x40, 0x40, 0x0C, 0x00, 0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00, 0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00},
    toaster2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x3C, 0x07, 0x80, 0x00, 0xE0, 0x7F, 0xE0, 0x03, 0x83, 0xC0, 0x70, 0x0E, 0x0E, 0x07, 0xF8, 0x18, 0x38, 0x3E, 0x18, 0x30, 0xE0, 0xF9, 0xE8, 0x61, 0x83, 0xFF, 0xF8, 0x7B, 0x0F, 0xFF, 0xF8, 0x9E, 0x1B, 0xFF, 0xF0, 0x87, 0xB7, 0xFF, 0xE8, 0xB1, 0xEF, 0xFF, 0xE0, 0xAC, 0x6F, 0x9F, 0x88, 0xA3, 0x5F, 0x60, 0x36, 0x83, 0x5E, 0xFB, 0x6D, 0x8B, 0x5E, 0xF6, 0xDB, 0x80, 0x4A, 0xFD, 0xB6, 0x88, 0x4F, 0x7F, 0xEE, 0x80, 0x4B, 0xBE, 0xFC, 0x88, 0x44, 0xDF, 0xBC, 0x80, 0x43, 0xEF, 0xF8, 0x88, 0x40, 0x07, 0xF0, 0x80, 0x40, 0x03, 0xE0, 0x40, 0x40, 0x0C, 0xC0, 0x20, 0x40, 0x30, 0x00, 0x18, 0x41, 0xC0, 0x00, 0x06, 0x4E, 0x00, 0x00, 0x01, 0xF0, 0x00, 0x00},
    toast[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x3D, 0xDC, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x07, 0xDA, 0xAF, 0xC0, 0x0F, 0xF4, 0x57, 0xF0, 0x1E, 0xC8, 0x0A, 0xF8, 0x0F, 0xB5, 0x17, 0xF0, 0x03, 0xEA, 0xAD, 0xC8, 0x14, 0xFF, 0x7F, 0x10, 0x0A, 0x3D, 0xDC, 0xA8, 0x05, 0x4F, 0xF1, 0x50, 0x02, 0xA3, 0xEA, 0xA0, 0x00, 0x54, 0x15, 0x00, 0x00, 0x2A, 0xAA, 0x00, 0x00, 0x05, 0x50, 0x00, 0x00, 0x02, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t *const mask[] PROGMEM = {
    toastermask0, toastermask1, toastermask2, toastermask1, toastmask};
const uint8_t *const img[] PROGMEM = {
    toaster0, toaster1, toaster2, toaster1, toast};

unsigned long lastExecutionTime = 0;
int PageNumberToShow = 1;
int LastPageShown = 1;

unsigned long previousMillisMenu = 0;
const long intervalMenu = 1000;

unsigned long lastCycle = 0;
const unsigned long cycleInterneval = 1000;

static bool lastTouched = false;
static bool pressLocked = false;

// Frame transition support: a snapshot of what was on screen before the
// current page/screensaver was rendered, plus bookkeeping so the transition
// only runs when the content source actually changes (not on every
// per-second re-render of the same page).
static uint8_t prevFrameBuffer[OLED_FRAME_BYTES];
static int lastTransitionedSource = -1; // 0 = screensaver, 1..6 = page, -1 = none yet
static bool screensaverEntryTransitionPending = false;

static void snapshotPreviousFrame()
{
    oledMana.snapshotFrame(prevFrameBuffer);
}

static void renderWeatherPage()
{
    currentWeather();
    drawPageIndicator(112, 22);
}

static void renderPageSource(bool sourceChanged, void (*renderPage)())
{
    if (sourceChanged)
    {
        snapshotPreviousFrame();
        oledMana.suppressDisplay(true);
    }

    renderPage();

    if (sourceChanged)
    {
        oledMana.startTransition(prevFrameBuffer, oled.getBuffer());
        oledMana.suppressDisplay(false);
        lastTransitionedSource = PageNumberToShow;
    }
    else
    {
        oledMana.display();
    }
}

void drawPageIndicator(int startX, int startY)
{
    oled.setFont(&Roboto_Black_9);
    oled.setTextColor(5);
    oled.setCursor(startX, startY);
    oled.print(String(PageNumberToShow) + "/" + String(NUMBER_OF_PAGES));
    oled.setTextColor(SSD1327_WHITE);
}

void cyclePagesUp()
{
    Serial.println("main page cycling down");
    if (LastPageShown >= NUMBER_OF_PAGES)
    {
        PageNumberToShow = 1;
    }
    else
    {
        PageNumberToShow = LastPageShown + 1;
    }
    LastPageShown = PageNumberToShow;
}

void cyclePagesDown()
{
    Serial.println("main page cycling up");
    if (LastPageShown <= 1)
    {
        PageNumberToShow = NUMBER_OF_PAGES;
    }
    else
    {
        PageNumberToShow = LastPageShown - 1;
    }
    LastPageShown = PageNumberToShow;
}

void manageKeys(bool butoonsPressed[])
{
    int keyPressed = -1;
    for (uint8_t key = 0; key < KEYS_AMMOUNT; ++key)
    {
        if (butoonsPressed[key] == true)
        {
            keyPressed = key;
            break;
        }
    }

    if (keyPressed <= 2)
    {

        cyclePagesUp();
    }
    else if (keyPressed >= 4)
    {
        if (PageNumberToShow == 0)
        {
            PageNumberToShow = LastPageShown;
        }
        else
        {
            cyclePagesDown();
        }
    }
    else
    {
        PageNumberToShow = 1;
        LastPageShown = 1;
    }
}

void showMainPage()
{
#if GESTURES_ENABLED == true

    GestureState g = useGesture();

    bool isGestured = g.detected;

    if (isGestured)
    {
        if (g.gestureType == 4)
        {
            cyclePagesUp();
            Serial.println("Cycling Up");
        }
        else if (g.gestureType == 3)
        {
            cyclePagesDown();
            Serial.println("Cycling Down");
        }
    }

#else
    ProximityState p = useProximity();
    bool isProximity = p.detected;

    if (isProximity == true)
    {
        if (PageNumberToShow == 0)
        {
            cyclePagesUp();
        }

        turnOffScreensaver();
    }

#endif

    touchState t = useTouch();

    bool isTouched = t.touched;

    bool pressed = isTouched && !pressLocked;
    bool released = !isTouched;

    if (pressed)
    {
        pressLocked = true;

        turnOffScreensaver();
        manageKeys(t.butoonsPressed);
        lastCycle = millis();

        Serial.println("Pressed");
    }

    if (isTouched)
    {
        if (millis() - lastCycle >= cycleInterneval)
        {
            lastCycle = millis();
            turnOffScreensaver();
            manageKeys(t.butoonsPressed);
        }
    }

    if (released)
    {
        pressLocked = false;
    }

    lastTouched = isTouched;

    unsigned long currentTime = millis();
    if (PageNumberToShow >= 1 && PageNumberToShow <= NUMBER_OF_PAGES)
    {
        if (currentTime - lastExecutionTime >= MAIN_PAGE_DURATION)
        {
            lastExecutionTime = currentTime;
            PageNumberToShow = 0;
            previousMillisMenu = millis() - intervalMenu;
            Serial.println("resetting menus");
            snapshotPreviousFrame();
            screensaverEntryTransitionPending = true;
            lastTransitionedSource = 0;
            setupScreensaver();
        }
        else
        {
            bool sourceChanged = (PageNumberToShow != lastTransitionedSource);

            switch (PageNumberToShow)
            {
            case 1:
                if (currentTime - previousMillisMenu >= intervalMenu)
                {
                    previousMillisMenu = currentTime;
                    renderPageSource(sourceChanged, showFirstPage);
                }
                break;

            case 2:
                if (currentTime - previousMillisMenu >= intervalMenu)
                {
                    previousMillisMenu = currentTime;
                    renderPageSource(sourceChanged, renderWeatherPage);
                }
                break;

            case 3:
                if (currentTime - previousMillisMenu >= intervalMenu)
                {
                    previousMillisMenu = currentTime;
                    renderPageSource(sourceChanged, showForecastPage);
                }
                break;

            case 4:
                if (currentTime - previousMillisMenu >= intervalMenu)
                {
                    previousMillisMenu = currentTime;
                    renderPageSource(sourceChanged, showInfoPage);
                }
                break;

            case 5:
                if (currentTime - previousMillisMenu >= intervalMenu)
                {
                    previousMillisMenu = currentTime;
                    renderPageSource(sourceChanged, showEnvSensorPage);
                }
                break;
            case 6:
                if (currentTime - previousMillisMenu >= intervalMenu)
                {
                    previousMillisMenu = currentTime;
                    renderPageSource(sourceChanged, showOptSensorPage);
                }
                break;

            default:
                break;
            }
        }
    }
    else
    {
        if (currentTime - lastExecutionTime >= SCREENSAVER_DURATION)
        {
            lastExecutionTime = currentTime;
            Serial.println("turn off screensaver and cycle pages");
            cyclePagesUp();
        }
        else
        {
            showScreensaver();
        }
    }
}

void turnOffScreensaver()
{
    previousMillisMenu = millis() - intervalMenu;
}

void showFirstPage()
{
    oled.clearDisplay();

    drawPageIndicator(110, 13);

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(SSD1327_WHITE);

    String headerText = getCurrentMonthName() + " " + String(year());
    centerText(headerText, 14);

    oled.drawLine(0, 20, 127, 20, 6);
    oled.setFont(&DejaVu_Sans_Bold_16);
    oled.setTextColor(SSD1327_WHITE);
    centerText(getCurrentWeekdayName(), 46);

    oled.setFont(&DejaVu_LGC_Sans_Bold_10);
    oled.setTextColor(12);
    centerText(String(day()) + " " + getCurrentMonthName(), 65);

    String dayStr = (day() < 10 ? "0" : "") + String(day());
    String monthStr = (month() < 10 ? "0" : "") + String(month());
    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(6);
    centerText(dayStr + "." + monthStr + "." + String(year()), 82);

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(12);
    centerText(formatTimeUntilNextAlarm(), 98);

    oled.drawLine(0, 112, 127, 112, 6);

    oled.setFont(&Roboto_Black_9);

    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(4, 123);
    oled.print("Bat: ");
    oled.setTextColor(12);
    oled.print(getBatteryPercentage());
    oled.setTextColor(6);
    oled.print("%");

    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(72, 123);
    if (WiFi.status() == WL_CONNECTED)
    {
        oled.print("WiFi: ");
        oled.setTextColor(12);
        oled.print(getSignalQuality(WiFi.RSSI()));
    }
    else
    {
        oled.setTextColor(12);
        oled.print("Offline");
    }

    delay(10);
    oledMana.display();

    oled.setFont(&DejaVu_LGC_Sans_Bold_10);
}

void showForecastPage()
{
    oled.clearDisplay();

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(10, 10);
    oled.print(String(day()) + "." + String(month()) + "." + String(year()));

    drawPageIndicator(110, 10);

    oled.drawLine(0, 14, 127, 14, 6);

    const int colWidth = 42;

    for (int i = 0; i < 3; i++)
    {
        int colX = i * colWidth + 1;

        if (i > 0)
        {
            oled.drawLine(colX - 1, 15, colX - 1, 114, 4);
        }

        oled.setFont(&DejaVu_LGC_Sans_Bold_9);
        oled.setTextColor(SSD1327_WHITE);
        oled.setCursor(colX + 4, 24);
        oled.print(getShortNextDay(i));

        displaySmallWidget(weatherDailyForecastData[i].weatherConditionId, colX + 1, 24);

        float tmax = weatherDailyForecastData[i].maxTemp;
        float tmin = weatherDailyForecastData[i].minTemp;
        int popVal = weatherDailyForecastData[i].pop;

        oled.setFont(&DejaVu_LGC_Sans_Bold_9);

        // H: High Temperature (hides unit if >= 3 digits)
        oled.setCursor(colX + 2, 70);
        oled.setTextColor(SSD1327_WHITE);
        oled.print("H:");
        int tmaxVal = (int)round(tmax);
        oled.print(tmaxVal);
        if (abs(tmaxVal) < 100)
        {
            oled.setTextColor(6);
            oled.print("°C");
        }

        // L: Low Temperature (hides unit if >= 3 digits)
        oled.setCursor(colX + 2, 81);
        oled.setTextColor(SSD1327_WHITE);
        oled.print("L:");
        int tminVal = (int)round(tmin);
        oled.print(tminVal);
        if (abs(tminVal) < 100)
        {
            oled.setTextColor(6);
            oled.print("°C");
        }

        // P: Probability of Precipitation (hides unit if >= 3 digits)
        oled.setCursor(colX + 2, 92);
        oled.setTextColor(SSD1327_WHITE);
        oled.print("P:");
        oled.print(popVal);
        if (popVal < 100)
        {
            oled.setTextColor(6);
            oled.print("%");
        }

        // Wind Speed (hides unit if >= 3 digits)
        oled.setCursor(colX + 2, 103);
        oled.setTextColor(SSD1327_WHITE);
        float windVal = weatherDailyForecastData[i].windSpeed;
        oled.print(windVal, 0);
        if (windVal < 100.0f)
        {
            oled.setTextColor(6);
            oled.print("m/s");
        }
    }

    oled.drawLine(0, 114, 127, 114, 6);

    oled.setFont(&Roboto_Black_9);
    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(2, 124);

    if (isWeatherAvailable)
    {
        oled.print(getShortNextDay(1) + ": " + String(weatherConditionIdToStr(weatherDailyForecastData[1].weatherConditionId)));
    }
    else
    {
        oled.print("N/A");
    }

    delay(10);
    oledMana.display();

    oled.setFont(&DejaVu_LGC_Sans_Bold_10);
}

void displayWiFiSignal(int x, int y)
{
    int32_t rssi = WiFi.RSSI();

    const uint8_t *wifiIcon;

    if (WiFi.status() != WL_CONNECTED)
    {
        wifiIcon = wifi_slash_duotone_48x48;
    }
    else if (rssi >= -50)
    {
        wifiIcon = wifi_high_duotone_48x48;
    }
    else if (rssi >= -60)
    {
        wifiIcon = wifi_medium_duotone_48x48;
    }
    else if (rssi >= -70)
    {
        wifiIcon = wifi_low_duotone_48x48;
    }
    else if (rssi >= -80)
    {
        wifiIcon = wifi_none_duotone_48x48;
    }
    else
    {
        wifiIcon = wifi_x_duotone_48x48;
    }

    oled.drawGrayscaleBitmap(x, y, wifiIcon, 16, 16);
}

void showInfoPage()
{
    oled.clearDisplay();

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(10, 10);
    oled.print(String(day()) + "." + String(month()) + "." + String(year()));

    drawPageIndicator(110, 10);

    oled.drawLine(0, 14, 127, 14, 6);

    if (charging == true)
    {
        oled.drawGrayscaleBitmap(4, 16, battery_charging_duotone_24x24, 24, 24);
    }
    else
    {
        oled.drawGrayscaleBitmap(4, 16, battery_empty_duotone_24x24, 24, 24);
        oled.fillRect(8, 25, map(getBatteryPercentage(), 0, 100, 0, 14), 6, SSD1327_WHITE);
    }

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setCursor(34, 26);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Batt: ");
    oled.print(getBatteryPercentage());
    oled.setTextColor(6);
    oled.print("%");
    oled.setTextColor(SSD1327_WHITE);

    oled.setCursor(34, 37);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Volt: ");
    oled.print(getBatteryVoltage(), 2);
    oled.setTextColor(6);
    oled.print("V");
    oled.setTextColor(SSD1327_WHITE);

    if (charging)
    {
        oled.setCursor(95, 26);
        oled.setTextColor(12);
        oled.print("CHG");
    }

    oled.drawLine(0, 42, 127, 42, 6);

    displayWiFiSignal(4, 46);

    oled.setTextColor(SSD1327_WHITE);
    oled.setFont(&DejaVu_LGC_Sans_Bold_9);

    oled.setCursor(34, 53);
    oled.print("SSID:");

    oled.setFont(&DejaVu_LGC_Sans_Bold_8);
    oled.setCursor(34, 63);
    oled.setTextColor(12);
    oled.print(WiFi.SSID());

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(2, 77);
    oled.print("Signal: ");
    int rssi = WiFi.RSSI();
    int percent = (rssi <= -100) ? 0 : (rssi >= -50) ? 100
                                                     : 2 * (rssi + 100);

    oled.setTextColor(12);
    oled.print(percent);
    oled.setTextColor(6);
    oled.print("% " + String(rssi) + "dBm");

    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(2, 88);
    oled.print("Channel: ");
    oled.setTextColor(12);
    oled.print(WiFi.channel());

    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(2, 99);
    oled.print("IP: ");
    oled.setTextColor(12);
    oled.print(WiFi.localIP().toString());

    oled.drawLine(0, 114, 127, 114, 6);

    oled.setFont(&Roboto_Black_9);
    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(2, 124);

    if (WiFi.status() == WL_CONNECTED)
    {
        oled.print("Status: Connected");
    }
    else
    {
        oled.print("Status: Disconnected");
    }

    delay(10);
    oledMana.display();

    oled.setFont(&DejaVu_LGC_Sans_Bold_10);
}

void showEnvSensorPage()
{
    oled.clearDisplay();

    drawPageIndicator(110, 10);

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(SSD1327_WHITE);
    centerText(String(day()) + "." + String(month()) + "." + String(year()), 10);

    oled.drawLine(0, 14, 127, 14, 6);

    oled.setCursor(2, 24);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("-Hum Sensor-");

    int ySens1Temp = 36;
    oled.setCursor(2, ySens1Temp);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Temp: ");
    oled.setTextColor(12);
    oled.print(readTemperature(), 1);
    oled.setTextColor(6);
    oled.print("°C");

    oled.drawGrayscaleBitmap(110, ySens1Temp - 11, thermometer_cold_duotone_24x24, 16, 16);

    int ySens1Hum = 48;
    oled.setCursor(2, ySens1Hum);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Hum:  ");
    oled.setTextColor(12);
    oled.print(readHumidity(), 1);
    oled.setTextColor(6);
    oled.print("%");

    oled.drawGrayscaleBitmap(110, ySens1Hum - 11, drop_simple_duotone_24x24, 16, 16);

    oled.drawLine(0, 54, 127, 54, 6);

    oled.setCursor(2, 64);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("-Press Sensor-");

    int yPress = 76;
    oled.setCursor(2, yPress);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("P: ");
    oled.setTextColor(12);
    oled.print(readPressure(), 1);
    oled.setTextColor(6);
    oled.print("hPa");

    oled.drawGrayscaleBitmap(110, yPress - 11, drop_simple_duotone_24x24, 16, 16);

    int yAlt = 88;
    oled.setCursor(2, yAlt);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Alt: ");
    oled.setTextColor(12);
    oled.print((int)readAltitude());
    oled.setTextColor(6);
    oled.print("m");

    oled.drawGrayscaleBitmap(110, yAlt - 11, drop_simple_duotone_24x24, 16, 16);

    int yBmpTemp = 100;
    oled.setCursor(2, yBmpTemp);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Temp: ");
    oled.setTextColor(12);
    oled.print(readTemperatureBMP(), 1);
    oled.setTextColor(6);
    oled.print("°C");

    oled.drawGrayscaleBitmap(110, yBmpTemp - 11, thermometer_hot_duotone_24x24, 16, 16);

    oled.setTextColor(SSD1327_WHITE);
    delay(10);
    oledMana.display();
    oled.setFont(&DejaVu_LGC_Sans_Bold_10);
}

void showOptSensorPage()
{
    oled.clearDisplay();

    drawPageIndicator(110, 10);

    oled.setFont(&DejaVu_LGC_Sans_Bold_9);
    oled.setTextColor(SSD1327_WHITE);
    centerText(String(day()) + "." + String(month()) + "." + String(year()), 10);

    oled.drawLine(0, 14, 127, 14, 6);

    ColorData rgbData = {0, 0, 0, 0};
    bool colorDataValid = readColorData(rgbData);

    float proxDist = readProximityDistance();
    uint16_t colorTemp = colorDataValid ? readColorTemperature(rgbData) : 0;

    oled.setCursor(2, 24);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("-ALS Light Sensor-");

    int yAlsLight = 36;
    oled.setCursor(2, yAlsLight);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Light: ");
    oled.setTextColor(12);
    oled.print(getLightLevel(), 1);
    oled.setTextColor(6);
    oled.print("lux");

    oled.drawGrayscaleBitmap(110, yAlsLight - 11, thermometer_cold_duotone_24x24, 16, 16);

    oled.drawLine(0, 42, 127, 42, 6);

    oled.setTextColor(SSD1327_WHITE);
    oled.setCursor(2, 52);
    oled.print("-RGB & Prox Sensor-");

    int yProx = 64;
    oled.setCursor(2, yProx);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Dist: ");
    oled.setTextColor(12);
    if (proxDist < 0.0f)
    {
        oled.print("---");
    }
    else
    {
        oled.print(proxDist, 1);
        oled.setTextColor(6);
        oled.print("cm");
    }

    oled.drawGrayscaleBitmap(110, yProx - 11, drop_simple_duotone_24x24, 16, 16);

    int yColorTemp = 76;
    oled.setCursor(2, yColorTemp);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("CTemp: ");
    oled.setTextColor(12);
    oled.print(colorTemp);
    oled.setTextColor(6);
    oled.print("K");

    oled.drawGrayscaleBitmap(110, yColorTemp - 11, thermometer_hot_duotone_24x24, 16, 16);

    int yRGB = 88;
    oled.setCursor(2, yRGB);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("RGB: ");
    oled.setTextColor(12);
    if (colorDataValid)
    {
        oled.print("R" + String(rgbData.red) + " G" + String(rgbData.green) + " B" + String(rgbData.blue));
    }
    else
    {
        oled.print("No Data");
    }

    int yClear = 100;
    oled.setCursor(2, yClear);
    oled.setTextColor(SSD1327_WHITE);
    oled.print("Clear: ");
    oled.setTextColor(12);
    oled.print(rgbData.ambient);

    oled.setTextColor(SSD1327_WHITE);
    delay(10);
    oledMana.display();
    oled.setFont(&DejaVu_LGC_Sans_Bold_10);
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
    oled.clearDisplay();
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

    oled.clearDisplay();

    for (i = 0; i < N_FLYERS; i++)
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
        oled.drawBitmap(x, y, mask[f], 32, 32, SSD1327_BLACK);
        oled.drawBitmap(x, y, img[f], 32, 32, SSD1327_WHITE);

        flyer[i].x -= flyer[i].depth * 2;
        flyer[i].y += flyer[i].depth;
        if ((flyer[i].y >= (128 * 16)) || (flyer[i].x <= (-32 * 16)))
        {
            if (random(7) < 5)
            {
                flyer[i].x = random(160) * 16;
                flyer[i].y = -32 * 16;
            }
            else
            {
                flyer[i].x = 128 * 16;
                flyer[i].y = random(128) * 16;
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

    if (screensaverEntryTransitionPending)
    {
        screensaverEntryTransitionPending = false;
        oledMana.startTransition(prevFrameBuffer, oled.getBuffer());
    }
    else
    {
        oledMana.display();
    }
}