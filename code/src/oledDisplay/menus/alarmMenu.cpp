#include "alarmMenu.h"

bool AlarmMenuUpdate = true;
uint8_t alarmCurrentState = 0;
bool isEditingAlarm = false;
bool inDaySelectionMode = false;

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
            alarms[i] = {
                // lets not have random data there
                true,                                       // exists
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


void initManageAlarm()
{
    alarmCurrentState = 0;
    isEditingAlarm = false;
    AlarmMenuUpdate = true;
    inDaySelectionMode = false;
    inkButtonStates btn = useButton();
}

void manageAlarms()
{
    uint8_t alarmIndex = alarmsSubmenu->entries[data.currentButton].text.toInt();
    bool exitAlarm = false;

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
                display.drawRect(0, 34, SCREEN_WIDTH - 1, 16, WHITE);
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
            AlarmMenuUpdate = true;
        }
        else if (isEditingAlarm)
        {
            isEditingAlarm = false;
            AlarmMenuUpdate = true;
        }
        else
        {
            exitAlarm = true;
        }
        break;

    default:
        break;
    }

    if (AlarmMenuUpdate)
    {
        AlarmMenuUpdate = false;
        redrawDisplay();
    }

    if (useAllButtons() != None)
    {
        lastInputTime = millis();
    }
    if (millis() - lastInputTime > LOOP_FUNCTION_TIMEOUT_MS)
    {
        exitAlarm = true;
    }

    if (exitAlarm == true && data.currentButton != 0)
    {
        AlarmMenuUpdate = false;
        exitLoopFunction = true;
        editCurrentMenuEntry(getAlarmEntryName(alarmIndex));
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