#include "phantom_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include "themes/watchdogs/theme.h"
#include <globals.h>

// === COLORS ===
static const uint16_t C_BG      = 0x0000;  // black
static const uint16_t C_GREEN   = 0x07E0;  // bright green
static const uint16_t C_DKGREEN = 0x0320;  // dark green
static const uint16_t C_MGREEN  = 0x0400;  // muted green
static const uint16_t C_WHITE   = 0xFFFF;
static const uint16_t C_GRAY    = 0x8410;  // medium gray
static const uint16_t C_DKGRAY  = 0x4208;  // dark gray
static const uint16_t C_DDKGRAY = 0x2104;  // very dark gray
static const uint16_t C_RED     = 0xF800;

// === LAYOUT (320x170 landscape) ===
static const int STATUS_H  = 18;   // status bar height
static const int ROW_H     = 22;   // each menu row height
static const int PAD_X     = 6;
static const int ICON_W    = 18;
static const int SCROLL_W  = 4;

extern int getBattery();

// ============================================================
//  BOOT SEQUENCE
// ============================================================
void PhantomMenu::bootSequence() {
    int w = tftWidth;
    int h = tftHeight;
    tft.fillScreen(C_BG);

    // Phase 1: horizontal scanlines sweep down
    for (int y = 0; y < h; y += 2) {
        tft.drawFastHLine(0, y, w, C_DKGREEN);
        if (y > 8) tft.drawFastHLine(0, y - 8, w, C_BG);
        delay(3);
    }
    tft.fillScreen(C_BG);

    // Phase 2: falling digits
    for (int frame = 0; frame < 20; frame++) {
        for (int col = 0; col < w; col += 10) {
            int y0 = random(-h, 0);
            int len = random(3, 8);
            for (int i = 0; i < len; i++) {
                int y = y0 + i * 10;
                if (y >= 0 && y < h) {
                    char buf[2] = { (char)('0' + random(0, 10)), 0 };
                    tft.setTextColor(i == 0 ? C_WHITE : C_GREEN, C_BG);
                    tft.drawString(buf, col, y, 1);
                }
            }
        }
        delay(30);
        tft.fillScreen(C_BG);
    }

    // Phase 3: fox mask draw with glitch
    int cx = w / 2;
    int cy = h / 2;
    int sc = min(w, h) / 6;

    for (int g = 0; g < 10; g++) {
        int ox = random(-5, 5), oy = random(-3, 3);
        uint16_t c = (g & 1) ? C_GREEN : C_WHITE;
        // eyes
        tft.fillTriangle(cx-sc*0.8+ox, cy-sc*0.3+oy, cx-sc*0.2+ox, cy-sc*0.6+oy, cx-sc*0.2+ox, cy-sc*0.1+oy, c);
        tft.fillTriangle(cx+sc*0.8+ox, cy-sc*0.3+oy, cx+sc*0.2+ox, cy-sc*0.6+oy, cx+sc*0.2+ox, cy-sc*0.1+oy, c);
        // ears
        tft.fillTriangle(cx-sc*0.6+ox, cy-sc*0.5+oy, cx-sc+ox, cy-sc+oy, cx-sc*0.2+ox, cy-sc*0.7+oy, c);
        tft.fillTriangle(cx+sc*0.6+ox, cy-sc*0.5+oy, cx+sc+ox, cy-sc+oy, cx+sc*0.2+ox, cy-sc*0.7+oy, c);
        delay(25);
        if (g < 9) {
            tft.fillTriangle(cx-sc*0.8+ox, cy-sc*0.3+oy, cx-sc*0.2+ox, cy-sc*0.6+oy, cx-sc*0.2+ox, cy-sc*0.1+oy, C_BG);
            tft.fillTriangle(cx+sc*0.8+ox, cy-sc*0.3+oy, cx+sc*0.2+ox, cy-sc*0.6+oy, cx+sc*0.2+ox, cy-sc*0.1+oy, C_BG);
            tft.fillTriangle(cx-sc*0.6+ox, cy-sc*0.5+oy, cx-sc+ox, cy-sc+oy, cx-sc*0.2+ox, cy-sc*0.7+oy, C_BG);
            tft.fillTriangle(cx+sc*0.6+ox, cy-sc*0.5+oy, cx+sc+ox, cy-sc+oy, cx+sc*0.2+ox, cy-sc*0.7+oy, C_BG);
        }
    }

    // Final fox mask
    tft.fillTriangle(cx-sc*0.8, cy-sc*0.3, cx-sc*0.2, cy-sc*0.6, cx-sc*0.2, cy-sc*0.1, C_WHITE);
    tft.fillTriangle(cx+sc*0.8, cy-sc*0.3, cx+sc*0.2, cy-sc*0.6, cx+sc*0.2, cy-sc*0.1, C_WHITE);
    tft.fillTriangle(cx, cy+sc*0.1, cx-sc*0.15, cy+sc*0.3, cx+sc*0.15, cy+sc*0.3, C_WHITE);
    tft.fillTriangle(cx-sc*0.6, cy-sc*0.5, cx-sc, cy-sc, cx-sc*0.2, cy-sc*0.7, C_WHITE);
    tft.fillTriangle(cx+sc*0.6, cy-sc*0.5, cx+sc, cy-sc, cx+sc*0.2, cy-sc*0.7, C_WHITE);
    tft.fillTriangle(cx-sc*0.55, cy-sc*0.55, cx-sc*0.85, cy-sc*0.9, cx-sc*0.3, cy-sc*0.65, C_BG);
    tft.fillTriangle(cx+sc*0.55, cy-sc*0.55, cx+sc*0.85, cy-sc*0.9, cx+sc*0.3, cy-sc*0.65, C_BG);
    tft.drawLine(cx, cy+sc*0.3, cx, cy+sc*0.5, C_WHITE);
    tft.drawLine(cx, cy+sc*0.5, cx-sc*0.3, cy+sc*0.6, C_WHITE);
    tft.drawLine(cx, cy+sc*0.5, cx+sc*0.3, cy+sc*0.6, C_WHITE);

    // circuit lines from mask
    for (int i = 0; i < 6; i++) {
        int sx = cx + (random(0,2) ? 1 : -1) * sc * (0.5 + random(0,40)/100.0);
        int sy = cy + random(-sc/2, sc/2);
        int ex = sx + (random(0,2) ? 1 : -1) * sc * 0.3;
        int ey = sy + random(-sc/3, sc/3);
        tft.drawLine(sx, sy, ex, ey, C_DKGREEN);
        tft.fillCircle(ex, ey, 2, C_GREEN);
    }
    delay(400);

    // Phase 4: text
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(C_GREEN, C_BG);
    tft.setTextSize(FP);
    const char *lines[] = { "DEDSEC NETWORK", "PHANTOM v1.0", "STATUS: ONLINE", ">>> ACCESS GRANTED <<<" };
    int ty = cy + sc + 20;
    for (int i = 0; i < 4; i++) {
        tft.drawString(lines[i], w / 2, ty, 1);
        ty += 12;
        delay(100);
    }
    delay(500);

    // Fade
    for (int b = 255; b >= 0; b -= 10) {
        tft.drawRect(cx - sc - 15, cy - sc - 15, sc * 2 + 30, sc * 2 + 90, tft.color565(b / 8, b / 4, b / 8));
        delay(8);
    }
    tft.fillScreen(C_BG);
}

