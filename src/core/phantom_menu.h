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
    void idleScreen();

private:
    void drawMenu();
    void drawGrid(int selectedIdx);

    std::vector<PhantomMenuItem> _items;
    int _selectedIndex;
    int _scrollOffset;
    unsigned long _lastActivity;
    static const int IDLE_TIMEOUT_MS = 30000;
};

#endif
