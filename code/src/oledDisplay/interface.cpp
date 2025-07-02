#include "interface.h"

#define BUTTONS_OFFSET 1
#define BUTTON_HEIGHT 10

TaskHandle_t menuTaskHandle;

struct entryMenu
{
    String text;
    void (*function)();
    void (*loopFunction)();
    struct Submenu *submenu;
    const GFXfont *font;
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
            resetToDefaultMenu();
            lastAnimationTime = millis();
        }

        if (currentState == ANIMATING)
        {
            if (millis() - lastAnimationTime >= animationInterval)
            {
                showMainPage();
                lastAnimationTime = millis();
            }

            if (useAllButtons() != None)
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

    // If no button is pressed, check for idle timeout
    if (!(useAllButtons() != None))
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

    switch (useButton())
    {
    case Up:
        data.currentButton = max(data.currentButton - 1, 0);
        showMenu();
        lastInputTime = millis(); // Reset last input time
        timerActive = false;      // Stop the timer
        break;
    case Down:
        data.currentButton = min(data.currentButton + 1, (data.isSubmenu ? data.submenuCount - 1 : data.totalMenus - 1));
        showMenu();
        lastInputTime = millis(); // Reset last input time
        timerActive = false;      // Stop the timer
        break;
    case Menu:
        handleConfirm();
        lastInputTime = millis(); // Reset last input time
        timerActive = false;      // Stop the timer
        break;
    case Back:
        timerActive = false;      // Stop the timer
        lastInputTime = millis(); // Reset last input time
        exitSubmenu();
        break;
    default:
        break;
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

void resetToDefaultMenu()
{
    stackPointer = -1;
    data.isSubmenu = false;
    data.currentSubmenu = nullptr;
    data.currentButton = 0;
    currentMenuItem = 0;
    pageNumber = 0;
    currentPage = 0;
    data.menuName = "Main Menu";
    showMenu();
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
        addEntryToSubmenu(alarmsSubmenu, "Add New Alarm", addNewAlarm, nullptr, &DejaVu_LGC_Sans_Bold_10);
    }
    return alarmsSubmenu;
}

Submenu *alarmsSubmenu = createAlarmsMenu();

bool AlarmMenuUpdate = true;

String getAlarmEntryName(int index)
{
    String days;
    for (int i = 0; i < 7; i++)
    {
        if (alarms[index].days[i])
        {
            if (!days.isEmpty())
                days += ".";
            days += getShortestWeekdayName(i + 1);
        }
    }

    String name = String(index) + " " + String(alarms[index].enabled ? "On" : "Off") + " " + formatWithLeadingZero(alarms[index].hours) + ":" + formatWithLeadingZero(alarms[index].minutes) + " " + days;
    return name;
}

uint8_t alarmCurrentState = 0;
bool isEditingAlarm = false;
bool inDaySelectionMode = false;

void initManageAlarm() {
    alarmCurrentState = 0;
    isEditingAlarm = false;
    inDaySelectionMode = false;
    inkButtonStates btn = useButton();
}

void manageAlarms()
{
    uint8_t alarmIndex = alarmsSubmenu->entries[data.currentButton].text.toInt();

    static unsigned long lastRepeatTime = 0;
    const unsigned long repeatInterval = 150;
    inkButtonStates btn = useButton();
    int labelWidth = 0;

    auto drawMenuOption = [&](const String &label, int16_t x, int16_t y, bool selected, bool editing)
    {
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(label, x, y, &x1, &y1, &w, &h);

        if (editing)
            display.drawRect(x1 - 2, y1 - 2, w + 4, h + 4, WHITE);
        else if (selected)
        {
            display.fillRect(x1 - 2, y1 - 2, w + 4, h + 4, WHITE);
            display.setTextColor(BLACK, WHITE);
        }
        else
            display.setTextColor(WHITE, BLACK);

        display.setCursor(x, y);
        display.print(label);
    };

    auto drawBitmapOption = [&](int16_t x, int16_t y, bool selected, bool editing)
    {
        if (editing || selected)
        {
            display.drawRect(x - 2, y - 2, 22, 22, WHITE);
            display.drawBitmap(x, y, remove_18x18, 18, 18, WHITE);
        }
        else
            display.drawBitmap(x, y, remove_18x18, 18, 18, BLACK, WHITE);
    };

    auto drawDaySelection = [&](int16_t x, int16_t y, bool selected, int dayIndex, bool groupSelected, bool buttonSelected)
    {
        String dayLabel = getShorterWeekdayName(dayIndex + 1);
        int16_t x1, y1;
        uint16_t w, h;
        display.getTextBounds(dayLabel, 0, 0, &x1, &y1, &w, &h);
        labelWidth = w;

        display.setTextColor(buttonSelected ? BLACK : WHITE, buttonSelected ? WHITE : BLACK);
        display.setCursor(x, y);
        display.print(dayLabel);

        if (selected && !buttonSelected)
            display.drawRect(x - 1, y - 10, w + 4, 12, WHITE);
        if (buttonSelected)
        {
            display.fillRect(x - 1, y - 10, w + 4, 12, WHITE);
            if (selected)
                display.drawRect(x - 3, y - 12, w + 8, 16, WHITE);
            display.setCursor(x, y);
            display.print(dayLabel);
        }
    };

    auto drawDaySelectionGroup = [&](int16_t x, int16_t y)
    {
        int startX = x;
        labelWidth = 0;
        for (int i = 0; i < 7; i++)
        {
            drawDaySelection(startX, y, alarms[alarmIndex].days[i], i, true, i == alarmCurrentState);
            startX += labelWidth + 5;
        }
    };

    auto redrawDisplay = [&]()
    {
        display.clearDisplay();
        display.setTextColor(WHITE, BLACK);
        display.setCursor(1, 8);
        display.println("Alarm " + String(alarmIndex));
        display.setFont(&font4pt7b);
        display.setCursor(70, 7);
        display.print("Today:" + getShortCurrentWeekdayName());

        display.setFont(&DejaVu_LGC_Sans_Bold_10);
        centerText(":", 19, 34);
        drawMenuOption(formatWithLeadingZero(alarms[alarmIndex].hours), 17, 19, !inDaySelectionMode && alarmCurrentState == 0, isEditingAlarm && alarmCurrentState == 0);
        drawMenuOption(formatWithLeadingZero(alarms[alarmIndex].minutes), 39, 19, !inDaySelectionMode && alarmCurrentState == 1, isEditingAlarm && alarmCurrentState == 1);

        display.setFont(&DejaVu_LGC_Sans_Bold_9);
        if (inDaySelectionMode)
            drawDaySelectionGroup(2, 46);
        else
        {
            int startX = 2;
            labelWidth = 0;
            for (int i = 0; i < 7; i++)
            {
                drawDaySelection(startX, 46, alarms[alarmIndex].days[i], i, false, false);
                startX += labelWidth + 5;
            }
            if (alarmCurrentState == 3)
                display.drawRect(0, 34, SCREEN_WIDTH-1, 16, WHITE);
        }

        display.setTextColor(WHITE, BLACK);
        display.setFont(&DejaVu_LGC_Sans_Bold_10);
        drawMenuOption("Enabled: " + String(alarms[alarmIndex].enabled ? "Yes" : "No"), 1, 32, !inDaySelectionMode && alarmCurrentState == 2, isEditingAlarm && alarmCurrentState == 2);
        drawMenuOption("Sound:" + String(alarms[alarmIndex].soundOn ? "On" : "Off"), 1, 59, !inDaySelectionMode && alarmCurrentState == 4, isEditingAlarm && alarmCurrentState == 4);
        drawMenuOption("Light:" + String(alarms[alarmIndex].lightOn ? "On" : "Off"), 84 - 15, 59, !inDaySelectionMode && alarmCurrentState == 5, isEditingAlarm && alarmCurrentState == 5);
        drawBitmapOption(SCREEN_WIDTH - 30, 14, !inDaySelectionMode && alarmCurrentState == 6, !inDaySelectionMode && isEditingAlarm && alarmCurrentState == 6);

        display.display();
    };

    auto updateAlarmValueUp = [&]()
    {
        if (alarmCurrentState == 0)
            alarms[alarmIndex].hours = (alarms[alarmIndex].hours + 1) % 24;
        else if (alarmCurrentState == 1)
            alarms[alarmIndex].minutes = (alarms[alarmIndex].minutes + 1) % 60;
        AlarmMenuUpdate = true;
    };

    auto updateAlarmValueDown = [&]()
    {
        if (alarmCurrentState == 0)
            alarms[alarmIndex].hours = (alarms[alarmIndex].hours + 23) % 24;
        else if (alarmCurrentState == 1)
            alarms[alarmIndex].minutes = (alarms[alarmIndex].minutes + 59) % 60;
        AlarmMenuUpdate = true;
    };

    unsigned long now = millis();

    switch (btn)
    {
    case Up:
        if (inDaySelectionMode)
            alarmCurrentState = (alarmCurrentState + 1) % 7;
        else if (!isEditingAlarm)
            alarmCurrentState = (alarmCurrentState + 6) % 7;
        else
            updateAlarmValueUp();
        AlarmMenuUpdate = true;
        lastRepeatTime = now;
        break;

    case Down:
        if (inDaySelectionMode)
            alarmCurrentState = (alarmCurrentState + 6) % 7;
        else if (!isEditingAlarm)
            alarmCurrentState = (alarmCurrentState + 1) % 7;
        else
            updateAlarmValueDown();
        AlarmMenuUpdate = true;
        lastRepeatTime = now;
        break;
    case LongDown:
        if (isEditingAlarm)
        {
            while (digitalRead(DOWN_PIN) == LOW)
            {
                if ((millis() - lastRepeatTime > 200))
                {
                    updateAlarmValueDown();
                    if (AlarmMenuUpdate)
                    {
                        AlarmMenuUpdate = false;
                        redrawDisplay();
                    }
                    lastRepeatTime = millis();
                }
            }
        }
        break;
    case LongUp:
        if (isEditingAlarm)
        {
            while (digitalRead(UP_PIN) == LOW)
            {
                if ((millis() - lastRepeatTime > 200))
                {
                    updateAlarmValueUp();
                    if (AlarmMenuUpdate)
                    {
                        AlarmMenuUpdate = false;
                        redrawDisplay();
                    }
                    lastRepeatTime = millis();
                }
            }
        }
        break;
    case Menu:
        if (inDaySelectionMode)
            alarms[alarmIndex].days[alarmCurrentState] = !alarms[alarmIndex].days[alarmCurrentState];
        else if (!isEditingAlarm)
        {
            switch (alarmCurrentState)
            {
            case 2:
                alarms[alarmIndex].enabled = !alarms[alarmIndex].enabled;
                break;
            case 3:
                inDaySelectionMode = true;
                alarmCurrentState = 0;
                break;
            case 4:
                alarms[alarmIndex].soundOn = !alarms[alarmIndex].soundOn;
                break;
            case 5:
                alarms[alarmIndex].lightOn = !alarms[alarmIndex].lightOn;
                break;
            case 6:
                deleteAlarmStatic(alarmIndex);
                break;
            default:
                isEditingAlarm = true;
                break;
            }
        }
        else
            isEditingAlarm = false;
        AlarmMenuUpdate = true;
        break;

    case Back:
        if (inDaySelectionMode)
        {
            inDaySelectionMode = false;
            alarmCurrentState = 0;
        }
        else if (isEditingAlarm)
            isEditingAlarm = false;
        else
        {
            exitLoopFunction = true;
            editCurrentMenuEntry(getAlarmEntryName(alarmIndex));
        }
        AlarmMenuUpdate = true;
        break;

    default:
        break;
    }

    if (AlarmMenuUpdate)
    {
        AlarmMenuUpdate = false;
        redrawDisplay();
    }

    if (shouldExitLoop() && data.currentButton != 0)
    {
        AlarmMenuUpdate = true;
        exitLoopFunction = true;
        editCurrentMenuEntry(getAlarmEntryName(alarmIndex));
    }
}

void deleteAlarmStatic(int index)
{
    Serial.println("delete: " + String(index));
    alarms[index].exists = false;
    removeMenuEntry(data.currentButton);
    alarmsSubmenu->count = alarmsSubmenu->count - 1;
    data.currentButton = 0;
    exitLoopFunction = true;
}

void addNewAlarm()
{
    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        if (!alarms[i].exists)
        {
            alarms[i] = { // lets not have random data there
                true,                                      // exists
                false,                                      // enabled
                {true, true, true, true, true, true, true}, // days
                0,                                          // hours
                0,                                          // minutes
                true,                                       // soundOn
                true                                        // lightOn
            };
            Serial.println("New alarm added. " + String(i));
            data.currentSubmenu = alarmsSubmenu->entries;
            data.submenuCount = alarmsSubmenu->count;

            addEntryToSubmenu(alarmsSubmenu, getAlarmEntryName(i), initManageAlarm, manageAlarms, &font4pt7b);

            // alarmsSubmenu->name = menuName;
            data.submenuCount++;
            data.currentButton = data.submenuCount - 1;
            delay(1);
            showMenu();
            return;
        }
    }
    Serial.println("Failed to add alarm: Maximum number of alarms reached.");
    int rectWidth = 120;
    int rectHeight = 40;
    int rectX = (SCREEN_WIDTH - rectWidth) / 2;
    int rectY = (SCREEN_HEIGHT - rectHeight) / 2;

    display.fillRect(rectX, rectY, rectWidth, rectHeight, BLACK);
    display.drawRect(rectX - 1, rectY - 1, rectWidth + 2, rectHeight + 2, WHITE);

    display.setTextColor(WHITE);
    display.setCursor(rectX + 10, rectY + 10);
    display.print("Max alarm");
    display.setCursor(rectX + 10, rectY + 20);
    display.print("number reached");
    display.display();
    delay(5000);
}

