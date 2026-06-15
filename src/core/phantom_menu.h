#ifndef __PHANTOM_MENU_H__
#define __PHANTOM_MENU_H__

#include <Arduino.h>
#include <vector>
#include <functional>
#include <globals.h>

struct PhantomMenuItem {
    const char *label;
    const char *icon;     // ASCII art icon name
    uint16_t color;       // Icon accent color
    std::function<void()> action;
};

class PhantomMenu {
public:
    PhantomMenu();
    ~PhantomMenu();

    // Main entry - runs the full menu loop with idle
    void begin();

    // Static idle screen - plays until user presses a button
    static void idleScreen();

    // Boot animation
    static void bootSequence();

private:
    // Menu rendering
    void drawMenu();
    void drawMenuItem(int index, bool selected);
    void drawStatusBar();
    void drawScanLines();
    void drawGlitchFrame();
    void drawGrid(int selectedIdx);

    // Animation helpers
    void animateTransition(int fromIdx, int toIdx);
    void glitchText(int x, int y, const char *text, uint16_t color, int iterations);
    void scanLineEffect(int y, int height, uint16_t color);
    void matrixRain(int duration);
    void foxMaskPulse(int cx, int cy, int scale);

    // Input
    bool waitForInput(int &selected);

    // Idle
    void idleMatrixRain();
    void idleFoxMask();
    void idleCircuitBoard();
    void idleDataStream();

    std::vector<PhantomMenuItem> _items;
    int _selectedIndex;
    int _scrollOffset;
    unsigned long _lastActivity;
    static const int IDLE_TIMEOUT_MS = 30000; // 30 seconds
};

#endif
