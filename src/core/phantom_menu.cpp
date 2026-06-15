#include "phantom_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "themes/watchdogs/theme.h"
#include <globals.h>

// Forward declarations of menu actions
extern void wifiMenuAction();
extern void bleMenuAction();
extern void rfMenuAction();
extern void nrf24MenuAction();
extern void irMenuAction();
extern void rfidMenuAction();
extern void fileMenuAction();
extern void scriptsMenuAction();
extern void clockMenuAction();
extern void othersMenuAction();
extern void configMenuAction();

// Colors
static const uint16_t PH_GREEN = 0x07E0;       // Bright green
static const uint16_t PH_DARK_GREEN = 0x0320;   // Dark green
static const uint16_t PH_MUTED_GREEN = 0x0400;  // Muted green
static const uint16_t PH_WHITE = 0xFFFF;
static const uint16_t PH_BLACK = 0x0000;
static const uint16_t PH_GRAY = 0x4208;
static const uint16_t PH_DARK_GRAY = 0x2104;
static const uint16_t PH_RED = 0xF800;

PhantomMenu::PhantomMenu() : _selectedIndex(0), _scrollOffset(0), _lastActivity(millis()) {
    _items = {
        {"WIFI",     "wifi",     PH_GREEN,   nullptr},
        {"BLE",      "ble",      0x07FF,     nullptr},
        {"RF",       "rf",       0xFFE0,     nullptr},
        {"NRF24",    "nrf",      0xF81F,     nullptr},
        {"IR",       "ir",       0xFD20,     nullptr},
        {"RFID",     "rfid",     0x07FF,     nullptr},
        {"FILES",    "file",     PH_WHITE,   nullptr},
        {"SCRIPTS",  "script",   PH_GREEN,   nullptr},
        {"CLOCK",    "clock",    PH_GRAY,    nullptr},
        {"EXTRAS",   "others",   PH_GREEN,   nullptr},
        {"CONFIG",   "config",   PH_GRAY,    nullptr},
    };
}

PhantomMenu::~PhantomMenu() {}

