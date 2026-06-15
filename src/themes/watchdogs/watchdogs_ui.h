#ifndef __WATCHDOGS_UI_H__
#define __WATCHDOGS_UI_H__

#include <Arduino.h>
#include <TFT_eSPI.h>

class WatchdogsUI {
public:
    static void drawAnimatedBorder(TFT_eSPI &tft, int w, int h, uint16_t color) {
        static int phase = 0;
        phase = (phase + 1) % 4;
        
        tft.drawRect(0, 0, w, h, color);
        
        // Corner brackets
        int cornerLen = 12;
        tft.drawFastHLine(2, 2, cornerLen, color);
        tft.drawFastVLine(2, 2, cornerLen, color);
        
        tft.drawFastHLine(w - cornerLen - 2, 2, cornerLen, color);
        tft.drawFastVLine(w - 3, 2, cornerLen, color);
        
        tft.drawFastHLine(2, h - 3, cornerLen, color);
        tft.drawFastVLine(2, h - cornerLen - 2, cornerLen, color);
        
        tft.drawFastHLine(w - cornerLen - 2, h - 3, cornerLen, color);
        tft.drawFastVLine(w - 3, h - cornerLen - 2, cornerLen, color);
        
        // Animated dashes along edges
        uint16_t dimColor = tft.color565(0, 40, 0);
        for (int i = cornerLen + 6; i < w - cornerLen - 6; i += 8) {
            tft.drawFastHLine(i, 1, 4, (i/8 + phase) % 2 ? color : dimColor);
            tft.drawFastHLine(i, h - 2, 4, (i/8 + phase) % 2 ? color : dimColor);
        }
        for (int i = cornerLen + 6; i < h - cornerLen - 6; i += 8) {
            tft.drawFastVLine(1, i, 4, (i/8 + phase) % 2 ? color : dimColor);
            tft.drawFastVLine(w - 2, i, 4, (i/8 + phase) % 2 ? color : dimColor);
        }
    }
    
    static void drawDataFlow(TFT_eSPI &tft, int x, int y, int width, uint16_t color) {
        static int offset = 0;
        offset = (offset + 1) % 16;
        
        for (int i = 0; i < width; i += 16) {
            int pos = (i + offset) % width;
            tft.drawFastHLine(x + pos, y, 6, color);
        }
    }
    
    static void drawGlitchBar(TFT_eSPI &tft, int y, int w, uint16_t color) {
        int glitchX = random(0, w);
        int glitchW = random(10, 60);
        tft.drawFastHLine(glitchX, y, glitchW, color);
        if (random(0, 3) == 0) {
            tft.drawFastHLine(glitchX + random(-5, 5), y + 1, glitchW / 2, color);
        }
    }
    
    static void drawCircuitPattern(TFT_eSPI &tft, int cx, int cy, int size, uint16_t color) {
        // Hub
        tft.fillCircle(cx, cy, 3, color);
        
        // Radiating lines with nodes
        for (int angle = 0; angle < 360; angle += 45) {
            float rad = angle * 3.14159 / 180;
            int endX = cx + cos(rad) * size;
            int endY = cy + sin(rad) * size;
            tft.drawLine(cx, cy, endX, endY, tft.color565(0, 60, 0));
            tft.fillCircle(endX, endY, 2, color);
            
            // Sub-branches
            float subRad = (angle + 30) * 3.14159 / 180;
            int subX = endX + cos(subRad) * size * 0.4;
            int subY = endY + sin(subRad) * size * 0.4;
            tft.drawLine(endX, endY, subX, subY, tft.color565(0, 40, 0));
            tft.fillCircle(subX, subY, 1, tft.color565(0, 100, 0));
        }
    }
    
    static void drawProgressBar(TFT_eSPI &tft, int x, int y, int w, int h, float progress, uint16_t fg, uint16_t bg) {
        tft.drawRect(x, y, w, h, fg);
        int fillW = (w - 2) * progress;
        tft.fillRect(x + 1, y + 1, fillW, h - 2, fg);
        if (fillW < w - 2) {
            tft.fillRect(x + 1 + fillW, y + 1, w - 2 - fillW, h - 2, bg);
        }
    }
    
    static void drawScanEffect(TFT_eSPI &tft, int w, int h, int &scanY, uint16_t color) {
        tft.drawFastHLine(0, scanY, w, color);
        scanY = (scanY + 2) % h;
        tft.drawFastHLine(0, scanY, w, tft.color565(0, 40, 0));
    }
};

#endif