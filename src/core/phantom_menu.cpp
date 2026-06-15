#include "phantom_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "themes/watchdogs/theme.h"
#include <globals.h>

static const uint16_t PH_GREEN = 0x07E0;
static const uint16_t PH_DARK_GREEN = 0x0320;
static const uint16_t PH_MUTED_GREEN = 0x0400;
static const uint16_t PH_WHITE = 0xFFFF;
static const uint16_t PH_BLACK = 0x0000;
static const uint16_t PH_GRAY = 0x4208;
static const uint16_t PH_DARK_GRAY = 0x2104;

PhantomMenu::PhantomMenu() : _selectedIndex(0), _scrollOffset(0), _lastActivity(millis()) {
    _items = {
        {"WIFI",     PH_GREEN,   nullptr},
        {"BLE",      0x07FF,     nullptr},
        {"RF",       0xFFE0,     nullptr},
        {"NRF24",    0xF81F,     nullptr},
        {"IR",       0xFD20,     nullptr},
        {"RFID",     0x07FF,     nullptr},
        {"FILES",    PH_WHITE,   nullptr},
        {"SCRIPTS",  PH_GREEN,   nullptr},
        {"CLOCK",    PH_GRAY,    nullptr},
        {"EXTRAS",   PH_GREEN,   nullptr},
        {"CONFIG",   PH_GRAY,    nullptr},
    };
}

PhantomMenu::~PhantomMenu() {}

