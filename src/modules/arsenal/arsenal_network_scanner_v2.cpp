#if !LITE_VERSION
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <globals.h>
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/wifi/wifi_common.h"

struct ScannedHost {
    IPAddress ip;
    String mac;
    String vendor;
    String services;
    bool alive;
};

static ScannedHost hosts[64];
static int hostCount = 0;

static void pingSweep() {
    hostCount = 0;
    IPAddress subnet = WiFi.localIP();
    subnet[3] = 0;
    
    for (int i = 1; i < 255 && hostCount < 64; i++) {
        IPAddress target = subnet;
        target[3] = i;
        
        WiFiClient client;
        client.setTimeout(50);
        
        bool alive = false;
        for (int port : {80, 443, 22, 21, 23, 8080, 3389, 445, 139, 53}) {
            if (client.connect(target, port)) {
                alive = true;
                client.stop();
                break;
            }
        }
        
        if (alive) {
            hosts[hostCount].ip = target;
            hosts[hostCount].alive = true;
            hosts[hostCount].services = "";
            hostCount++;
        }
        
        if (i % 20 == 0) {
            drawMainBorderWithTitle("Scanning...");
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextSize(FP);
            tft.setCursor(12, 50);
            tft.printf("Scanning: %d/255", i);
            tft.setCursor(12, 66);
            tft.printf("Hosts found: %d", hostCount);
            tft.drawCentreString("Esc to stop", tftWidth / 2, tftHeight - 20, 1);
            if (check(EscPress)) { returnToMenu = true; return; }
            esp_task_wdt_reset();
        }
    }
}

void arsenal_network_scanner_v2(void) {
    if (WiFi.status() != WL_CONNECTED && WiFi.getMode() == WIFI_MODE_NULL) {
        displayRedStripe("Connect WiFi first!");
        delay(1500);
        return;
    }
    
    if (WiFi.getMode() == WIFI_MODE_NULL) WiFi.mode(WIFI_STA);
    
    pingSweep();
    if (returnToMenu) return;
    
    int selected = 0;
    while (true) {
        drawMainBorderWithTitle("Network Scan");
        int y = 32;
        int padX = 6;
        
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(FP);
        tft.setCursor(padX, y);
        tft.printf("Hosts: %d", hostCount);
        y += 14;
        
        int maxShow = min(hostCount, 9);
        int startIdx = max(0, selected - 4);
        for (int i = startIdx; i < startIdx + maxShow && i < hostCount; i++) {
            bool isSel = (i == selected);
            uint16_t bg = isSel ? tft.color565(30, 30, 50) : TFT_BLACK;
            uint16_t fg = isSel ? TFT_GREEN : TFT_WHITE;
            
            tft.fillRect(0, y - 1, tftWidth, 12, bg);
            tft.setTextColor(fg, bg);
            tft.setCursor(padX, y);
            tft.print(hosts[i].ip.toString());
            y += 12;
        }
        
        tft.setTextColor(tft.color565(0, 150, 0), TFT_BLACK);
        tft.drawCentreString("Sel:details Up/Dn:move", tftWidth / 2, tftHeight - 16, 1);
        
        if (check(UpPress) || check(PrevPress)) { selected = max(0, selected - 1); }
        if (check(DownPress) || check(NextPress)) { selected = min(hostCount - 1, selected + 1); }
        if (check(EscPress)) { returnToMenu = true; break; }
        if (check(SelPress)) {
            drawMainBorderWithTitle("Host Details");
            y = 40;
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextSize(FP);
            tft.setCursor(12, y);
            tft.print("IP: " + hosts[selected].ip.toString());
            y += 16;
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setCursor(12, y);
            tft.print("Open ports:");
            y += 14;
            
            WiFiClient client;
            for (int port : {22, 23, 25, 53, 80, 110, 135, 139, 143, 443, 445, 993, 995, 1433, 1521, 3306, 3389, 5432, 5900, 8080, 8443}) {
                client.setTimeout(50);
                if (client.connect(hosts[selected].ip, port)) {
                    client.stop();
                    tft.setCursor(12 + (y % 2 == 0 ? 0 : tftWidth/2), y);
                    tft.printf(":%d", port);
                    y += 11;
                    if (y > tftHeight - 30) break;
                }
            }
            
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.drawCentreString("Press any key", tftWidth / 2, tftHeight - 20, 1);
            while (!check(EscPress) && !check(SelPress)) delay(100);
        }
        esp_task_wdt_reset();
        delay(50);
    }
}
#endif