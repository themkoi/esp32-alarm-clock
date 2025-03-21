#include "interface.h"

#define BUTTONS_OFFSET 1
#define BUTTON_HEIGHT 10

struct entryMenu
{
    String text;
    void (*function)();
    void (*loopFunction)();
    struct Submenu *submenu;
    bool *boolPtr;
    void (*boolToggleFunction)(bool *);
};

struct Submenu
{
    String name;
    entryMenu *entries;
    int count;
    int maxMenus;
};

// Global menu data
struct menuData
{
    int totalMenus;
    int textSize;
    String menuName;
    int linesThick;
    entryMenu entryList[MAX_MENU_ITEMS];
    int itemsOnPage;
    int currentButton;
    bool isSubmenu;
    entryMenu *currentSubmenu;
    int submenuCount;
};

menuData data = {};
int currentMenuItem = 0;
int pageNumber = 0;
int currentPage = 0;

#define MAX_STACK_SIZE 10
Submenu *menuStack[MAX_STACK_SIZE];
int stackPointer = -1;

unsigned long lastInputTime = 0;
bool startedLoop = false;

bool menuRunning = true;

bool exitLoopFunction = false;

void resetPreviousItems()
{
    display.clearDisplay();
}

void showMenu() 
{
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    display.clearDisplay();
    display.setTextSize(data.textSize);

    int maxItems = 4;
    int minItems = 1;
    int availableHeight = SCREEN_HEIGHT - 20;
    int usedHeight = 0;
    int tmpItemsOnPage = maxItems;

for (int i = 0; i < maxItems && i < (data.isSubmenu ? data.submenuCount : data.totalMenus); i++)
{
    String text = data.isSubmenu ? data.currentSubmenu[i].text : data.entryList[i].text;
    if ((data.isSubmenu && data.currentSubmenu[i].boolPtr) || (!data.isSubmenu && data.entryList[i].boolPtr))
    {
        text += " " + String(*(data.isSubmenu ? data.currentSubmenu[i].boolPtr : data.entryList[i].boolPtr) ? "True" : "False");
    }

    int16_t x1, y1;
    uint16_t textWidth, textHeight;
    display.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);

    int lineHeight = (textWidth > SCREEN_WIDTH) ? (textHeight * 2) : textHeight;

    if (usedHeight + lineHeight > availableHeight)
    {
        tmpItemsOnPage = max(minItems, i);
        break;
    }
    usedHeight += lineHeight + BUTTONS_OFFSET;
}

    data.itemsOnPage = tmpItemsOnPage;

    int startingButton = data.currentButton - (data.currentButton % data.itemsOnPage);
    currentPage = data.currentButton / data.itemsOnPage;
    pageNumber = ((data.isSubmenu ? data.submenuCount : data.totalMenus) + data.itemsOnPage - 1) / data.itemsOnPage;

    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 10);
    display.print(String(currentPage + 1) + "/" + String(pageNumber));

    centerText(data.menuName, 10);

    int y = 20;
    for (int i = startingButton; i < startingButton + data.itemsOnPage && i < (data.isSubmenu ? data.submenuCount : data.totalMenus); i++)
    {
        String displayText = data.isSubmenu ? data.currentSubmenu[i].text : data.entryList[i].text;
        if ((data.isSubmenu && data.currentSubmenu[i].boolPtr) || (!data.isSubmenu && data.entryList[i].boolPtr))
        {
            displayText += " " + String(*(data.isSubmenu ? data.currentSubmenu[i].boolPtr : data.entryList[i].boolPtr) ? "True" : "False");
        }

        int16_t x1, y1;
        uint16_t textWidth, textHeight;
        display.getTextBounds(displayText, 0, 0, &x1, &y1, &textWidth, &textHeight);
        Serial.println("width:" + String(textWidth));
        Serial.println("height:" + String(textHeight));

        float floatLines = textHeight / BUTTON_HEIGHT;
        int lines = round(floatLines);
        if (lines == 0)
        {
            lines++;
        }
        

        int boxHeight = (lines * BUTTON_HEIGHT) + (lines * BUTTONS_OFFSET); 
        int boxY = y - 8;

        if (data.currentButton == i)
        {
            display.fillRect(0, boxY, SCREEN_WIDTH, boxHeight, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        }
        else
        {
            display.setTextColor(SSD1306_WHITE);
        }

        display.setCursor(4, y);
        display.print(displayText);

        y += boxHeight + BUTTONS_OFFSET;
    }

    manager.oledDisplay();
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    display.setTextColor(SSD1306_WHITE);
}


