#include "phantom_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include <globals.h>

static const uint16_t C_BG   = 0x0000;
static const uint16_t C_WHITE = 0xFFFF;
static const uint16_t C_LGRAY = 0xC618;
static const uint16_t C_DGRAY = 0x4208;

static const int STATUS_H  = 16;
static const int ROW_H     = 24;
static const int PAD_X     = 8;
static const int ICON_W    = 16;
static const int SCROLL_W  = 3;

extern int getBattery();

PhantomMenu::PhantomMenu() {}
PhantomMenu::~PhantomMenu() {}

void PhantomMenu::clearAllInputs() {
    while (AnyKeyPress || SelPress || UpPress || DownPress ||
           NextPress || PrevPress || EscPress) {
        AnyKeyPress = false;
        SelPress = false;
        UpPress = false;
        DownPress = false;
        NextPress = false;
        PrevPress = false;
        EscPress = false;
        delay(5);
    }
}

void PhantomMenu::bootSequence() {
    int w = tftWidth;
    int h = tftHeight;
    tft.fillScreen(C_BG);

    for (int y = 0; y < h; y += 3) {
        tft.drawFastHLine(0, y, w, C_DGRAY);
        if (y > 6) tft.drawFastHLine(0, y - 6, w, C_BG);
        delay(2);
    }
    tft.fillScreen(C_BG);

    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(FM);

    const char *title = "PHANTOM";
    int titleLen = strlen(title);
    int charW = 12 * titleLen;
    int startX = (w - charW) / 2;
    int cy = h / 2 - 10;

    for (int i = 0; i < titleLen; i++) {
        char buf[2] = { title[i], 0 };
        tft.setTextColor(C_WHITE, C_BG);
        tft.drawString(buf, startX + i * 12 + 6, cy, FM);
        delay(80);
    }

    int lineW = titleLen * 12 + 4;
    int lineX = (w - lineW) / 2;
    for (int x = 0; x < lineW; x += 2) {
        tft.drawFastHLine(lineX + x, cy + 12, 2, C_WHITE);
        delay(5);
    }

    tft.setTextSize(FP);
    tft.setTextColor(C_LGRAY, C_BG);
    tft.drawString("v1.0", w / 2, cy + 20, FP);

        delay(600);

    for (int b = 255; b >= 0; b -= 20) {
        tft.drawRect(lineX - 2, cy - 20, lineW + 4, 50, tft.color565(b, b, b));
        delay(5);
    }
    tft.fillScreen(C_BG);
}

void PhantomMenu::drawStatusBar() {
    int w = tftWidth;

    tft.fillRect(0, 0, w, STATUS_H, C_BG);
    tft.drawFastHLine(0, STATUS_H - 1, w, C_DGRAY);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(C_WHITE, C_BG);
    tft.setTextSize(FP);
    tft.drawString("PHANTOM", PAD_X, 3, FP);

    int bat = getBattery();
    char batBuf[8];
    snprintf(batBuf, sizeof(batBuf), "%d%%", bat);

    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(C_LGRAY, C_BG);
    tft.drawString(batBuf, w - PAD_X, 3, FP);

    int bx = w - PAD_X - 30;
    int by = 4;
    int bw = 14;
    int bh = 7;
    tft.drawRect(bx, by, bw, bh, C_DGRAY);
    tft.drawRect(bx + bw, by + 2, 2, 3, C_DGRAY);
    int fill = map(bat, 0, 100, 0, bw - 2);
    tft.fillRect(bx + 1, by + 1, fill, bh - 2, C_WHITE);
}

void PhantomMenu::drawMenuList() {
    int w = tftWidth;
    int h = tftHeight;
    int listY = STATUS_H;
    int listH = h - STATUS_H;
    int visibleRows = listH / ROW_H;

    if (_sel < _scroll) _scroll = _sel;
    if (_sel >= _scroll + visibleRows) _scroll = _sel - visibleRows + 1;

    tft.fillRect(0, listY, w, listH, C_BG);

    for (int i = 0; i < visibleRows && (_scroll + i) < (int)_items.size(); i++) {
        int idx = _scroll + i;
        int y = listY + i * ROW_H;
        bool sel = (idx == _sel);
        drawMenuItem(idx, sel, y);
    }

    if ((int)_items.size() > visibleRows) {
        int sbH = max(8, listH * visibleRows / (int)_items.size());
        int sbY = listY + (listH - sbH) * _sel / max(1, (int)_items.size() - 1);
        tft.fillRect(w - SCROLL_W, listY, SCROLL_W, listH, C_DGRAY);
        tft.fillRect(w - SCROLL_W, sbY, SCROLL_W, sbH, C_WHITE);
    }
}

