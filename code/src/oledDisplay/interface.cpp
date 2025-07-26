#include "interface.h"

#define BUTTONS_OFFSET 1
#define BUTTON_HEIGHT 10

TaskHandle_t menuTaskHandle;

menuData data = {};
int mainMenuLastSelectedIndex = 0;

Submenu *menuStack[MAX_STACK_SIZE];
int stackPointer = -1;

unsigned long lastInputTime = 0;

bool menuRunning = true;
bool previousMenuState = true;

bool exitLoopFunction = false;

void showMenu()
{
    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    display.clearDisplay();
    display.setTextSize(data.textSize);
    int currentPage = 0;
    int maxItems = 4;
    int minItems = 1;
    int pageNumber = 0;
    int availableHeight = SCREEN_HEIGHT - 20;
    int usedHeight = 0;
    int tmpItemsOnPage = maxItems;

    for (int i = 0; i < maxItems && i < (data.isSubmenu ? data.submenuCount : data.totalMenus); i++)
    {
        auto &entry = data.isSubmenu ? data.currentSubmenu[i] : data.entryList[i];

        if (entry.font)
            display.setFont(entry.font); // Set font for this entry

        String text = data.isSubmenu ? data.currentSubmenu[i].text : data.entryList[i].text;

        int16_t x1, y1;
        uint16_t textWidth, textHeight;
        display.getTextBounds(text, 0, 0, &x1, &y1, &textWidth, &textHeight);

        int lineHeight = (textWidth > SCREEN_WIDTH) ? (textHeight * 2) : textHeight;

        if (textHeight < 10)
        {
            lineHeight = textHeight + 2;
        }
        else
        {
            lineHeight = textHeight;
        }

        if (usedHeight + lineHeight > availableHeight)
        {
            tmpItemsOnPage = max(minItems, i);
            break;
        }

        usedHeight += lineHeight + BUTTONS_OFFSET;
        display.setFont(&DejaVu_LGC_Sans_Bold_10);
    }

    data.itemsOnPage = tmpItemsOnPage;

    int startingButton = data.currentButton - (data.currentButton % data.itemsOnPage);
    currentPage = data.currentButton / data.itemsOnPage;
    pageNumber = ((data.isSubmenu ? data.submenuCount : data.totalMenus) + data.itemsOnPage - 1) / data.itemsOnPage;

    display.setCursor(0, 0);
    display.setTextColor(SSD1306_WHITE);

    display.setFont(&DejaVu_LGC_Sans_Bold_10);
    display.setCursor(0, 10);
    display.print(String(currentPage + 1) + "/" + String(pageNumber));

    centerText(data.menuName, 10);

    int y = 20;
    for (int i = startingButton; i < startingButton + data.itemsOnPage && i < (data.isSubmenu ? data.submenuCount : data.totalMenus); i++)
    {
        auto &entry = data.isSubmenu ? data.currentSubmenu[i] : data.entryList[i];

        if (entry.font)
            display.setFont(entry.font); // Set font for this entry

        String displayText = data.isSubmenu ? data.currentSubmenu[i].text : data.entryList[i].text;
        int16_t x1, y1;
        uint16_t textWidth, textHeight;
        display.getTextBounds(displayText, 0, 0, &x1, &y1, &textWidth, &textHeight);

        // Adjust for smaller fonts to ensure proper line height
        if (textHeight < 10)
        {
            textHeight = 10; // Adjust textHeight to avoid too small font rendering
        }

        float floatLines = (float)textHeight / (float)BUTTON_HEIGHT;
        int lines = round(floatLines);
        if (lines == 0)
        {
            lines = 1; // Ensure at least one line
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

        int verticalOffset = (boxHeight - textHeight) / 2;

        display.setCursor(1, y + verticalOffset);
        display.print(displayText);

        // Increase Y based on text height and lines
        y += boxHeight + BUTTONS_OFFSET;
    }

    manager.sendOledAction(OLED_DISPLAY);
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

static bool timerActive = false;

enum IdleState
{
    IDLE,
    ANIMATING
};

IdleState currentState = IDLE;
unsigned long lastAnimationTime = 0;
bool idleEnabled = true;

void startIdleAnimation()
{
    if (idleEnabled)
    {
        if (currentState == IDLE && millis() - lastInputTime > MENU_TIMEOUT)
        {
            resetToDefaultMenu();
            currentState = ANIMATING;
            Serial.println("Main Page started...");
            lastAnimationTime = millis();
        }

        if (currentState == ANIMATING)
        {
            showMainPage();
            if (useAllButtons() != None)
            {
                resetToDefaultMenu();
                useButton();
                showMenu();
                lastInputTime = millis();
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
        if (stackPointer >= 0)
        {
            Submenu *prevSubmenu = menuStack[stackPointer];
            data.currentSubmenu = prevSubmenu->entries;
            data.submenuCount = prevSubmenu->count;
            data.menuName = prevSubmenu->name;
            data.isSubmenu = true;
            data.currentButton = prevSubmenu->lastSelectedIndex;
        }
        else
        {
            data.currentSubmenu = nullptr;
            data.menuName = "Main Menu";
            data.isSubmenu = false;
            data.currentButton = mainMenuLastSelectedIndex;
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
        menuRunning = false;
        previousMenuState = false;
        startIdleAnimation();
    }
}

void pushSubmenu(Submenu *submenu)
{
    if (stackPointer >= 0)
        menuStack[stackPointer]->lastSelectedIndex = data.currentButton;
    else
        mainMenuLastSelectedIndex = data.currentButton;

    if (stackPointer < 9)
    {
        stackPointer++;
        menuStack[stackPointer] = submenu;
    }
}

bool shouldExitLoop()
{
    if (useAllButtons() != None)
    {
        lastInputTime = millis();
    }

    if (millis() - lastInputTime > LOOP_FUNCTION_TIMEOUT_MS || exitLoopFunction == true || useButtonBack() == Back || useButtonBack() == LongBack)
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
        delay(5);
        loopFunction();
        if (exitLoopFunction == true)
        {
            break;
        }
    }
    showMenu();
}

void handleConfirm()
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
    else if (data.entryList[data.currentButton].loopFunction != nullptr)
    {
        runLoopFunction(data.entryList[data.currentButton].loopFunction);
    }
}

void loopMenu()
{
    if (useAllButtons() == None)
    {
        if (!timerActive)
        {
            useTouch();
            lastInputTime = millis();
            timerActive = true;
        }
        else
        {
            if (millis() - lastInputTime >= MENU_TIMEOUT)
            {
                if (previousMenuState == true)
                {
                    resetToDefaultMenu();
                }
                startIdleAnimation();
                menuRunning = false;
                previousMenuState = false;
            }
        }
    }
    else
    {
        lastInputTime = millis();
        timerActive = false;
        menuRunning = true;
        showMenu();
        if (menuRunning == true && previousMenuState == false)
        {
            useButton();
            previousMenuState = true;
        }
    }

    if (menuRunning == true)
    {
        useTouch();
        switch (useButton())
        {
        case Up:
            data.currentButton = max(data.currentButton - 1, 0);
            showMenu();
            lastInputTime = millis();
            timerActive = false;
            break;
        case Down:
            data.currentButton = min(data.currentButton + 1, (data.isSubmenu ? data.submenuCount - 1 : data.totalMenus - 1));
            showMenu();
            lastInputTime = millis();
            timerActive = false;
            break;
        case Menu:
            handleConfirm();
            lastInputTime = millis();
            timerActive = false;
            break;
        case Back:
            timerActive = false;
            lastInputTime = millis();
            exitSubmenu();
            break;
        default:
            break;
        }
    }
}

void resetToDefaultMenu()
{
    stackPointer = -1;
    data.isSubmenu = false;
    data.currentSubmenu = nullptr;
    data.currentButton = 0;
    data.menuName = "Main Menu";
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

void editCurrentMenuEntry(String newText, void (*newFunction)(), void (*newLoopFunction)())
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

Submenu *createSubmenu(const String &name, int maxMenus, void (*function)())
{
    Submenu *submenu = new Submenu;
    submenu->name = name;
    submenu->entries = new entryMenu[maxMenus];
    submenu->count = 0;
    submenu->maxMenus = maxMenus;
    return submenu;
}

bool addEntryToSubmenu(Submenu *submenu, const String &text, void (*function)(), void (*loopFunction)(), const GFXfont *font)
{
    if (submenu->count >= MAX_MENU_ITEMS)
        return false;

    submenu->entries[submenu->count].text = text;
    submenu->entries[submenu->count].function = function;
    submenu->entries[submenu->count].loopFunction = loopFunction;
    submenu->entries[submenu->count].submenu = nullptr; // Assuming this is what you want
    submenu->entries[submenu->count].font = font;

    submenu->count++;
    return true;
}

void menuTask(void *parameter)
{
    while (true)
    {
        loopMenu();
        vTaskDelay(10);
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

    entryMenu weatherButton = {"Weather", nullptr, nullptr, weatherSubmenu, &DejaVu_LGC_Sans_Bold_10};

    entryMenu *chartItems = new entryMenu[3]{
        {"Temp Chart", initTempGraph, loopTempGraph, nullptr, nullptr},
        {"Humidity Chart", initHumidityGraph, loopHumidityGraph, nullptr, nullptr},
        {"Light Chart", initLightGraph, loopLightGraph, nullptr, nullptr},
    };

    Submenu *chartSubmenu = new Submenu{"Charts", chartItems, 3, 3};

    entryMenu chartButton = {"Charts", nullptr, nullptr, chartSubmenu, &DejaVu_LGC_Sans_Bold_10};

    entryMenu *debugItems = new entryMenu[6]{
        {"General Debug", nullptr, generalDebugMenu, nullptr, nullptr},
        {"Cpu Debug", nullptr, CPUDebugMenu, nullptr, nullptr},
        {"WiFi Debug", nullptr, wifiDebugMenu, nullptr, nullptr},
        {"Touch Debug", nullptr, touchDebugMenu, nullptr, nullptr},
        {"FPS calc", nullptr, fpsCalc, nullptr, nullptr},
        {"Upload OTA", startOTA, nullptr, nullptr, nullptr},
    };

    Submenu *debugSubmenu = new Submenu{"Debug", debugItems, 6, 6};
    entryMenu debugButton = {"Debug", nullptr, nullptr, debugSubmenu, &DejaVu_LGC_Sans_Bold_10};

    entryMenu *manageAlarmsItems = new entryMenu[5]{
        {"Save Alarms", saveAlarms, nullptr, nullptr, nullptr},
        {"Disable All Alarms", disableAlarmsIn, nullptr, nullptr, nullptr},
        {"Enable All Alarms", enableAlarmsIn, nullptr, nullptr, nullptr},
        {"Refresh Alarms", refreshAlarmsSubmenu, nullptr, nullptr, nullptr},
        {"Alarms Flash Read", readAlarmsIn, nullptr, nullptr, nullptr},
    };
    Submenu *manageAlarmsSubmenu = new Submenu{"Alarm Ctrl", manageAlarmsItems, 5, 5};

    // Initialize alarm menu
    alarmsSubmenu = createAlarmsMenu();
    entryMenu *alarmsItems = new entryMenu[2]{
        {"Alarms", nullptr, nullptr, alarmsSubmenu, nullptr},
        {"Alarm Ctrl", nullptr, nullptr, manageAlarmsSubmenu, nullptr},
    };
    Submenu *alarmsMain = new Submenu{"Alarms", alarmsItems, 2, 2};
    entryMenu alarmsButton = {"Alarms", nullptr, nullptr, alarmsMain, &DejaVu_LGC_Sans_Bold_10};

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
        menuTask,        // Task function
        "Menu Task",     // Task name
        4096,            // Stack size in bytes
        NULL,            // Task parameter
        3,               // Task priority
        &menuTaskHandle, // Task handle
        0                // Core number (0 or 1 for ESP32)
    );
}