void initMenu(entryMenu *entryList, int totalMenus, String menuName, int textSize, int linesThick)
{
    int realTotalMenus = 0;
    for (int i = 0; i < totalMenus; i++)
    {
        if (entryList[i].text != "")
        {
            data.entryList[realTotalMenus] = entryList[i];
            realTotalMenus++;
        }
    }

    data.totalMenus = realTotalMenus;
    data.textSize = textSize;
    data.menuName = menuName;
    data.linesThick = linesThick;
    data.currentButton = 0;
    data.itemsOnPage = (textSize == 1) ? 8 : 5;
    data.isSubmenu = false;
    data.currentSubmenu = nullptr;
    data.submenuCount = 0;

    showMenu();
}

void updateLastInputTime()
{
    lastInputTime = millis();
}

static bool timerActive = false; // Track if the timer is active

enum IdleState
{
    IDLE,
    ANIMATING
};

IdleState currentState = IDLE;
unsigned long lastAnimationTime = 0;
const unsigned long animationInterval = 1; // Adjust as needed
bool idleEnabled = true;

void startIdleAnimation()
{
    if (idleEnabled)
    {
        if (currentState == IDLE && millis() - lastInputTime > MENU_TIMEOUT)
        {
            currentState = ANIMATING;
            Serial.println("Main Page started...");
            lastAnimationTime = millis();
        }

        if (currentState == ANIMATING)
        {
            if (millis() - lastAnimationTime >= animationInterval)
            {
                showMainPage();
                lastAnimationTime = millis();
            }

            if (buttons.checkButtonInput())
            {
                updateLastInputTime();
                currentState = IDLE;
            }
        }
    }
}

void exitSubmenu()
{
    if (data.isSubmenu && stackPointer >= 0)
    {
        stackPointer--;
        data.currentButton = 0;

        if (stackPointer >= 0)
        {
            Submenu *prevSubmenu = menuStack[stackPointer];
            data.currentSubmenu = prevSubmenu->entries;
            data.submenuCount = prevSubmenu->count;
            data.menuName = prevSubmenu->name;
            data.isSubmenu = true;
        }
        else
        {
            data.currentSubmenu = nullptr;
            data.menuName = "Main Menu";
            data.isSubmenu = false;
        }

        showMenu();
        Serial.println("Exited submenu, now in: " + data.menuName);
    }
    else
    {
        Serial.println("No submenu to exit.");
        idleEnabled = true;
        timerActive = true;
        displayedWeather = false;
        lastInputTime = millis();
        lastInputTime = lastInputTime - 10001111;
        currentState = IDLE;
        startIdleAnimation();
    }
}

void pushSubmenu(Submenu *submenu)
{
    if (stackPointer < 9)
    {
        stackPointer++;
        menuStack[stackPointer] = submenu;
    }
}