// ============================================================
//  STATUS BAR
// ============================================================
void PhantomMenu::drawStatusBar() {
    int w = tftWidth;

    // Background
    tft.fillRect(0, 0, w, STATUS_H, C_BG);
    tft.drawFastHLine(0, STATUS_H - 1, w, C_DKGREEN);

    // Left: PHANTOM
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(C_GREEN, C_BG);
    tft.setTextSize(FP);
    tft.drawString("PHANTOM", PAD_X, 3, 1);

    // Right: battery + version
    int bat = getBattery();
    char batBuf[12];
    snprintf(batBuf, sizeof(batBuf), "%d%%", bat);

    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(bat > 20 ? C_GREEN : C_RED, C_BG);
    tft.drawString(batBuf, w - PAD_X, 3, 1);

    // Battery icon (simple bar)
    int bx = w - PAD_X - 28;
    int by = 4;
    int bw = 16;
    int bh = 8;
    tft.drawRect(bx, by, bw, bh, C_GRAY);
    tft.drawRect(bx + bw, by + 2, 2, 4, C_GRAY); // nub
    int fill = map(bat, 0, 100, 0, bw - 2);
    uint16_t batColor = bat > 50 ? C_GREEN : (bat > 20 ? 0xFFE0 : C_RED);
    tft.fillRect(bx + 1, by + 1, fill, bh - 2, batColor);
}