void PhantomMenu::drawMenuItem(int idx, bool sel, int y) {
    int w = tftWidth;
    int itemW = w - PAD_X * 2 - SCROLL_W - 2;
    int x = PAD_X;

    if (sel) {
        tft.fillRect(x, y + 2, 3, ROW_H - 4, C_WHITE);
    }

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(sel ? C_WHITE : C_LGRAY, C_BG);
    tft.setTextSize(FP);
    tft.drawString(_items[idx].label, x + 8, y + 7, FP);
}

void PhantomMenu::begin() {
    _sel = 0;
    _scroll = 0;
    _lastActivity = millis();
    _idleEnabled = true;

    clearAllInputs();

    _items.clear();
    _items.push_back({"WiFi",   nullptr});
    _items.push_back({"BLE",    nullptr});
    _items.push_back({"RF",     nullptr});
    _items.push_back({"NRF24",  nullptr});
    _items.push_back({"IR",     nullptr});
    _items.push_back({"RFID",   nullptr});
    _items.push_back({"Files",  nullptr});
    _items.push_back({"Scripts", nullptr});
    _items.push_back({"Clock",  nullptr});
    _items.push_back({"Extras", nullptr});
    _items.push_back({"Config", nullptr});

    drawStatusBar();
    drawMenuList();

    while (true) {
        bool handled = false;

        if (PrevPress || check(UpPress)) {
            check(PrevPress);
            _sel = (_sel - 1 + _items.size()) % _items.size();
            _lastActivity = millis();
            drawMenuList();
            handled = true;
        }

        if (!handled && (NextPress || check(DownPress))) {
            check(NextPress);
            _sel = (_sel + 1) % _items.size();
            _lastActivity = millis();
            drawMenuList();
            handled = true;
        }

        if (!handled && (SelPress || check(SelPress))) {
            check(SelPress);
            _lastActivity = millis();
            delay(80);

            clearAllInputs();

            extern std::function<void(int)> phantomMenuCallback;
            if (phantomMenuCallback) {
                phantomMenuCallback(_sel);
            }

            clearAllInputs();

            drawStatusBar();
            drawMenuList();
            _lastActivity = millis();
            handled = true;
        }

        if (!handled && (EscPress || check(EscPress))) {
            check(EscPress);
            clearAllInputs();
            break;
        }

        if (_idleEnabled && !handled && (millis() - _lastActivity > 30000)) {
            idleLoop();
            _lastActivity = millis();
            clearAllInputs();
            drawStatusBar();
            drawMenuList();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void PhantomMenu::idleLoop() {
    tft.fillScreen(C_BG);
    int w = tftWidth;
    int h = tftHeight;

    int mode = 0;
    int modeCount = 3;

    int dropX[20], dropY[20];
    bool dropsInit = false;

    int hexOff = 0;

    int breathPhase = 0;

    unsigned long lastSwitch = millis();

    while (true) {
        if (AnyKeyPress || SelPress || EscPress) {
            break;
        }
        if (digitalRead(SEL_BTN) == BTN_ACT) break;
        if (digitalRead(BK_BTN) == BTN_ACT) break;

        switch (mode) {
            case 0: {
                if (!dropsInit) {
                    for (int i = 0; i < 20; i++) {
                        dropX[i] = (i * 16) % w;
                        dropY[i] = random(-h, 0);
                    }
                    dropsInit = true;
                }
                for (int i = 0; i < 20; i++) {
                    tft.setTextColor(C_BG, C_BG);
                    tft.drawString(" ", dropX[i], dropY[i] - 10, FP);
                    dropY[i] += 8;
                    if (dropY[i] >= 0 && dropY[i] < h) {
                        char buf[2] = { (char)('0' + random(0, 10)), 0 };
                        tft.setTextColor(C_WHITE, C_BG);
                        tft.drawString(buf, dropX[i], dropY[i], FP);
                    }
                    int ty = dropY[i] - 8;
                    if (ty >= 0 && ty < h) {
                        char buf[2] = { (char)('0' + random(0, 10)), 0 };
                        tft.setTextColor(C_DGRAY, C_BG);
                        tft.drawString(buf, dropX[i], ty, FP);
                    }
                    if (dropY[i] > h + 10) dropY[i] = random(-40, -5);
                }
                delay(50);
                break;
            }
            case 1: {
                breathPhase = (breathPhase + 2) % 360;
                float intensity = (sin(breathPhase * 0.01745) + 1.0) / 2.0;
                uint16_t c = tft.color565(
                    (int)(intensity * 200),
                    (int)(intensity * 200),
                    (int)(intensity * 200)
                );
                tft.fillScreen(C_BG);
                for (int i = 0; i < 5; i++) {
                    int ly = h / 2 - 20 + i * 10;
                    int lw = (int)(w * 0.3 * intensity) + 10;
                    int lx = (w - lw) / 2;
                    tft.drawFastHLine(lx, ly, lw, c);
                }
                tft.setTextDatum(MC_DATUM);
                tft.setTextSize(FM);
                tft.setTextColor(c, C_BG);
                tft.drawString("PHANTOM", w / 2, h / 2 - 4, FM);
                delay(30);
                break;
            }
            case 2: {
                tft.setTextSize(FP);
                tft.setTextDatum(TL_DATUM);
                tft.fillScreen(C_BG);
                for (int r = 0; r < 6; r++) {
                    int y = 8 + r * 24;
                    if (y > h - 20) break;
                    char line[48];
                    snprintf(line, sizeof(line), "%08lX  %04X  %08lX",
                             (unsigned long)random(0, 0xFFFFFFFF), (unsigned int)random(0, 0xFFFF), (unsigned long)random(0, 0xFFFFFFFF));
                    tft.setTextColor(r % 2 == 0 ? C_WHITE : C_DGRAY, C_BG);
                    tft.drawString(line, 4, y, FP);
                }
                hexOff++;
                delay(80);
                break;
            }
        }

        if (millis() - lastSwitch > 10000) {
            mode = (mode + 1) % modeCount;
            lastSwitch = millis();
            dropsInit = false;
            tft.fillScreen(C_BG);
    }
}

void PhantomMenu::submenu(const char *title, std::vector<PhantomMenuItem> &items) {
    int sel = 0;
    int scroll = 0;
    int w = tftWidth;
    int h = tftHeight;
    int listY = STATUS_H + 14;
    int listH = h - listY;
    int visibleRows = listH / ROW_H;

    clearAllInputs();

    auto draw = [&]() {
        tft.fillRect(0, STATUS_H, w, 14, C_BG);
        tft.drawFastHLine(0, STATUS_H + 13, w, C_DGRAY);
        tft.setTextDatum(TL_DATUM);
        tft.setTextColor(C_WHITE, C_BG);
        tft.setTextSize(FP);
        tft.drawString(title, PAD_X, STATUS_H + 3, FP);

        tft.setTextDatum(TR_DATUM);
        tft.setTextColor(C_DGRAY, C_BG);
        tft.drawString("ESC", w - PAD_X, STATUS_H + 3, FP);

        if (sel < scroll) scroll = sel;
        if (sel >= scroll + visibleRows) scroll = sel - visibleRows + 1;

        tft.fillRect(0, listY, w, listH, C_BG);

        for (int i = 0; i < visibleRows && (scroll + i) < (int)items.size(); i++) {
            int idx = scroll + i;
            int y = listY + i * ROW_H;
            bool isSel = (idx == sel);
            int itemW = w - PAD_X * 2 - SCROLL_W - 2;

            if (isSel) {
                tft.fillRect(PAD_X, y + 2, 3, ROW_H - 4, C_WHITE);
            }

            tft.setTextDatum(TL_DATUM);
            tft.setTextColor(isSel ? C_WHITE : C_LGRAY, C_BG);
            tft.setTextSize(FP);
            tft.drawString(items[idx].label, PAD_X + 8, y + 7, FP);
        }

        if ((int)items.size() > visibleRows) {
            int sbH = max(8, listH * visibleRows / (int)items.size());
            int sbY = listY + (listH - sbH) * sel / max(1, (int)items.size() - 1);
            tft.fillRect(w - SCROLL_W, listY, SCROLL_W, listH, C_DGRAY);
            tft.fillRect(w - SCROLL_W, sbY, SCROLL_W, sbH, C_WHITE);
        }
    };

    draw();

    while (true) {
        bool handled = false;

        if (PrevPress || check(UpPress)) {
            check(PrevPress);
            sel = (sel - 1 + items.size()) % items.size();
            draw();
            handled = true;
        }

        if (!handled && (NextPress || check(DownPress))) {
            check(NextPress);
            sel = (sel + 1) % items.size();
            draw();
            handled = true;
        }

        if (!handled && (SelPress || check(SelPress))) {
            check(SelPress);
            if (sel >= 0 && sel < (int)items.size() && items[sel].action) {
                clearAllInputs();
                items[sel].action();
                clearAllInputs();
                draw();
            }
            handled = true;
        }

        if (!handled && (EscPress || check(EscPress))) {
            check(EscPress);
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    clearAllInputs();
}