bool shouldExitLoop()
{
    if (buttons.checkButtonInput())
    {
        lastInputTime = millis();
    }

    if (millis() - lastInputTime > LOOP_FUNCTION_TIMEOUT_MS || exitLoopFunction == true || buttons.checkExit())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void runLoopFunction(void (*loopFunction)())
{
    unsigned long lastInputTime = millis();
    exitLoopFunction = false;

    while (true)
    {
        loopFunction();

        if (exitLoopFunction == true)
        {
            break;
        }

        delay(10);
    }

    showMenu();
}

void handleConfirm()
{
    if (startedLoop == true)
    {
        if (data.isSubmenu && data.currentSubmenu != nullptr)
        {
            entryMenu selectedEntry = data.currentSubmenu[data.currentButton];

            if (selectedEntry.function != nullptr)
            {
                selectedEntry.function();
                delay(1);
            }

            if (selectedEntry.submenu != nullptr)
            {
                pushSubmenu(selectedEntry.submenu);
                data.currentSubmenu = selectedEntry.submenu->entries;
                data.submenuCount = selectedEntry.submenu->count;
                data.menuName = selectedEntry.submenu->name;
                data.isSubmenu = true;
                data.currentButton = 0;
                showMenu();
            }
            else if (selectedEntry.boolToggleFunction != nullptr)
            {
                selectedEntry.boolToggleFunction(selectedEntry.boolPtr);
                showMenu();
            }
            else if (selectedEntry.loopFunction != nullptr)
            {
                runLoopFunction(selectedEntry.loopFunction);
            }
        }
        else if (data.entryList[data.currentButton].function != nullptr)
        {
            data.entryList[data.currentButton].function();

            if (data.entryList[data.currentButton].submenu != nullptr)
            {
                pushSubmenu(data.entryList[data.currentButton].submenu);
                data.currentSubmenu = data.entryList[data.currentButton].submenu->entries;
                data.submenuCount = data.entryList[data.currentButton].submenu->count;
                data.menuName = data.entryList[data.currentButton].submenu->name;
                data.isSubmenu = true;
                data.currentButton = 0;
                showMenu();
            }
        }
        else if (data.entryList[data.currentButton].submenu != nullptr)
        {
            pushSubmenu(data.entryList[data.currentButton].submenu);
            data.currentSubmenu = data.entryList[data.currentButton].submenu->entries;
            data.submenuCount = data.entryList[data.currentButton].submenu->count;
            data.menuName = data.entryList[data.currentButton].submenu->name;
            data.isSubmenu = true;
            data.currentButton = 0;
            showMenu();
        }
        else if (data.entryList[data.currentButton].boolToggleFunction != nullptr)
        {
            data.entryList[data.currentButton].boolToggleFunction(data.entryList[data.currentButton].boolPtr);
            showMenu();
        }
        else if (data.entryList[data.currentButton].loopFunction != nullptr)
        {
            runLoopFunction(data.entryList[data.currentButton].loopFunction);
        }
    }
}

void loopMenu()
{
    // Check for button actions regardless of timer state
    if (buttons.checkUp())
    {
        data.currentButton = max(data.currentButton - 1, 0);
        showMenu();
        lastInputTime = millis(); // Reset last input time
        timerActive = false;      // Stop the timer
    }

    if (buttons.checkDown())
    {
        data.currentButton = min(data.currentButton + 1, (data.isSubmenu ? data.submenuCount - 1 : data.totalMenus - 1));
        showMenu();
        lastInputTime = millis(); // Reset last input time
        timerActive = false;      // Stop the timer
    }

    if (buttons.checkConfirm())
    {
        handleConfirm();
        lastInputTime = millis(); // Reset last input time
        timerActive = false;      // Stop the timer
    }

    if (buttons.checkExit())
    {
        timerActive = false;      // Stop the timer
        lastInputTime = millis(); // Reset last input time
        buttons.checkButtonInput();
        exitSubmenu();
    }

    // If no button is pressed, check for idle timeout
    if (!(buttons.checkButtonInput()))
    {
        // No button is pressed, check for idle timeout
        if (!timerActive)
        {
            lastInputTime = millis(); // Start the timer
            timerActive = true;
        }
        else
        {
            // If the timer is active, check the elapsed time
            if (millis() - lastInputTime >= MENU_TIMEOUT)
            {
                // Call startIdleAnimation continuously after 10 seconds of inactivity
                startIdleAnimation();
                menuRunning = false;
            }
        }
    }
    else
    {
        // If any button is pressed, reset the timer and show the menu
        lastInputTime = millis(); // Reset the last input time
        timerActive = false;      // Stop the timer
        menuRunning = true;
        showMenu(); // Show the menu
    }
}

void addMenuEntry(entryMenu entry)
{
    if (data.isSubmenu && data.currentSubmenu != nullptr)
    {
        Submenu *prevSubmenu = menuStack[stackPointer + 1];

        if (data.submenuCount < prevSubmenu->maxMenus)
        {
            data.currentSubmenu[data.submenuCount++] = entry;
            showMenu();
            Serial.println("Added entry to submenu: " + entry.text);
        }
        else
        {
            Serial.println("Failed to add entry to submenu: Max menu items reached.");
        }
    }
    else
    {
        if (data.totalMenus < MAX_MENU_ITEMS)
        {
            data.entryList[data.totalMenus++] = entry;
            showMenu();
            Serial.println("Added entry to main menu: " + entry.text);
        }
        else
        {
            Serial.println("Failed to add entry to main menu: Max menu items reached.");
        }
    }
}

void removeMenuEntry(int index)
{
    if (data.isSubmenu && data.currentSubmenu != nullptr)
    {
        if (index >= 0 && index < data.submenuCount)
        {
            for (int i = index; i < data.submenuCount - 1; i++)
            {
                data.currentSubmenu[i] = data.currentSubmenu[i + 1];
            }
            data.submenuCount--;
            showMenu();
            Serial.println("Removed entry from submenu at index: " + String(index));
        }
    }
    else
    {
        if (index >= 0 && index < data.totalMenus)
        {
            for (int i = index; i < data.totalMenus - 1; i++)
            {
                data.entryList[i] = data.entryList[i + 1];
            }
            data.totalMenus--;
            showMenu();
            Serial.println("Removed entry from main menu at index: " + String(index));
        }
    }
}

void editCurrentMenuEntry(String newText, void (*newFunction)() = nullptr, void (*newLoopFunction)() = nullptr)
{
    entryMenu *currentEntry;
    if (data.isSubmenu && data.currentSubmenu != nullptr)
    {
        currentEntry = &data.currentSubmenu[data.currentButton];
    }
    else
    {
        currentEntry = &data.entryList[data.currentButton];
    }

    currentEntry->text = newText;

    if (newFunction != nullptr)
        currentEntry->function = newFunction;

    if (newLoopFunction != nullptr)
        currentEntry->loopFunction = newLoopFunction;

    showMenu();
}

Submenu *createSubmenu(const String &name, int maxMenus, void (*function)() = nullptr)
{
    Submenu *submenu = new Submenu;
    submenu->name = name;
    submenu->entries = new entryMenu[maxMenus];
    submenu->count = 0;
    submenu->maxMenus = maxMenus;
    return submenu;
}

bool addEntryToSubmenu(Submenu *submenu, const String &text,
                       void (*function)() = nullptr,
                       void (*loopFunction)() = nullptr,
                       bool *boolPtr = nullptr,
                       void (*boolToggleFunction)(bool *) = nullptr)
{
    if (submenu->count >= submenu->maxMenus)
    {
        Serial.println("Submenu is full. Cannot add more entries.");
        return false;
    }
    submenu->entries[submenu->count].text = text;
    submenu->entries[submenu->count].function = function;
    submenu->entries[submenu->count].loopFunction = loopFunction;
    submenu->entries[submenu->count].submenu = nullptr; // Initialize as no submenu
    submenu->entries[submenu->count].boolPtr = boolPtr;
    submenu->entries[submenu->count].boolToggleFunction = boolToggleFunction;
    submenu->count++;
    return true;
}

uint8_t indexList[MAX_ALARMS];

void deleteAlarm(int index)
{
    alarms[index].exists = false;
    showMenu();
}

void deleteAlarmStatic(int index);
void changeAlarmDay();

Submenu *createAlarmsMenu()
{
    Submenu *alarmsSubmenu = nullptr;
    if (!alarmsSubmenu)
    {
        alarmsSubmenu = createSubmenu("Alarms", MAX_ALARMS + 1); // +1 for the add alarm mhm

        // Add "Add New Alarm" option
        addEntryToSubmenu(alarmsSubmenu, "Add New Alarm", addNewAlarm);
    }
    return alarmsSubmenu;
}

Submenu *alarmsSubmenu = createAlarmsMenu();

bool AlarmMenuUpdate = true;

void manageAlarms()
{
    static uint8_t currentState = 0;
    uint8_t alarmIndex = alarmsSubmenu->entries[data.currentButton].text.toInt();
    static bool isEditing = false;

    auto drawMenuOption = [&](const String &label, int16_t x, int16_t y, bool selected, bool editing)
    {
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(label, x, y, &x1, &y1, &w, &h);

        if (editing)
        {
            display.drawRect(x1 - 2, y1 - 2, w + 4, h + 4, WHITE);
        }
        else if (selected)
        {
            display.fillRect(x1 - 2, y1 - 2, w + 4, h + 4, WHITE);
            display.setTextColor(BLACK, WHITE);
        }
        else
        {
            display.setTextColor(WHITE, BLACK);
        }

        display.setCursor(x, y);
        display.print(label);
    };

    auto drawBitmapOption = [&](int16_t x, int16_t y, bool selected, bool editing)
    {
        if (editing)
        {
            display.drawRect(x - 4, y - 4, 26, 26, WHITE);
            display.drawBitmap(x, y, remove_18x18, 18, 18, WHITE);
        }
        else if (selected)
        {
            display.drawRect(x - 4, y - 4, 26, 26, WHITE);
            display.drawBitmap(x, y, remove_18x18, 18, 18, WHITE);
        }
        else
        {
            display.drawBitmap(x, y, remove_18x18, 18, 18, BLACK, WHITE);
        }
    };

    auto updateAlarmValue = [&]()
    {
        if (buttons.checkUp())
        {
            AlarmMenuUpdate = true;
            if (currentState == 0)
                alarms[alarmIndex].hours = (alarms[alarmIndex].hours + 1) % 24;
            else if (currentState == 1)
                alarms[alarmIndex].minutes = (alarms[alarmIndex].minutes + 1) % 60;
            else if (currentState == 3)
                alarms[alarmIndex].day = (alarms[alarmIndex].day + 1) % 7;
        }
        else if (buttons.checkDown())
        {
            AlarmMenuUpdate = true;
            if (currentState == 0)
                alarms[alarmIndex].hours = (alarms[alarmIndex].hours + 23) % 24;
            else if (currentState == 1)
                alarms[alarmIndex].minutes = (alarms[alarmIndex].minutes + 59) % 60;
            else if (currentState == 3)
                alarms[alarmIndex].day = (alarms[alarmIndex].day + 6) % 7;
        }
    };

    if (AlarmMenuUpdate)
    {
        AlarmMenuUpdate = false;
        display.clearDisplay();
        display.setCursor(0, 0);

        display.setTextColor(WHITE, BLACK);
        display.setCursor(1, 10);
        display.println("Alarm " + String(alarmIndex));
        display.setFont(&DejaVu_LGC_Sans_Bold_8);
        display.setCursor(65, 10);
        display.print("Today:" + getShortCurrentWeekdayName());

        display.setFont(&DejaVu_LGC_Sans_Bold_10);
        centerText(":", 25, 34);
        drawMenuOption(formatWithLeadingZero(alarms[alarmIndex].hours), 35 - 18, 25, currentState == 0, isEditing && currentState == 0);
        drawMenuOption(formatWithLeadingZero(alarms[alarmIndex].minutes), 35 + 4, 25, currentState == 1, isEditing && currentState == 1);
        drawMenuOption("Day: " + getWeekdayName(alarms[alarmIndex].day + 1), 1, 50, currentState == 3, isEditing && currentState == 3);
        drawMenuOption("Enabled: " + String(alarms[alarmIndex].enabled ? "Yes" : "No"), 1, 37, currentState == 2, isEditing && currentState == 2);
        drawMenuOption("Sound: " + String(alarms[alarmIndex].soundOn ? "On" : "Off"), 1, 63, currentState == 4, isEditing && currentState == 4);
        drawBitmapOption(SCREEN_WIDTH - 30, 20, currentState == 5, isEditing && currentState == 5);

        manager.oledDisplay();
    }

    if (!isEditing)
    {
        if (buttons.checkConfirm())
        {
            AlarmMenuUpdate = true;
            if (currentState == 2)
                alarms[alarmIndex].enabled = !alarms[alarmIndex].enabled;
            else if (currentState == 4)
                alarms[alarmIndex].soundOn = !alarms[alarmIndex].soundOn;
            else if (currentState == 5)
                deleteAlarmStatic(alarmIndex);
            else
                isEditing = true;
        }
        else if (buttons.checkExit())
        {
            AlarmMenuUpdate = true;
            exitLoopFunction = true;
            AlarmMenuUpdate = true;
            editCurrentMenuEntry(String(alarmIndex) + " " + String(alarms[alarmIndex].enabled ? "On" : "Off") + " " + getShortWeekdayName(alarms[alarmIndex].day + 1) + " " + formatWithLeadingZero(alarms[alarmIndex].hours) + ":" + formatWithLeadingZero(alarms[alarmIndex].minutes));
        }
        else if (buttons.checkUp())
        {
            AlarmMenuUpdate = true;
            currentState = (currentState + 5) % 6;
        }
        else if (buttons.checkDown())
        {
            AlarmMenuUpdate = true;
            currentState = (currentState + 1) % 6;
        }
    }
    else
    {
        updateAlarmValue();

        if (buttons.checkConfirm() || buttons.checkExit())
        {
            AlarmMenuUpdate = true;
            isEditing = false;
        }
    }

    if (shouldExitLoop() == true)
    {
        AlarmMenuUpdate = true;
        exitLoopFunction = true;
        AlarmMenuUpdate = true;
        editCurrentMenuEntry(String(alarmIndex) + " " + String(alarms[alarmIndex].enabled ? "On" : "Off") + " " + getShortWeekdayName(alarms[alarmIndex].day + 1) + " " + formatWithLeadingZero(alarms[alarmIndex].hours) + ":" + formatWithLeadingZero(alarms[alarmIndex].minutes));
    }
}

void deleteAlarmStatic(int index)
{
    Serial.println("delete: " + String(index));
    alarms[index].exists = false;
    removeMenuEntry(data.currentButton);
    alarmsSubmenu->count = alarmsSubmenu->count - 1;
    exitLoopFunction = true;
}

void addNewAlarm()
{
    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        if (!alarms[i].exists)
        {
            alarms[i] = {true, false, 0, 0, 0, true}; // probably should do it instead of letting random data in there
            alarms[i].exists = true;
            Serial.println("New alarm added. " + String(i));
            data.currentSubmenu = alarmsSubmenu->entries;
            data.submenuCount = alarmsSubmenu->count;

            addEntryToSubmenu(alarmsSubmenu, String(i) + " " + String(alarms[i].enabled ? "On" : "Off") + " " + getShortWeekdayName(alarms[i].day + 1) + " " + formatWithLeadingZero(alarms[i].hours) + ":" + formatWithLeadingZero(alarms[i].minutes), nullptr, manageAlarms, nullptr, nullptr);

            // alarmsSubmenu->name = menuName;
            data.submenuCount++;
            data.currentButton = data.submenuCount - 1;
            delay(1);
            showMenu();
            return;
        }
    }
    Serial.println("Failed to add alarm: Maximum number of alarms reached.");
}

