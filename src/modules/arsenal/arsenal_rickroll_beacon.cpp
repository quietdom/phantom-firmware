#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <globals.h>
#include "arsenal.h"
#include "core/display.h"
#include "core/mykeyboard.h"

static const char *RICKROLL[] = {
    "NeverGonnaGiveYouUp", "NeverGonnaLetYouDown", "NeverGonnaRunAround",
    "NeverGonnaMakeYouCry", "NeverGonnaSayGoodbye", "NeverGonnaTellALie",
    "NeverGonnaDesertYou", "RickRollDeployed!", "DEDSEC_RICKROLL",
    "0x4C:0x00:0x12", "HackThePlanet", "PhantomNet",
};
static const int RICK_COUNT = sizeof(RICKROLL) / sizeof(RICKROLL[0]);

static void randomMAC(uint8_t *mac) {
    for (int i = 0; i < 6; i++) mac[i] = random(256);
    mac[0] |= 0x02;
    mac[0] &= 0xFE;
}

static void sendBeacon(const char *ssid, uint8_t channel) {
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    
    uint8_t beacon[] = {
        0x80, 0x00,
        0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x64, 0x00,
        0x11, 0x00,
        0x00, 0x07,
    };
    
    uint8_t mac[6];
    randomMAC(mac);
    memcpy(beacon + 10, mac, 6);
    memcpy(beacon + 16, mac, 6);
    
    int ssidLen = strlen(ssid);
    uint8_t ssidTag[2 + 32];
    ssidTag[0] = 0x00;
    ssidTag[1] = ssidLen;
    memcpy(ssidTag + 2, ssid, ssidLen);
    
    uint8_t frame[200];
    int pos = 0;
    memcpy(frame, beacon, sizeof(beacon));
    pos = sizeof(beacon);
    memcpy(frame + pos, ssidTag, 2 + ssidLen);
    pos += 2 + ssidLen;
    
    uint8_t rates[] = {0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x24, 0x30, 0x48, 0x6C};
    memcpy(frame + pos, rates, sizeof(rates));
    pos += sizeof(rates);
    
    esp_wifi_80211_tx(WIFI_IF_STA, frame, pos, false);
}

void arsenal_rickroll_beacon(void) {
    ARSENAL_HEAP_CHECK();
    WiFi.mode(WIFI_STA);
    esp_wifi_set_promiscuous(true);
    
    int sent = 0;
    int channel = 1;
    unsigned long startTime = millis();
    
    while (true) {
        for (int i = 0; i < 10; i++) {
            sendBeacon(RICKROLL[sent % RICK_COUNT], channel);
            sent++;
        }
        channel = (channel % 14) + 1;
        
        if (sent % 50 == 0) {
            drawMainBorderWithTitle("Rickroll Beacon");
            int y = 45;
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.setTextSize(FP);
            tft.setCursor(12, y);
            tft.print("NEVER GONNA GIVE YOU UP");
            y += 16;
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setCursor(12, y);
            tft.printf("Beacons: %d", sent);
            y += 14;
            tft.setCursor(12, y);
            unsigned long elapsed = (millis() - startTime) / 1000;
            tft.printf("Rate: ~%d/s", elapsed > 0 ? sent / elapsed : 0);
            y += 14;
            tft.setCursor(12, y);
            tft.printf("Ch: %d | %s", channel, RICKROLL[sent % RICK_COUNT]);
            tft.setTextColor(TFT_YELLOW, TFT_BLACK);
            tft.drawCentreString("Esc:stop", tftWidth / 2, tftHeight - 20, 1);
        }
        
        if (check(EscPress)) { returnToMenu = true; break; }
        esp_task_wdt_reset();
        delay(20);
    }
    
    esp_wifi_set_promiscuous(false);
}