/*********************************************************************
**  Boot Sequence
**********************************************************************/
void PhantomMenu::bootSequence() {
    int w = tftWidth;
    int h = tftHeight;
    tft.fillScreen(PH_BLACK);

    // Phase 1: Scanlines sweep
    for (int y = 0; y < h; y += 2) {
        tft.drawFastHLine(0, y, w, PH_DARK_GREEN);
        if (y > 10) tft.drawFastHLine(0, y - 10, w, PH_BLACK);
        delay(5);
    }
    tft.fillScreen(PH_BLACK);

    // Phase 2: Falling characters (matrix style)
    for (int frame = 0; frame < 25; frame++) {
        for (int col = 0; col < w; col += 14) {
            int startY = random(-h, 0);
            int dropLen = random(3, 8);
            for (int i = 0; i < dropLen; i++) {
                int y = startY + i * 14;
                if (y >= 0 && y < h) {
                    char buf[2] = { (char)('0' + random(0, 10)), 0 };
                    uint16_t c = (i == 0) ? PH_WHITE : PH_GREEN;
                    tft.setTextColor(c, PH_BLACK);
                    tft.drawString(buf, col, y, 1);
                }
            }
        }
        delay(40);
        // Clear for next frame
        tft.fillScreen(PH_BLACK);
    }

    // Phase 3: Fox mask glitch-in
    int cx = w / 2;
    int cy = h / 2;
    int scale = min(w, h) / 5;

    for (int glitch = 0; glitch < 12; glitch++) {
        int ox = random(-6, 6);
        int oy = random(-4, 4);
        uint16_t c = (glitch % 3 == 0) ? PH_GREEN : PH_WHITE;

        tft.fillTriangle(cx - scale*0.8 + ox, cy - scale*0.3 + oy,
                         cx - scale*0.2 + ox, cy - scale*0.6 + oy,
                         cx - scale*0.2 + ox, cy - scale*0.1 + oy, c);
        tft.fillTriangle(cx + scale*0.8 + ox, cy - scale*0.3 + oy,
                         cx + scale*0.2 + ox, cy - scale*0.6 + oy,
                         cx + scale*0.2 + ox, cy - scale*0.1 + oy, c);
        tft.fillTriangle(cx + ox, cy + scale*0.1 + oy,
                         cx - scale*0.15 + ox, cy + scale*0.3 + oy,
                         cx + scale*0.15 + ox, cy + scale*0.3 + oy, c);
        tft.fillTriangle(cx - scale*0.6 + ox, cy - scale*0.5 + oy,
                         cx - scale*1.0 + ox, cy - scale*1.0 + oy,
                         cx - scale*0.2 + ox, cy - scale*0.7 + oy, c);
        tft.fillTriangle(cx + scale*0.6 + ox, cy - scale*0.5 + oy,
                         cx + scale*1.0 + ox, cy - scale*1.0 + oy,
                         cx + scale*0.2 + ox, cy - scale*0.7 + oy, c);

        delay(30);
        if (glitch < 11) {
            tft.fillTriangle(cx - scale*0.8 + ox, cy - scale*0.3 + oy,
                             cx - scale*0.2 + ox, cy - scale*0.6 + oy,
                             cx - scale*0.2 + ox, cy - scale*0.1 + oy, PH_BLACK);
            tft.fillTriangle(cx + scale*0.8 + ox, cy - scale*0.3 + oy,
                             cx + scale*0.2 + ox, cy - scale*0.6 + oy,
                             cx + scale*0.2 + ox, cy - scale*0.1 + oy, PH_BLACK);
            tft.fillTriangle(cx + ox, cy + scale*0.1 + oy,
                             cx - scale*0.15 + ox, cy + scale*0.3 + oy,
                             cx + scale*0.15 + ox, cy + scale*0.3 + oy, PH_BLACK);
            tft.fillTriangle(cx - scale*0.6 + ox, cy - scale*0.5 + oy,
                             cx - scale*1.0 + ox, cy - scale*1.0 + oy,
                             cx - scale*0.2 + ox, cy - scale*0.7 + oy, PH_BLACK);
            tft.fillTriangle(cx + scale*0.6 + ox, cy - scale*0.5 + oy,
                             cx + scale*1.0 + ox, cy - scale*1.0 + oy,
                             cx + scale*0.2 + ox, cy - scale*0.7 + oy, PH_BLACK);
        }
    }

    // Phase 4: Clean fox mask
    tft.fillTriangle(cx - scale*0.8, cy - scale*0.3, cx - scale*0.2, cy - scale*0.6,
                     cx - scale*0.2, cy - scale*0.1, PH_WHITE);
    tft.fillTriangle(cx + scale*0.8, cy - scale*0.3, cx + scale*0.2, cy - scale*0.6,
                     cx + scale*0.2, cy - scale*0.1, PH_WHITE);
    tft.fillTriangle(cx, cy + scale*0.1, cx - scale*0.15, cy + scale*0.3,
                     cx + scale*0.15, cy + scale*0.3, PH_WHITE);
    tft.fillTriangle(cx - scale*0.6, cy - scale*0.5, cx - scale*1.0, cy - scale*1.0,
                     cx - scale*0.2, cy - scale*0.7, PH_WHITE);
    tft.fillTriangle(cx + scale*0.6, cy - scale*0.5, cx + scale*1.0, cy - scale*1.0,
                     cx + scale*0.2, cy - scale*0.7, PH_WHITE);
    tft.fillTriangle(cx - scale*0.55, cy - scale*0.55, cx - scale*0.85, cy - scale*0.9,
                     cx - scale*0.3, cy - scale*0.65, PH_BLACK);
    tft.fillTriangle(cx + scale*0.55, cy - scale*0.55, cx + scale*0.85, cy - scale*0.9,
                     cx + scale*0.3, cy - scale*0.65, PH_BLACK);
    tft.drawLine(cx, cy + scale*0.3, cx, cy + scale*0.5, PH_WHITE);
    tft.drawLine(cx, cy + scale*0.5, cx - scale*0.3, cy + scale*0.6, PH_WHITE);
    tft.drawLine(cx, cy + scale*0.5, cx + scale*0.3, cy + scale*0.6, PH_WHITE);

    for (int i = 0; i < 8; i++) {
        int sx = cx + (random(0,2) ? 1 : -1) * scale * (0.5 + random(0,50)/100.0);
        int sy = cy + random(-scale, scale) * 0.5;
        int ex = sx + (random(0,2) ? 1 : -1) * scale * (0.3 + random(0,50)/100.0);
        int ey = sy + random(-scale/2, scale/2);
        tft.drawLine(sx, sy, ex, ey, PH_DARK_GREEN);
        tft.fillCircle(ex, ey, 2, PH_GREEN);
    }

    delay(500);

    // Phase 5: Text
    tft.setTextDatum(MC_DATUM);
    const char *lines[] = {
        "DEDSEC NETWORK",
        "PHANTOM v1.0",
        "STATUS: ONLINE",
        ">>> ACCESS GRANTED <<<"
    };
    int ty = cy + scale + 15;
    for (int i = 0; i < 4; i++) {
        int len = strlen(lines[i]);
        for (int pos = 0; pos <= len; pos++) {
            char buf[64];
            strncpy(buf, lines[i], pos);
            buf[pos] = '\0';
            tft.setTextColor(PH_GREEN, PH_BLACK);
            tft.drawString(buf, w/2, ty, 1);
            delay(20);
        }
        ty += 14;
    }

    delay(600);

    // Phase 6: Fade out
    for (int brightness = 255; brightness >= 0; brightness -= 8) {
        uint16_t color = tft.color565(brightness/8, brightness/4, brightness/8);
        tft.drawRect(cx - scale - 20, cy - scale - 20, (scale + 20) * 2, (scale + 20) * 2 + 60, color);
        delay(10);
    }
    tft.fillScreen(PH_BLACK);
}