/*********************************************************************
**  Boot Sequence - Full Watch Dogs animated boot
**********************************************************************/
void PhantomMenu::bootSequence() {
    int w = tftWidth;
    int h = tftHeight;

    tft.fillScreen(PH_BLACK);

    // Phase 1: Scanlines sweep down
    for (int y = 0; y < h; y += 2) {
        tft.drawFastHLine(0, y, w, PH_DARK_GREEN);
        if (y > 10) tft.drawFastHLine(0, y - 10, w, PH_BLACK);
        delay(5);
    }
    tft.fillScreen(PH_BLACK);

    // Phase 2: Matrix rain characters falling
    const char *chars = "01アイウエオカキクケコ";
    for (int frame = 0; frame < 30; frame++) {
        for (int col = 0; col < w; col += 12) {
            int dropLen = random(3, 10);
            int startY = random(-h, 0);
            for (int i = 0; i < dropLen; i++) {
                int y = startY + i * 12;
                if (y >= 0 && y < h) {
                    char c = chars[random(0, strlen(chars))];
                    uint16_t color = (i == 0) ? PH_WHITE : PH_GREEN;
                    if (i == dropLen - 1) color = PH_DARK_GREEN;
                    tft.drawChar(col, y, c, color, PH_BLACK, 1);
                }
            }
        }
        delay(40);
        // Fade previous frame
        for (int col = 0; col < w; col += 12) {
            for (int y = 0; y < h; y += 12) {
                tft.drawChar(col, y, ' ', PH_BLACK, PH_BLACK, 1);
            }
        }
    }
    tft.fillScreen(PH_BLACK);

    // Phase 3: Fox mask glitch-in
    int cx = w / 2;
    int cy = h / 2;
    int scale = min(w, h) / 5;

    for (int glitch = 0; glitch < 12; glitch++) {
        int ox = random(-6, 6);
        int oy = random(-4, 4);
        uint16_t c = (glitch % 3 == 0) ? PH_GREEN : PH_WHITE;

        // Eyes
        tft.fillTriangle(cx - scale*0.8 + ox, cy - scale*0.3 + oy,
                         cx - scale*0.2 + ox, cy - scale*0.6 + oy,
                         cx - scale*0.2 + ox, cy - scale*0.1 + oy, c);
        tft.fillTriangle(cx + scale*0.8 + ox, cy - scale*0.3 + oy,
                         cx + scale*0.2 + ox, cy - scale*0.6 + oy,
                         cx + scale*0.2 + ox, cy - scale*0.1 + oy, c);
        // Nose
        tft.fillTriangle(cx + ox, cy + scale*0.1 + oy,
                         cx - scale*0.15 + ox, cy + scale*0.3 + oy,
                         cx + scale*0.15 + ox, cy + scale*0.3 + oy, c);
        // Ears
        tft.fillTriangle(cx - scale*0.6 + ox, cy - scale*0.5 + oy,
                         cx - scale*1.0 + ox, cy - scale*1.0 + oy,
                         cx - scale*0.2 + ox, cy - scale*0.7 + oy, c);
        tft.fillTriangle(cx + scale*0.6 + ox, cy - scale*0.5 + oy,
                         cx + scale*1.0 + ox, cy - scale*1.0 + oy,
                         cx + scale*0.2 + ox, cy - scale*0.7 + oy, c);

        delay(30);
        if (glitch < 11) {
            // Erase for next glitch frame
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

    // Phase 4: Final clean fox mask
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
    // Inner ears
    tft.fillTriangle(cx - scale*0.55, cy - scale*0.55, cx - scale*0.85, cy - scale*0.9,
                     cx - scale*0.3, cy - scale*0.65, PH_BLACK);
    tft.fillTriangle(cx + scale*0.55, cy - scale*0.55, cx + scale*0.85, cy - scale*0.9,
                     cx + scale*0.3, cy - scale*0.65, PH_BLACK);
    // Mouth
    tft.drawLine(cx, cy + scale*0.3, cx, cy + scale*0.5, PH_WHITE);
    tft.drawLine(cx, cy + scale*0.5, cx - scale*0.3, cy + scale*0.6, PH_WHITE);
    tft.drawLine(cx, cy + scale*0.5, cx + scale*0.3, cy + scale*0.6, PH_WHITE);
    // Circuit lines
    for (int i = 0; i < 8; i++) {
        int sx = cx + (random(0,2) ? 1 : -1) * scale * (0.5 + random(0,50)/100.0);
        int sy = cy + random(-scale, scale) * 0.5;
        int ex = sx + (random(0,2) ? 1 : -1) * scale * (0.3 + random(0,50)/100.0);
        int ey = sy + random(-scale/2, scale/2);
        tft.drawLine(sx, sy, ex, ey, PH_DARK_GREEN);
        tft.fillCircle(ex, ey, 2, PH_GREEN);
    }

    delay(500);

    // Phase 5: Text overlay
    tft.setTextDatum(MC_DATUM);
    const char *lines[] = {
        "DEDSEC NETWORK",
        "PHANTOM v1.0",
        "STATUS: ONLINE",
        ">>> ACCESS GRANTED <<<"
    };
    int ty = cy + scale + 15;
    for (int i = 0; i < 4; i++) {
        // Glitch each character in
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
**  Draw main menu grid
**********************************************************************/
void PhantomMenu::drawMenu() {
    tft.fillScreen(PH_BLACK);

    // Title bar
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(PH_GREEN, PH_BLACK);
    tft.setTextSize(FP);
    tft.drawString("PHANTOM", 5, 4, 1);

    // Status bar right side
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(PH_GRAY, PH_BLACK);

    // Version
    tft.drawString("v1.0", tftWidth - 5, 4, 1);

    // Separator line
    tft.drawFastHLine(0, 16, tftWidth, PH_DARK_GREEN);

    // Draw grid
    drawGrid(_selectedIndex);

    // Footer
    tft.drawFastHLine(0, tftHeight - 18, tftWidth, PH_DARK_GREEN);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(PH_GRAY, PH_BLACK);
    tft.setTextSize(FP);
    tft.drawString("SELECT:OK  UP/DOWN:NAV", tftWidth / 2, tftHeight - 10, 1);

    // Scan lines overlay
    drawScanLines();
}

void PhantomMenu::drawGrid(int selectedIdx) {
    int w = tftWidth;
    int h = tftHeight;

    // Calculate grid layout - 3 columns
    int cols = 3;
    int padX = 8;
    int padY = 22;  // below title
    int footerH = 20;
    int availableH = h - padY - footerH;
    int cellW = (w - 2 * padX) / cols;
    int cellH = 36;
    int rows = (_items.size() + cols - 1) / cols;

    // Draw each cell
    for (int i = 0; i < (int)_items.size(); i++) {
        int col = i % cols;
        int row = i / cols;

        int x = padX + col * cellW;
        int y = padY + row * cellH;
        bool selected = (i == selectedIdx);

        // Cell background
        if (selected) {
            tft.fillRect(x, y, cellW - 2, cellH - 2, PH_DARK_GREEN);
            tft.drawRect(x, y, cellW - 2, cellH - 2, PH_GREEN);
        } else {
            tft.drawRect(x, y, cellW - 2, cellH - 2, PH_DARK_GRAY);
        }

        // Icon - draw a small symbol based on type
        int iconX = x + cellW/2;
        int iconY = y + 10;
        uint16_t iconColor = selected ? PH_GREEN : PH_GRAY;

        // Draw simple geometric icons
        switch (i) {
            case 0: // WiFi - arcs
                for (int a = 0; a < 3; a++) {
                    tft.drawArc(iconX, iconY, 4 + a*4, 2 + a*4, 220, 320, iconColor, PH_BLACK);
                }
                break;
            case 1: // BLE - diamond
                tft.fillTriangle(iconX, iconY - 6, iconX - 5, iconY, iconX + 5, iconY, iconColor);
                tft.fillTriangle(iconX, iconY + 6, iconX - 5, iconY, iconX + 5, iconY, iconColor);
                break;
            case 2: // RF - signal waves
                tft.drawArc(iconX, iconY, 8, 6, 300, 60, iconColor, PH_BLACK);
                tft.drawArc(iconX, iconY, 12, 10, 300, 60, iconColor, PH_BLACK);
                break;
            case 3: // NRF24 - chip
                tft.drawRect(iconX - 5, iconY - 4, 10, 8, iconColor);
                tft.fillRect(iconX - 2, iconY - 1, 4, 2, iconColor);
                break;
            case 4: // IR - beam
                tft.fillCircle(iconX, iconY, 4, iconColor);
                tft.drawFastHLine(iconX + 5, iconY, 8, iconColor);
                break;
            case 5: // RFID - card
                tft.drawRect(iconX - 6, iconY - 4, 12, 8, iconColor);
                tft.drawFastHLine(iconX - 4, iconY, 8, iconColor);
                break;
            case 6: // Files - folder
                tft.drawRect(iconX - 5, iconY - 3, 10, 7, iconColor);
                tft.fillRect(iconX - 5, iconY - 5, 5, 2, iconColor);
                break;
            case 7: // Scripts - brackets
                tft.drawChar(iconX - 4, iconY - 4, '{', iconColor, PH_BLACK, 1);
                tft.drawChar(iconX + 2, iconY - 4, '}', iconColor, PH_BLACK, 1);
                break;
            case 8: // Clock - circle
                tft.drawCircle(iconX, iconY, 6, iconColor);
                tft.drawLine(iconX, iconY, iconX, iconY - 4, iconColor);
                tft.drawLine(iconX, iconY, iconX + 3, iconY, iconColor);
                break;
            case 9: // Extras - plus
                tft.drawFastHLine(iconX - 5, iconY, 10, iconColor);
                tft.drawFastVLine(iconX, iconY - 5, 10, iconColor);
                break;
            case 10: // Config - gear
                tft.drawCircle(iconX, iconY, 5, iconColor);
                tft.fillCircle(iconX, iconY, 2, iconColor);
                break;
        }

        // Label
        tft.setTextDatum(MC_DATUM);
        tft.setTextColor(selected ? PH_WHITE : PH_GRAY, selected ? PH_DARK_GREEN : PH_BLACK);
        tft.setTextSize(FP);
        tft.drawString(_items[i].label, iconX, y + cellH - 8, 1);
    }
}

void PhantomMenu::drawScanLines() {
    for (int y = 0; y < tftHeight; y += 3) {
        tft.drawFastHLine(0, y, tftWidth, 0x0008); // Very subtle dark line
    }
}

void PhantomMenu::glitchText(int x, int y, const char *text, uint16_t color, int iterations) {
    for (int i = 0; i < iterations; i++) {
        int ox = random(-2, 2);
        int oy = random(-1, 1);
        uint16_t c = (i % 2 == 0) ? color : PH_WHITE;
        tft.setTextColor(c, PH_BLACK);
        tft.drawString(text, x + ox, y + oy, 1);
        delay(30);
    }
    tft.setTextColor(color, PH_BLACK);
    tft.drawString(text, x, y, 1);
}

void PhantomMenu::scanLineEffect(int y, int height, uint16_t color) {
    for (int i = 0; i < height; i++) {
        tft.drawFastHLine(0, y + i, tftWidth, color);
    }
}

void PhantomMenu::matrixRain(int duration) {
    unsigned long start = millis();
    const char *chars = "01アイウエオカキクケコサシスセソ";
    int charLen = strlen(chars);
    int w = tftWidth;
    int h = tftHeight;

    // Track columns
    struct Drop { int x; int y; int len; char lastChar; };
    static Drop drops[40];
    int numDrops = min(w / 8, 40);

    // Init drops
    for (int i = 0; i < numDrops; i++) {
        drops[i].x = i * 8;
        drops[i].y = random(-h, 0);
        drops[i].len = random(5, 15);
        drops[i].lastChar = ' ';
    }

    while (millis() - start < duration) {
        for (int i = 0; i < numDrops; i++) {
            // Erase tail
            int tailY = drops[i].y - drops[i].len * 10;
            if (tailY >= 0 && tailY < h) {
                tft.drawChar(drops[i].x, tailY, ' ', PH_BLACK, PH_BLACK, 1);
            }

            // Draw head
            drops[i].y += 10;
            if (drops[i].y < h && drops[i].y >= 0) {
                char c = chars[random(0, charLen)];
                tft.drawChar(drops[i].x, drops[i].y, c, PH_WHITE, PH_BLACK, 1);
            }

            // Draw body
            int bodyY = drops[i].y - 10;
            if (bodyY >= 0 && bodyY < h) {
                char c = chars[random(0, charLen)];
                tft.drawChar(drops[i].x, bodyY, c, PH_GREEN, PH_BLACK, 1);
            }

            // Reset when off screen
            if (drops[i].y - drops[i].len * 10 > h) {
                drops[i].y = random(-h, 0);
                drops[i].len = random(5, 15);
            }
        }
        delay(30);
        if (check(AnyKeyPress)) break;
    }
}

void PhantomMenu::foxMaskPulse(int cx, int cy, int scale) {
    static bool toggled = false;
    toggled = !toggled;
    uint16_t c = toggled ? PH_GREEN : PH_WHITE;

    // Eyes
    tft.fillTriangle(cx - scale*0.8, cy - scale*0.3, cx - scale*0.2, cy - scale*0.6,
                     cx - scale*0.2, cy - scale*0.1, c);
    tft.fillTriangle(cx + scale*0.8, cy - scale*0.3, cx + scale*0.2, cy - scale*0.6,
                     cx + scale*0.2, cy - scale*0.1, c);
    // Nose
    tft.fillTriangle(cx, cy + scale*0.1, cx - scale*0.15, cy + scale*0.3,
                     cx + scale*0.15, cy + scale*0.3, c);
}

/*********************************************************************
**  Idle screen animations - play until user input
**********************************************************************/
void PhantomMenu::idleScreen() {
    tft.fillScreen(PH_BLACK);

    int w = tftWidth;
    int h = tftHeight;
    int cx = w / 2;
    int cy = h / 2;
    int scale = min(w, h) / 6;

    unsigned long lastMode = millis();
    int mode = 0;
    int modeCount = 4;

    while (true) {
        if (check(AnyKeyPress)) {
            delay(50);
            break;
        }

        unsigned long elapsed = millis() - lastMode;
        if (elapsed > 10000) { // Switch mode every 10 seconds
            mode = (mode + 1) % modeCount;
            lastMode = millis();
            tft.fillScreen(PH_BLACK);
        }

        switch (mode) {
            case 0: // Matrix rain
                idleMatrixRain();
                break;
            case 1: // Fox mask pulsing
                idleFoxMask();
                break;
            case 2: // Circuit board pattern
                idleCircuitBoard();
                break;
            case 3: // Data stream
                idleDataStream();
                break;
        }
        delay(10);
    }

    tft.fillScreen(PH_BLACK);
}

void PhantomMenu::idleMatrixRain() {
    int w = tftWidth;
    int h = tftHeight;
    const char *chars = "01アイウエオ";
    int charLen = strlen(chars);

    static int dropX[30];
    static int dropY[30];
    static bool initialized = false;

    if (!initialized) {
        for (int i = 0; i < 30; i++) {
            dropX[i] = (i * 8) % w;
            dropY[i] = random(-h, 0);
        }
        initialized = true;
    }

    for (int i = 0; i < 30; i++) {
        // Erase old
        tft.drawChar(dropX[i], dropY[i], ' ', PH_BLACK, PH_BLACK, 1);

        // Move
        dropY[i] += 8;

        // Draw new
        if (dropY[i] >= 0 && dropY[i] < h) {
            char c = chars[random(0, charLen)];
            tft.drawChar(dropX[i], dropY[i], c, PH_WHITE, PH_BLACK, 1);
        }

        // Trail
        int trailY = dropY[i] - 8;
        if (trailY >= 0 && trailY < h) {
            char c = chars[random(0, charLen)];
            tft.drawChar(dropX[i], trailY, c, PH_GREEN, PH_BLACK, 1);
        }

        // Older trail
        int oldY = dropY[i] - 16;
        if (oldY >= 0 && oldY < h) {
            tft.drawChar(dropX[i], oldY, ' ', PH_BLACK, PH_BLACK, 1);
        }

        // Reset
        if (dropY[i] > h + 20) {
            dropY[i] = random(-50, -10);
        }
    }

    delay(50);
}

void PhantomMenu::idleFoxMask() {
    int w = tftWidth;
    int h = tftHeight;
    int cx = w / 2;
    int cy = h / 2;
    int scale = min(w, h) / 5;

    static int phase = 0;
    phase = (phase + 1) % 360;

    float pulse = 1.0 + 0.15 * sin(phase * 0.05);
    int s = scale * pulse;

    // Clear area
    tft.fillRect(cx - scale - 20, cy - scale - 20, (scale+20)*2, (scale+20)*2 + 20, PH_BLACK);

    uint16_t eyeColor = (phase % 60 < 30) ? PH_GREEN : PH_WHITE;

    // Eyes
    tft.fillTriangle(cx - s*0.8, cy - s*0.3, cx - s*0.2, cy - s*0.6,
                     cx - s*0.2, cy - s*0.1, eyeColor);
    tft.fillTriangle(cx + s*0.8, cy - s*0.3, cx + s*0.2, cy - s*0.6,
                     cx + s*0.2, cy - s*0.1, eyeColor);
    // Nose
    tft.fillTriangle(cx, cy + s*0.1, cx - s*0.15, cy + s*0.3,
                     cx + s*0.15, cy + s*0.3, PH_WHITE);
    // Ears
    tft.fillTriangle(cx - s*0.6, cy - s*0.5, cx - s*1.0, cy - s*1.0,
                     cx - s*0.2, cy - s*0.7, PH_WHITE);
    tft.fillTriangle(cx + s*0.6, cy - s*0.5, cx + s*1.0, cy - s*1.0,
                     cx + s*0.2, cy - s*0.7, PH_WHITE);
    // Inner ears
    tft.fillTriangle(cx - s*0.55, cy - s*0.55, cx - s*0.85, cy - s*0.9,
                     cx - s*0.3, cy - s*0.65, PH_BLACK);
    tft.fillTriangle(cx + s*0.55, cy - s*0.55, cx + s*0.85, cy - s*0.9,
                     cx + s*0.3, cy - s*0.65, PH_BLACK);
    // Mouth
    tft.drawLine(cx, cy + s*0.3, cx, cy + s*0.5, PH_WHITE);
    tft.drawLine(cx, cy + s*0.5, cx - s*0.3, cy + s*0.6, PH_WHITE);
    tft.drawLine(cx, cy + s*0.5, cx + s*0.3, cy + s*0.6, PH_WHITE);

    // Pulsing circuit lines around mask
    for (int i = 0; i < 4; i++) {
        float angle = (phase + i * 90) * 0.01745;
        int lx = cx + cos(angle) * (s * 1.3);
        int ly = cy + sin(angle) * (s * 1.3);
        int ex = lx + cos(angle) * 15;
        int ey = ly + sin(angle) * 15;
        tft.drawLine(lx, ly, ex, ey, PH_DARK_GREEN);
        tft.fillCircle(ex, ey, 2, PH_GREEN);
    }

    delay(60);
}

void PhantomMenu::idleCircuitBoard() {
    int w = tftWidth;
    int h = tftHeight;

    // Draw random circuit traces
    for (int i = 0; i < 5; i++) {
        int x1 = random(0, w);
        int y1 = random(0, h);
        int x2 = x1 + random(-60, 60);
        int y2 = y1 + random(-30, 30);

        // Horizontal then vertical (like PCB traces)
        tft.drawFastHLine(min(x1,x2), y1, abs(x2-x1), PH_DARK_GREEN);
        tft.drawFastVLine(x2, min(y1,y2), abs(y2-y1), PH_DARK_GREEN);

        // Node dot at junction
        tft.fillCircle(x2, y1, 2, PH_GREEN);

        // Random component dots
        if (random(0, 3) == 0) {
            tft.fillCircle(x2, y1, 4, PH_MUTED_GREEN);
            tft.fillCircle(x2, y1, 2, PH_GREEN);
        }
    }

    // Scanning line
    static int scanY = 0;
    tft.drawFastHLine(0, scanY, w, PH_GREEN);
    tft.drawFastHLine(0, scanY - 1, w, PH_DARK_GREEN);
    tft.drawFastHLine(0, scanY + 1, w, PH_DARK_GREEN);
    scanY = (scanY + 2) % h;

    delay(80);
}

void PhantomMenu::idleDataStream() {
    int w = tftWidth;
    int h = tftHeight;
    int centerY = h / 2;

    // Hex data scrolling horizontally
    static int offset = 0;
    char hexLine[64];
    snprintf(hexLine, sizeof(hexLine), "PHANTOM::%04X::DEDSEC::%04X::SECURE",
             random(0, 0xFFFF), random(0, 0xFFFF));

    tft.setTextSize(FP);
    tft.setTextDatum(TL_DATUM);

    // Multiple rows of hex data
    for (int row = 0; row < 8; row++) {
        int y = 20 + row * 35;
        if (y > h - 30) break;

        char line[80];
        snprintf(line, sizeof(line), "%08X  %s  %04X  %08X",
                 random(0, 0xFFFFFFFF), hexLine + (row * 3) % strlen(hexLine),
                 random(0, 0xFFFF), random(0, 0xFFFFFFFF));

        uint16_t color = (row % 3 == 0) ? PH_GREEN : PH_GRAY;
        tft.setTextColor(color, PH_BLACK);
        tft.drawString(line, 5 - (offset % 20), y, 1);
    }

    offset = (offset + 1) % 100;

    // Blinking cursor
    static bool cursorOn = false;
    cursorOn = !cursorOn;
    if (cursorOn) {
        tft.fillRect(w - 10, h / 2, 8, 12, PH_GREEN);
    }

    delay(100);
}

/*********************************************************************
**  Main menu loop
**********************************************************************/
bool PhantomMenu::waitForInput(int &selected) {
    if (check(SelPress) || check(AnyKeyPress)) {
        _lastActivity = millis();
        return true;
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

    // Check idle timeout
    if (millis() - _lastActivity > IDLE_TIMEOUT_MS) {
        idleScreen();
        _lastActivity = millis();
        drawMenu();
    }

    return false;
}

/*********************************************************************
**  begin - Main entry point
**********************************************************************/
void PhantomMenu::begin() {
    returnToMenu = false;
    _selectedIndex = 0;
    _lastActivity = millis();

    drawMenu();

    while (!returnToMenu) {
        if (waitForInput(_selectedIndex)) {
            // Selected an item - launch its submenu
            // Map index to the actual Bruce menu
            tft.fillScreen(PH_BLACK);

            // Glitch transition
            for (int i = 0; i < 5; i++) {
                int ox = random(-4, 4);
                int oy = random(-3, 3);
                tft.setTextColor(PH_GREEN, PH_BLACK);
                tft.setTextDatum(MC_DATUM);
                tft.drawString(_items[_selectedIndex].label, tftWidth/2 + ox, tftHeight/2 + oy, 1);
                delay(30);
            }
            tft.fillScreen(PH_BLACK);

            // Call the Bruce submenu
            // We need to call through the mainMenu object
            // but we can't include main_menu.h here to avoid circular deps
            // So we use a function pointer set by main.cpp
            extern std::function<void(int)> phantomMenuCallback;
            if (phantomMenuCallback) {
                phantomMenuCallback(_selectedIndex);
            }

            // After submenu returns, redraw menu
            _lastActivity = millis();
            drawMenu();
        }

        delay(10);
    }
}