// ============================================================
//  MENU LIST
// ============================================================
void PhantomMenu::drawMenuList() {
    int w = tftWidth;
    int h = tftHeight;
    int listY = STATUS_H;
    int listH = h - STATUS_H;
    int visibleRows = listH / ROW_H;

    // Clamp scroll
    if (_sel < _scroll) _scroll = _sel;
    if (_sel >= _scroll + visibleRows) _scroll = _sel - visibleRows + 1;

    // Clear list area
    tft.fillRect(0, listY, w, listH, C_BG);

    // Draw items
    for (int i = 0; i < visibleRows && (_scroll + i) < (int)_items.size(); i++) {
        int idx = _scroll + i;
        int y = listY + i * ROW_H;
        bool sel = (idx == _sel);
        drawMenuItem(idx, sel, y);
    }

    // Scroll indicator
    if ((int)_items.size() > visibleRows) {
        int sbH = max(10, listH * visibleRows / (int)_items.size());
        int sbY = listY + (listH - sbH) * _sel / max(1, (int)_items.size() - 1);
        tft.fillRect(w - SCROLL_W - 1, listY, SCROLL_W, listH, C_DDKGRAY);
        tft.fillRect(w - SCROLL_W - 1, sbY, SCROLL_W, sbH, C_GREEN);
    }
}

void PhantomMenu::drawMenuItem(int idx, bool sel, int y) {
    int w = tftWidth;
    int itemW = w - PAD_X * 2 - SCROLL_W - 2;
    int x = PAD_X;

    if (sel) {
        tft.fillRoundRect(x, y + 1, itemW, ROW_H - 2, 3, C_DKGREEN);
    }

    // Icon area
    uint16_t ic = sel ? C_GREEN : C_GRAY;
    int ix = x + 3;
    int iy = y + ROW_H / 2;

    switch (idx) {
        case 0: // WiFi - signal arcs
            for (int a = 0; a < 3; a++)
                tft.drawArc(ix + 6, iy, 3 + a * 3, 2 + a * 3, 225, 315, ic, C_BG);
            break;
        case 1: // BLE - diamond
            tft.fillTriangle(ix + 6, iy - 5, ix + 1, iy, ix + 11, iy, ic);
            tft.fillTriangle(ix + 6, iy + 5, ix + 1, iy, ix + 11, iy, ic);
            break;
        case 2: // RF - wave
            tft.drawArc(ix + 6, iy, 6, 4, 310, 50, ic, C_BG);
            tft.drawArc(ix + 6, iy, 9, 7, 310, 50, ic, C_BG);
            break;
        case 3: // NRF24 - chip
            tft.drawRect(ix + 2, iy - 4, 8, 8, ic);
            tft.fillRect(ix + 4, iy - 1, 4, 2, ic);
            break;
        case 4: // IR - dot+beam
            tft.fillCircle(ix + 4, iy, 3, ic);
            tft.drawFastHLine(ix + 8, iy, 6, ic);
            break;
        case 5: // RFID - card
            tft.drawRect(ix + 1, iy - 4, 10, 8, ic);
            tft.drawFastHLine(ix + 3, iy, 6, ic);
            break;
        case 6: // Files - folder
            tft.drawRect(ix + 2, iy - 2, 8, 6, ic);
            tft.fillRect(ix + 2, iy - 4, 4, 2, ic);
            break;
        case 7: // Scripts - brackets
            tft.drawRect(ix + 1, iy - 4, 3, 8, ic);
            tft.drawRect(ix + 8, iy - 4, 3, 8, ic);
            break;
        case 8: // Clock
            tft.drawCircle(ix + 6, iy, 5, ic);
            tft.drawLine(ix + 6, iy, ix + 6, iy - 3, ic);
            tft.drawLine(ix + 6, iy, ix + 9, iy, ic);
            break;
        case 9: // Extras - star
            tft.drawFastHLine(ix + 1, iy, 10, ic);
            tft.drawFastVLine(ix + 6, iy - 4, 9, ic);
            break;
        case 10: // Config - gear
            tft.drawCircle(ix + 6, iy, 4, ic);
            tft.fillCircle(ix + 6, iy, 2, ic);
            break;
    }

    // Label
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(sel ? C_WHITE : C_GRAY, sel ? C_DKGREEN : C_BG);
    tft.setTextSize(FP);
    tft.drawString(_items[idx].label, x + ICON_W + 4, y + 6, 1);
}