/*********************************************************************
**  Draw main menu
**********************************************************************/
void PhantomMenu::drawMenu() {
    tft.fillScreen(PH_BLACK);

    // Title
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(PH_GREEN, PH_BLACK);
    tft.setTextSize(FP);
    tft.drawString("PHANTOM", 5, 4, 1);

    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(PH_GRAY, PH_BLACK);
    tft.drawString("v1.0", tftWidth - 5, 4, 1);

    tft.drawFastHLine(0, 16, tftWidth, PH_DARK_GREEN);

    drawGrid(_selectedIndex);

    // Footer
    tft.drawFastHLine(0, tftHeight - 18, tftWidth, PH_DARK_GREEN);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(PH_GRAY, PH_BLACK);
    tft.setTextSize(FP);
    tft.drawString("OK:SELECT  UP/DOWN:NAV", tftWidth / 2, tftHeight - 10, 1);

    // Scan lines
    for (int y = 0; y < tftHeight; y += 3) {
        tft.drawFastHLine(0, y, tftWidth, 0x0008);
    }
}

void PhantomMenu::drawGrid(int selectedIdx) {
    int w = tftWidth;
    int cols = 3;
    int padX = 8;
    int padY = 22;
    int footerH = 20;
    int cellW = (w - 2 * padX) / cols;
    int cellH = 36;

    for (int i = 0; i < (int)_items.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        int x = padX + col * cellW;
        int y = padY + row * cellH;
        bool sel = (i == selectedIdx);

        // Cell bg
        if (sel) {
            tft.fillRect(x, y, cellW - 2, cellH - 2, PH_DARK_GREEN);
            tft.drawRect(x, y, cellW - 2, cellH - 2, PH_GREEN);
        } else {
            tft.drawRect(x, y, cellW - 2, cellH - 2, PH_DARK_GRAY);
        }

        int iconX = x + cellW / 2;
        int iconY = y + 10;
        uint16_t ic = sel ? PH_GREEN : PH_GRAY;

        // Draw icons with geometric shapes
        switch (i) {
            case 0: // WiFi
                for (int a = 0; a < 3; a++)
                    tft.drawArc(iconX, iconY, 4 + a*4, 2 + a*4, 220, 320, ic, PH_BLACK);
                break;
            case 1: // BLE
                tft.fillTriangle(iconX, iconY-6, iconX-5, iconY, iconX+5, iconY, ic);
                tft.fillTriangle(iconX, iconY+6, iconX-5, iconY, iconX+5, iconY, ic);
                break;
            case 2: // RF
                tft.drawArc(iconX, iconY, 8, 6, 300, 60, ic, PH_BLACK);
                tft.drawArc(iconX, iconY, 12, 10, 300, 60, ic, PH_BLACK);
                break;
            case 3: // NRF24
                tft.drawRect(iconX-5, iconY-4, 10, 8, ic);
                tft.fillRect(iconX-2, iconY-1, 4, 2, ic);
                break;
            case 4: // IR
                tft.fillCircle(iconX, iconY, 4, ic);
                tft.drawFastHLine(iconX+5, iconY, 8, ic);
                break;
            case 5: // RFID
                tft.drawRect(iconX-6, iconY-4, 12, 8, ic);
                tft.drawFastHLine(iconX-4, iconY, 8, ic);
                break;
            case 6: // Files
                tft.drawRect(iconX-5, iconY-3, 10, 7, ic);
                tft.fillRect(iconX-5, iconY-5, 5, 2, ic);
                break;
            case 7: // Scripts
                tft.drawRect(iconX-5, iconY-5, 5, 10, ic);
                tft.drawRect(iconX+1, iconY-5, 5, 10, ic);
                break;
            case 8: // Clock
                tft.drawCircle(iconX, iconY, 6, ic);
                tft.drawLine(iconX, iconY, iconX, iconY-4, ic);
                tft.drawLine(iconX, iconY, iconX+3, iconY, ic);
                break;
            case 9: // Extras
                tft.drawFastHLine(iconX-5, iconY, 10, ic);
                tft.drawFastVLine(iconX, iconY-5, 10, ic);
                break;
            case 10: // Config
                tft.drawCircle(iconX, iconY, 5, ic);
                tft.fillCircle(iconX, iconY, 2, ic);
                break;
        }

        // Label
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(sel ? PH_WHITE : PH_GRAY, sel ? PH_DARK_GREEN : PH_BLACK);
        tft.setTextSize(FP);
        tft.drawString(_items[i].label, iconX, y + cellH - 8, 1);
    }
}

