#ifndef __PHANTOM_MENU_H__
#define __PHANTOM_MENU_H__

#include <Arduino.h>
#include <vector>
#include <functional>
#include <globals.h>

struct PhantomMenuItem {
    const char *label;
    uint16_t color;
    std::function<void()> action;
};

class PhantomMenu {
public:
    PhantomMenu();
    ~PhantomMenu();

    void begin();
    static void bootSequence();

    // Submenu system
    static void submenu(const char *title, std::vector<PhantomMenuItem> &items);

private:
    void drawStatusBar();
    void drawMenuList();
    void drawMenuItem(int idx, bool selected, int y);

    // Idle
    void idleLoop();
    void idleMatrixRain();
    void idleFoxMask();
    void idleCircuit();
    void idleDataScroll();

    std::vector<PhantomMenuItem> _items;
    int _sel;
    int _scroll;
    unsigned long _lastActivity;
    bool _idleEnabled;
};

#endif