// ============================================================
//  MAIN MENU LOOP
// ============================================================
void PhantomMenu::begin() {
    returnToMenu = false;
    _sel = 0;
    _scroll = 0;
    _lastActivity = millis();
    _idleEnabled = true; // default on

    // Build items
    _items.clear();
    _items.push_back({"WiFi",     C_GREEN,   nullptr});
    _items.push_back({"BLE",      0x07FF,    nullptr});
    _items.push_back({"RF",       0xFFE0,    nullptr});
    _items.push_back({"NRF24",    0xF81F,    nullptr});
    _items.push_back({"IR",       0xFD20,    nullptr});
    _items.push_back({"RFID",     0x07FF,    nullptr});
    _items.push_back({"Files",    C_WHITE,   nullptr});
    _items.push_back({"Scripts",  C_GREEN,   nullptr});
    _items.push_back({"Clock",    C_GRAY,    nullptr});
    _items.push_back({"Extras",   C_GREEN,   nullptr});
    _items.push_back({"Config",   C_GRAY,    nullptr});

    drawStatusBar();
    drawMenuList();

    while (!returnToMenu) {
        // Navigation - encoder up/down
        if (check(UpPress) || check(PrevPress)) {
            _sel = (_sel - 1 + _items.size()) % _items.size();
            _lastActivity = millis();
            drawMenuList();
        }

        // Navigation - encoder down
        if (check(DownPress) || check(NextPress)) {
            _sel = (_sel + 1) % _items.size();
            _lastActivity = millis();
            drawMenuList();
        }

        // Select
        if (check(SelPress)) {
            _lastActivity = millis();
            // Flash selection
            drawMenuList();
            delay(80);

            // Launch Bruce submenu
            extern std::function<void(int)> phantomMenuCallback;
            if (phantomMenuCallback) {
                phantomMenuCallback(_sel);
            }

            // Redraw after submenu returns
            drawStatusBar();
            drawMenuList();
            _lastActivity = millis();
        }

        // Long press back = exit
        if (check(EscPress) && check(PrevPress)) {
            returnToMenu = true;
            break;
        }

        // Idle timeout -> loop continuously until input
        if (_idleEnabled && (millis() - _lastActivity > 30000)) {
            idleLoop();
            _lastActivity = millis();
            drawStatusBar();
            drawMenuList();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ============================================================
//  IDLE SCREEN - loops continuously until user input
// ============================================================
void PhantomMenu::idleLoop() {
    tft.fillScreen(C_BG);
    int w = tftWidth;
    int h = tftHeight;

    int mode = 0;
    int modeCount = 4;

    // Persistent animation state
    int dropX[30], dropY[30];
    bool dropsInit = false;
    int scanY = 0;
    int hexOff = 0;
    int foxPh = 0;

    while (true) {
        if (check(AnyKeyPress) || check(SelPress) || check(UpPress) ||
            check(DownPress) || check(NextPress) || check(PrevPress) ||
            check(EscPress)) {
            delay(50);
            break;
        }

        switch (mode) {
            case 0: { // Matrix rain - loops on its own
                if (!dropsInit) {
                    for (int i = 0; i < 30; i++) {
                        dropX[i] = (i * 11) % w;
                        dropY[i] = random(-h, 0);
                    }
                    dropsInit = true;
                }
                for (int i = 0; i < 30; i++) {
                    // clear tail
                    tft.setTextColor(C_BG, C_BG);
                    char sp[] = " ";
                    tft.drawString(sp, dropX[i], dropY[i] - 10, 1);
                    // move
                    dropY[i] += 8;
                    // draw head
                    if (dropY[i] >= 0 && dropY[i] < h) {
                        char buf[2] = { (char)('0' + random(0, 10)), 0 };
                        tft.setTextColor(C_WHITE, C_BG);
                        tft.drawString(buf, dropX[i], dropY[i], 1);
                    }
                    // green trail
                    int ty = dropY[i] - 8;
                    if (ty >= 0 && ty < h) {
                        char buf[2] = { (char)('0' + random(0, 10)), 0 };
                        tft.setTextColor(C_GREEN, C_BG);
                        tft.drawString(buf, dropX[i], ty, 1);
                    }
                    if (dropY[i] > h + 10) dropY[i] = random(-40, -5);
                }
                delay(40);
                break;
            }
            case 1: { // Fox mask pulse
                int cx = w / 2, cy = h / 2;
                int sc = min(w, h) / 6;
                foxPh = (foxPh + 1) % 360;
                float pulse = 1.0 + 0.12 * sin(foxPh * 0.06);
                int s = sc * pulse;

                tft.fillRect(cx - sc - 15, cy - sc - 15, sc * 2 + 30, sc * 2 + 30, C_BG);

                uint16_t ec = (foxPh % 50 < 25) ? C_GREEN : C_WHITE;

                tft.fillTriangle(cx-s*0.8, cy-s*0.3, cx-s*0.2, cy-s*0.6, cx-s*0.2, cy-s*0.1, ec);
                tft.fillTriangle(cx+s*0.8, cy-s*0.3, cx+s*0.2, cy-s*0.6, cx+s*0.2, cy-s*0.1, ec);
                tft.fillTriangle(cx, cy+s*0.1, cx-s*0.15, cy+s*0.3, cx+s*0.15, cy+s*0.3, C_WHITE);
                tft.fillTriangle(cx-s*0.6, cy-s*0.5, cx-s, cy-s, cx-s*0.2, cy-s*0.7, C_WHITE);
                tft.fillTriangle(cx+s*0.6, cy-s*0.5, cx+s, cy-s, cx+s*0.2, cy-s*0.7, C_WHITE);
                tft.fillTriangle(cx-s*0.55, cy-s*0.55, cx-s*0.85, cy-s*0.9, cx-s*0.3, cy-s*0.65, C_BG);
                tft.fillTriangle(cx+s*0.55, cy-s*0.55, cx+s*0.85, cy-s*0.9, cx+s*0.3, cy-s*0.65, C_BG);
                tft.drawLine(cx, cy+s*0.3, cx, cy+s*0.5, C_WHITE);
                tft.drawLine(cx, cy+s*0.5, cx-s*0.3, cy+s*0.6, C_WHITE);
                tft.drawLine(cx, cy+s*0.5, cx+s*0.3, cy+s*0.6, C_WHITE);

                // orbiting dots
                for (int i = 0; i < 4; i++) {
                    float a = (foxPh + i * 90) * 0.01745;
                    int dx = cx + cos(a) * s * 1.3;
                    int dy = cy + sin(a) * s * 1.3;
                    tft.fillCircle(dx, dy, 2, C_GREEN);
                }
                delay(50);
                break;
            }
            case 2: { // Circuit board
                for (int i = 0; i < 2; i++) {
                    int x1 = random(10, w - 10);
                    int y1 = random(STATUS_H + 5, h - 5);
                    int x2 = x1 + random(-50, 50);
                    int y2 = y1 + random(-25, 25);
                    tft.drawFastHLine(min(x1, x2), y1, abs(x2 - x1) + 1, C_DKGREEN);
                    tft.drawFastVLine(x2, min(y1, y2), abs(y2 - y1) + 1, C_DKGREEN);
                    tft.fillCircle(x2, y1, 2, C_GREEN);
                }
                // scan line
                tft.drawFastHLine(0, scanY, w, C_GREEN);
                scanY = (scanY + 1) % h;
                delay(60);
                break;
            }
            case 3: { // Data scroll
                tft.setTextSize(FP);
                tft.setTextDatum(TL_DATUM);
                for (int r = 0; r < 5; r++) {
                    int y = 20 + r * 28;
                    if (y > h - 20) break;
                    char line[60];
                    snprintf(line, sizeof(line), "%08X  PHANTOM  %04X",
                             random(0, 0xFFFFFFFF), random(0, 0xFFFF));
                    tft.setTextColor(r % 2 == 0 ? C_GREEN : C_DKGRAY, C_BG);
                    tft.drawString(line, 4 - (hexOff % 15), y, 1);
                }
                hexOff = (hexOff + 1) % 80;
                delay(80);
                break;
            }
        }

        // Auto-cycle mode every 8 seconds
        static unsigned long lastSwitch = 0;
        if (millis() - lastSwitch > 8000) {
            mode = (mode + 1) % modeCount;
            lastSwitch = millis();
            tft.fillScreen(C_BG);
        }
    }
    tft.fillScreen(C_BG);
}

// ============================================================
//  SUBMENU - custom themed submenu
// ============================================================
void PhantomMenu::submenu(const char *title, std::vector<PhantomMenuItem> &items) {
    int sel = 0;
    int scroll = 0;
    int w = tftWidth;
    int h = tftHeight;
    int listY = STATUS_H + 14;
    int listH = h - listY;
    int visibleRows = listH / ROW_H;

    auto draw = [&]() {
        tft.fillRect(0, STATUS_H, w, 14, C_BG);
        tft.drawFastHLine(0, STATUS_H + 13, w, C_DKGREEN);
        tft.setTextDatum(TL_DATUM);
        tft.setTextColor(C_GREEN, C_BG);
        tft.setTextSize(FP);
        tft.drawString(title, PAD_X, STATUS_H + 3, 1);

        // back hint
        tft.setTextDatum(TR_DATUM);
        tft.setTextColor(C_DKGRAY, C_BG);
        tft.drawString("ESC:back", w - PAD_X, STATUS_H + 3, 1);

        // clamp scroll
        if (sel < scroll) scroll = sel;
        if (sel >= scroll + visibleRows) scroll = sel - visibleRows + 1;

        tft.fillRect(0, listY, w, listH, C_BG);

        for (int i = 0; i < visibleRows && (scroll + i) < (int)items.size(); i++) {
            int idx = scroll + i;
            int y = listY + i * ROW_H;
            bool isSel = (idx == sel);
            int itemW = w - PAD_X * 2 - SCROLL_W - 2;

            if (isSel) {
                tft.fillRoundRect(PAD_X, y + 1, itemW, ROW_H - 2, 3, C_DKGREEN);
            }

            tft.setTextDatum(TL_DATUM);
            tft.setTextColor(isSel ? C_WHITE : C_GRAY, isSel ? C_DKGREEN : C_BG);
            tft.setTextSize(FP);
            tft.drawString(items[idx].label, PAD_X + 6, y + 6, 1);
        }

        // scroll bar
        if ((int)items.size() > visibleRows) {
            int sbH = max(8, listH * visibleRows / (int)items.size());
            int sbY = listY + (listH - sbH) * sel / max(1, (int)items.size() - 1);
            tft.fillRect(w - SCROLL_W - 1, listY, SCROLL_W, listH, C_DDKGRAY);
            tft.fillRect(w - SCROLL_W - 1, sbY, SCROLL_W, sbH, C_GREEN);
        }
    };

    draw();

    while (true) {
        if (check(UpPress) || check(PrevPress)) {
            sel = (sel - 1 + items.size()) % items.size();
            draw();
        }
        if (check(DownPress) || check(NextPress)) {
            sel = (sel + 1) % items.size();
            draw();
        }
        if (check(SelPress)) {
            if (sel >= 0 && sel < (int)items.size() && items[sel].action) {
                items[sel].action();
                draw();
            }
        }
        if (check(EscPress)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