/*********************************************************************
**  Idle screen - plays until user presses a button
**********************************************************************/
void PhantomMenu::idleScreen() {
    tft.fillScreen(PH_BLACK);

    int w = tftWidth;
    int h = tftHeight;

    unsigned long lastMode = millis();
    int mode = 0;

    // Persistent state for animations
    int dropX[30], dropY[30];
    bool dropsInit = false;
    int scanY = 0;
    int hexOffset = 0;
    int foxPhase = 0;

    while (true) {
        if (check(AnyKeyPress)) {
            delay(50);
            break;
        }

        if (millis() - lastMode > 10000) {
            mode = (mode + 1) % 4;
            lastMode = millis();
            tft.fillScreen(PH_BLACK);
        }

        switch (mode) {
            case 0: { // Matrix rain
                if (!dropsInit) {
                    for (int i = 0; i < 30; i++) {
                        dropX[i] = (i * 8) % w;
                        dropY[i] = random(-h, 0);
                    }
                    dropsInit = true;
                }
                for (int i = 0; i < 30; i++) {
                    tft.setTextColor(PH_BLACK, PH_BLACK);
                    char space[] = " ";
                    tft.drawString(space, dropX[i], dropY[i], 1);

                    dropY[i] += 8;

                    if (dropY[i] >= 0 && dropY[i] < h) {
                        char buf[2] = { (char)('0' + random(0, 10)), 0 };
                        tft.setTextColor(PH_WHITE, PH_BLACK);
                        tft.drawString(buf, dropX[i], dropY[i], 1);
                    }

                    int trailY = dropY[i] - 8;
                    if (trailY >= 0 && trailY < h) {
                        char buf[2] = { (char)('0' + random(0, 10)), 0 };
                        tft.setTextColor(PH_GREEN, PH_BLACK);
                        tft.drawString(buf, dropX[i], trailY, 1);
                    }

                    if (dropY[i] > h + 20) {
                        dropY[i] = random(-50, -10);
                    }
                }
                delay(50);
                break;
            }
            case 1: { // Fox mask pulsing
                int cx = w / 2;
                int cy = h / 2;
                int scale = min(w, h) / 5;
                foxPhase = (foxPhase + 1) % 360;
                float pulse = 1.0 + 0.15 * sin(foxPhase * 0.05);
                int s = scale * pulse;

                tft.fillRect(cx - scale - 20, cy - scale - 20, (scale+20)*2, (scale+20)*2 + 20, PH_BLACK);

                uint16_t eyeColor = (foxPhase % 60 < 30) ? PH_GREEN : PH_WHITE;

                tft.fillTriangle(cx - s*0.8, cy - s*0.3, cx - s*0.2, cy - s*0.6,
                                 cx - s*0.2, cy - s*0.1, eyeColor);
                tft.fillTriangle(cx + s*0.8, cy - s*0.3, cx + s*0.2, cy - s*0.6,
                                 cx + s*0.2, cy - s*0.1, eyeColor);
                tft.fillTriangle(cx, cy + s*0.1, cx - s*0.15, cy + s*0.3,
                                 cx + s*0.15, cy + s*0.3, PH_WHITE);
                tft.fillTriangle(cx - s*0.6, cy - s*0.5, cx - s*1.0, cy - s*1.0,
                                 cx - s*0.2, cy - s*0.7, PH_WHITE);
                tft.fillTriangle(cx + s*0.6, cy - s*0.5, cx + s*1.0, cy - s*1.0,
                                 cx + s*0.2, cy - s*0.7, PH_WHITE);
                tft.fillTriangle(cx - s*0.55, cy - s*0.55, cx - s*0.85, cy - s*0.9,
                                 cx - s*0.3, cy - s*0.65, PH_BLACK);
                tft.fillTriangle(cx + s*0.55, cy - s*0.55, cx + s*0.85, cy - s*0.9,
                                 cx + s*0.3, cy - s*0.65, PH_BLACK);
                tft.drawLine(cx, cy + s*0.3, cx, cy + s*0.5, PH_WHITE);
                tft.drawLine(cx, cy + s*0.5, cx - s*0.3, cy + s*0.6, PH_WHITE);
                tft.drawLine(cx, cy + s*0.5, cx + s*0.3, cy + s*0.6, PH_WHITE);

                for (int i = 0; i < 4; i++) {
                    float angle = (foxPhase + i * 90) * 0.01745;
                    int lx = cx + cos(angle) * (s * 1.3);
                    int ly = cy + sin(angle) * (s * 1.3);
                    int ex = lx + cos(angle) * 15;
                    int ey = ly + sin(angle) * 15;
                    tft.drawLine(lx, ly, ex, ey, PH_DARK_GREEN);
                    tft.fillCircle(ex, ey, 2, PH_GREEN);
                }
                delay(60);
                break;
            }
            case 2: { // Circuit board
                for (int i = 0; i < 3; i++) {
                    int x1 = random(0, w);
                    int y1 = random(0, h);
                    int x2 = x1 + random(-60, 60);
                    int y2 = y1 + random(-30, 30);
                    tft.drawFastHLine(min(x1,x2), y1, abs(x2-x1), PH_DARK_GREEN);
                    tft.drawFastVLine(x2, min(y1,y2), abs(y2-y1), PH_DARK_GREEN);
                    tft.fillCircle(x2, y1, 2, PH_GREEN);
                }
                tft.drawFastHLine(0, scanY, w, PH_GREEN);
                tft.drawFastHLine(0, scanY - 1, w, PH_DARK_GREEN);
                scanY = (scanY + 2) % h;
                delay(80);
                break;
            }
            case 3: { // Data stream
                tft.setTextSize(FP);
                tft.setTextDatum(TL_DATUM);
                for (int row = 0; row < 6; row++) {
                    int y = 25 + row * 40;
                    if (y > h - 30) break;
                    char line[64];
                    snprintf(line, sizeof(line), "%08X  PHANTOM  %04X  %08X",
                             random(0, 0xFFFFFFFF), random(0, 0xFFFF), random(0, 0xFFFFFFFF));
                    uint16_t color = (row % 3 == 0) ? PH_GREEN : PH_GRAY;
                    tft.setTextColor(color, PH_BLACK);
                    tft.drawString(line, 5 - (hexOffset % 20), y, 1);
                }
                hexOffset = (hexOffset + 1) % 100;
                delay(100);
                break;
            }
        }
    }
    tft.fillScreen(PH_BLACK);
}

