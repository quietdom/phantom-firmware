#ifndef __PHANTOM_MENU_H__
#define __PHANTOM_MENU_H__

#include <Arduino.h>
#include <vector>
#include <functional>
#include <globals.h>

struct PhantomMenuItem {
    const char *label;
    std::function<void()> action;
};

class PhantomMenu {
public:
    PhantomMenu();
    ~PhantomMenu();

    void begin();
    static void bootSequence();
    static void submenu(const char *title, std::vector<PhantomMenuItem> &items);

private:
    void drawStatusBar();
    void drawMenuList();
    void drawMenuItem(int idx, bool selected, int y);
    void idleLoop();
    void clearAllInputs();

    std::vector<PhantomMenuItem> _items;
    int _sel = 0;
    int _scroll = 0;
    unsigned long _lastActivity = 0;
    bool _idleEnabled = true;
};

#endif
