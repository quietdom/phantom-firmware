#ifndef __WATCHDOGS_BOOT_H__
#define __WATCHDOGS_BOOT_H__

#include <Arduino.h>
#include <TFT_eSPI.h>

class WatchdogsBoot {
public:
    static void run(TFT_eSPI &tft, int tftWidth, int tftHeight) {
        tft.fillScreen(TFT_BLACK);
        
        // Phase 1: Scan lines effect
        drawScanlines(tft, tftWidth, tftHeight);
        delay(300);
        
        // Phase 2: Glitch text
        drawGlitchText(tft, tftWidth, tftHeight, "PHANTOM");
        delay(400);
        
        // Phase 3: Fox mask animation
        drawFoxMask(tft, tftWidth, tftHeight);
        delay(600);
        
        // Phase 4: Status text
        drawStatusText(tft, tftWidth, tftHeight);
        delay(800);
        
        // Phase 5: Fade to black
        fadeToBlack(tft, tftWidth, tftHeight);
    }

private:
    static void drawScanlines(TFT_eSPI &tft, int w, int h) {
        for (int y = 0; y < h; y += 3) {
            tft.drawFastHLine(0, y, w, tft.color565(0, 20, 0));
        }
    }
    
    static void drawGlitchText(TFT_eSPI &tft, int w, int h, const char *text) {
        tft.setTextSize(3);
        tft.setTextDatum(MC_DATUM);
        
        for (int i = 0; i < 8; i++) {
            int offsetX = random(-4, 4);
            int offsetY = random(-2, 2);
            uint16_t color = (i % 2 == 0) ? TFT_WHITE : tft.color565(0, 255, 0);
            tft.setTextColor(color, TFT_BLACK);
            tft.drawString(text, w/2 + offsetX, h/2 - 20 + offsetY, 1);
            delay(50);
        }
        
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(text, w/2, h/2 - 20, 1);
    }
    
    static void drawFoxMask(TFT_eSPI &tft, int w, int h) {
        int cx = w / 2;
        int cy = h / 2 + 10;
        int scale = min(w, h) / 6;
        
        uint16_t fg = TFT_WHITE;
        uint16_t bg = TFT_BLACK;
        
        // Eyes - angular fox eyes
        tft.fillTriangle(cx - scale*0.8, cy - scale*0.3, cx - scale*0.2, cy - scale*0.6, cx - scale*0.2, cy - scale*0.1, fg);
        tft.fillTriangle(cx + scale*0.8, cy - scale*0.3, cx + scale*0.2, cy - scale*0.6, cx + scale*0.2, cy - scale*0.1, fg);
        
        // Nose
        tft.fillTriangle(cx, cy + scale*0.1, cx - scale*0.15, cy + scale*0.3, cx + scale*0.15, cy + scale*0.3, fg);
        
        // Ears - pointed triangles
        tft.fillTriangle(cx - scale*0.6, cy - scale*0.5, cx - scale*1.0, cy - scale*1.0, cx - scale*0.2, cy - scale*0.7, fg);
        tft.fillTriangle(cx + scale*0.6, cy - scale*0.5, cx + scale*1.0, cy - scale*1.0, cx + scale*0.2, cy - scale*0.7, fg);
        
        // Inner ears (black)
        tft.fillTriangle(cx - scale*0.55, cy - scale*0.55, cx - scale*0.85, cy - scale*0.9, cx - scale*0.3, cy - scale*0.65, bg);
        tft.fillTriangle(cx + scale*0.55, cy - scale*0.55, cx + scale*0.85, cy - scale*0.9, cx + scale*0.3, cy - scale*0.65, bg);
        
        // Mouth line
        tft.drawLine(cx, cy + scale*0.3, cx, cy + scale*0.5, fg);
        tft.drawLine(cx, cy + scale*0.5, cx - scale*0.3, cy + scale*0.6, fg);
        tft.drawLine(cx, cy + scale*0.5, cx + scale*0.3, cy + scale*0.6, fg);
        
        // Circuit lines extending from mask
        for (int i = 0; i < 6; i++) {
            int startX = cx + (random(0, 2) ? 1 : -1) * scale * (0.5 + random(0, 50) / 100.0);
            int startY = cy + random(-scale, scale) * 0.5;
            int endX = startX + (random(0, 2) ? 1 : -1) * scale * (0.3 + random(0, 50) / 100.0);
            int endY = startY + random(-scale/2, scale/2);
            tft.drawLine(startX, startY, endX, endY, tft.color565(0, 80, 0));
            tft.fillCircle(endX, endY, 2, tft.color565(0, 120, 0));
        }
    }
    
    static void drawStatusText(TFT_eSPI &tft, int w, int h) {
        tft.setTextSize(1);
        tft.setTextDatum(MC_DATUM);
        
        const char *lines[] = {
            "DEDSEC NETWORK",
            "OS: PHANTOM v1.0",
            "STATUS: OPERATIONAL",
            ">>> ACCESS GRANTED <<<"
        };
        
        int y = h/2 + 50;
        for (int i = 0; i < 4; i++) {
            tft.setTextColor(tft.color565(0, 200, 0), TFT_BLACK);
            tft.drawString(lines[i], w/2, y, 1);
            y += 14;
            delay(150);
        }
    }
    
    static void fadeToBlack(TFT_eSPI &tft, int w, int h) {
        for (int brightness = 255; brightness >= 0; brightness -= 15) {
            uint16_t color = tft.color565(brightness/8, brightness/4, brightness/8);
            tft.drawRect(0, 0, w, h, color);
            delay(20);
        }
        tft.fillScreen(TFT_BLACK);
    }
};

#endif