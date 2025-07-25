#ifndef INTERFACE_H
#define INTERFACE_H

#include "../defines.h"

void initMenus();
void showMenu();

void addNewAlarm();

bool shouldExitLoop();
void resetToDefaultMenu();

extern bool menuRunning;
extern bool idleEnabled;
extern bool exitLoopFunction;
extern unsigned long lastInputTime;

extern TaskHandle_t menuTaskHandle;

struct Submenu; 

struct entryMenu {
    String text;
    void (*function)();
    void (*loopFunction)();
    Submenu *submenu;
    const GFXfont *font;
};

struct Submenu {
    String name;
    entryMenu *entries;
    int count;
    int maxMenus;
    int lastSelectedIndex = 0;
};

struct menuData {
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

extern menuData data;

bool addEntryToSubmenu(Submenu *submenu, const String &text, void (*function)(), void (*loopFunction)(), const GFXfont *font);
Submenu *createSubmenu(const String &name, int maxMenus, void (*function)() = nullptr);
void editCurrentMenuEntry(String newText, void (*newFunction)() = nullptr, void (*newLoopFunction)() = nullptr);
void addMenuEntry(entryMenu entry);
void removeMenuEntry(int index);

#endif