void initAlarmMenus()
{
    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        Serial.println("Alarm exists: " + String(alarms[i].exists));

        if (alarms[i].exists == true)
        {
            Serial.println("New alarm added. " + String(i));

            addEntryToSubmenu(alarmsSubmenu, String(i) + " " + String(alarms[i].enabled ? "On" : "Off") + " " + getShortWeekdayName(alarms[i].day + 1) + " " + formatWithLeadingZero(alarms[i].hours) + ":" + formatWithLeadingZero(alarms[i].minutes), nullptr, manageAlarms, nullptr, nullptr);

            // alarmsSubmenu->name = menuName;
            delay(1);
            showMenu();
        }
    }
}

void startOTA()
{
    saveOtaValue(true);
    ESP.restart();
}

void handleMenus()
{
    loopMenu();
}

void menuTask(void *parameter)
{
    startedLoop = true;
    while (true)
    {
        if (goToSleep == false && (manager.ScreenEnabled == true || inputDetected == true))
        {
            handleMenus();
        }
        vTaskDelay(10); // Adjust delay as needed
    }
}

void initMenus()
{
    entryMenu *WeatherItems = new entryMenu[4]{
        {"Current Weather", initWeatherMenu,
         currentWeatherMenu, nullptr, nullptr},
        {"Today's cast", initWeatherMenu, []()
         { displayWeatherCast(0); }, nullptr, nullptr},
        {"Tomorrow's cast", initWeatherMenu, []()
         { displayWeatherCast(1); }, nullptr, nullptr},
        {"Day After's Cast", initWeatherMenu, []()
         { displayWeatherCast(2); }, nullptr, nullptr}};

    Submenu *weatherSubmenu = new Submenu{"Weather", WeatherItems, 4, 4};

    entryMenu weatherButton = {"Weather", nullptr, nullptr, weatherSubmenu, nullptr};

    entryMenu *chartItems = new entryMenu[3]{
        {"Temp Chart", initTempGraph, loopTempGraph, nullptr, nullptr},
        {"Humidity Chart", initHumidityGraph, loopHumidityGraph, nullptr, nullptr},
        {"Light Chart", initLightGraph, loopLightGraph, nullptr, nullptr},
    };

    Submenu *chartSubmenu = new Submenu{"Charts", chartItems, 3, 3};

    entryMenu chartButton = {"Charts", nullptr, nullptr, chartSubmenu, nullptr};

    entryMenu *debugItems = new entryMenu[6]{
        {"General Debug", nullptr, generalDebugMenu, nullptr, nullptr},
        {"Cpu Debug", nullptr, CPUDebugMenu, nullptr, nullptr},
        {"WiFi Debug", nullptr, wifiDebugMenu, nullptr, nullptr},
        {"Touch Debug", nullptr, touchDebugMenu, nullptr, nullptr},
        {"FPS calc", nullptr, fpsCalc, nullptr, nullptr},
        {"Upload OTA", startOTA, nullptr, nullptr, nullptr},
    };

    Submenu *debugSubmenu = new Submenu{"Debug", debugItems, 6, 6};
    entryMenu debugButton = {"Debug", nullptr, nullptr, debugSubmenu, nullptr};

    entryMenu *manageAlarmsItems = new entryMenu[1]{
        {"Save Alarms", saveAlarms, nullptr, nullptr, nullptr},
    };
    Submenu *manageAlarmsSubmenu = new Submenu{"Alarm Ctrl", manageAlarmsItems, 1, 1};

    // Initialize alarm menu
    alarmsSubmenu = createAlarmsMenu();
    entryMenu *alarmsItems = new entryMenu[2]{
        {"Alarms", nullptr, nullptr, alarmsSubmenu, nullptr},
        {"Alarm Ctrl", nullptr, nullptr, manageAlarmsSubmenu, nullptr},
    };
    Submenu *alarmsSubmenu = new Submenu{"Alarms", alarmsItems, 2, 2};                                                   
    entryMenu alarmsButton = {"Alarms", nullptr, nullptr, alarmsSubmenu, nullptr}; 

    // Initialize the main menu
    initMenu(new entryMenu[4]{
                 alarmsButton,
                 weatherButton,
                 chartButton,
                 debugButton,
             },
             4, "Main Menu", 1, 1);
    initAlarmMenus();
    xTaskCreatePinnedToCore(
        menuTask,    // Task function
        "Menu Task", // Task name
        2048,        // Stack size in bytes
        NULL,        // Task parameter
        3,           // Task priority
        NULL,        // Task handle
        0            // Core number (0 or 1 for ESP32)
    );
}