/*********************************************************************
**  Main menu loop
**********************************************************************/
void PhantomMenu::begin() {
    returnToMenu = false;
    _selectedIndex = 0;
    _lastActivity = millis();

    drawMenu();

    while (!returnToMenu) {
        if (check(SelPress) || check(AnyKeyPress)) {
            _lastActivity = millis();

            // Glitch transition
            tft.fillScreen(PH_BLACK);
            for (int i = 0; i < 5; i++) {
                int ox = random(-4, 4);
                int oy = random(-3, 3);
                tft.setTextColor(PH_GREEN, PH_BLACK);
                tft.setTextDatum(MC_DATUM);
                tft.drawString(_items[_selectedIndex].label, tftWidth/2 + ox, tftHeight/2 + oy, 1);
                delay(30);
            }
            tft.fillScreen(PH_BLACK);

            // Launch Bruce submenu
            extern std::function<void(int)> phantomMenuCallback;
            if (phantomMenuCallback) {
                phantomMenuCallback(_selectedIndex);
            }

            _lastActivity = millis();
            drawMenu();
        }

        if (check(UpPress) || check(PrevPress)) {
            _selectedIndex = (_selectedIndex - 1 + _items.size()) % _items.size();
            _lastActivity = millis();
            drawMenu();
            delay(100);
        }

        if (check(DownPress) || check(NextPress)) {
            _selectedIndex = (_selectedIndex + 1) % _items.size();
            _lastActivity = millis();
            drawMenu();
            delay(100);
        }

        // Idle timeout
        if (millis() - _lastActivity > IDLE_TIMEOUT_MS) {
            idleScreen();
            _lastActivity = millis();
            drawMenu();
        }

        delay(10);
    }
}