void initAlarmMenus()
{
    for (int i = 0; i < MAX_ALARMS; ++i)
    {
        Serial.println("Alarm exists: " + String(alarms[i].exists));

        if (alarms[i].exists == true)
        {
            Serial.println("New alarm added. " + String(i));

            addEntryToSubmenu(alarmsSubmenu, getAlarmEntryName(i), initManageAlarm, manageAlarms, &font4pt7b);

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
    while (true)
    {
        handleMenus();
        vTaskDelay(10);
    }
}

void refreshAlarmsSubmenu()
{
    if (alarmsSubmenu)
    {
        memset(alarmsSubmenu->entries, 0, sizeof(entryMenu) * alarmsSubmenu->maxMenus);
        alarmsSubmenu->count = 0;

        addEntryToSubmenu(alarmsSubmenu, "Add New Alarm", addNewAlarm, nullptr, &DejaVu_LGC_Sans_Bold_10);

        for (int i = 0; i < MAX_ALARMS; ++i)
        {
            if (alarms[i].exists)
            {
                addEntryToSubmenu(alarmsSubmenu, getAlarmEntryName(i), initManageAlarm, manageAlarms, &font4pt7b);
            }
        }

        if (data.isSubmenu && data.currentSubmenu == alarmsSubmenu->entries)
        {
            data.submenuCount = alarmsSubmenu->count;
            showMenu();
        }
    }
}

void disableAlarmsIn()
{
    disableAllAlarms();
    refreshAlarmsSubmenu();
}

void enableAlarmsIn()
{
    enableAllAlarms();
    refreshAlarmsSubmenu();
}

void readAlarmsIn()
{
    readAlarms();
    refreshAlarmsSubmenu();
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
        {"Alarms Flash Read", refreshAlarmsSubmenu, nullptr, nullptr, nullptr},
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
