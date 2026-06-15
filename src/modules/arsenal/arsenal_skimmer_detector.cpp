#if !LITE_VERSION
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include <globals.h>
#include "arsenal.h"
#include "core/display.h"
#include "core/mykeyboard.h"

static const char *SUSPICIOUS_NAMES[] = {
    "HC-03", "HC-05", "HC-06", "HC-08", "BT-HC05",
    "JDY-31", "AT-09", "HM-10", "CC41-A", "CC2541",
    "MLT-BT05", "SPP-CA", "FFD0", "BLED112",
    "Nordic_UART", "RNBT", "RFComm",
};
static const int SUSPICIOUS_COUNT = sizeof(SUSPICIOUS_NAMES) / sizeof(SUSPICIOUS_NAMES[0]);

struct SkimmerAlert {
    String name;
    String address;
    int rssi;
    unsigned long time;
};

static SkimmerAlert alerts[16];
static int alertCount = 0;

class SkimmerCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice *dev) {
        if (!dev->haveName()) return;
        String name = dev->getName().c_str();
        
        for (int i = 0; i < SUSPICIOUS_COUNT; i++) {
            if (name.equalsIgnoreCase(SUSPICIOUS_NAMES[i])) {
                if (alertCount < 16) {
                    alerts[alertCount].name = name;
                    alerts[alertCount].address = dev->getAddress().toString().c_str();
                    alerts[alertCount].rssi = dev->getRSSI();
                    alerts[alertCount].time = millis();
                    alertCount++;
                }
                return;
            }
        }
    }
};

void arsenal_skimmer_detector(void) {
    NimBLEDevice::deinit(true);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    if (!NimBLEDevice::init("")) {
        displayError("BLE init failed", true);
        return;
    }
    
    NimBLEScan *scan = NimBLEDevice::getScan();
    SkimmerCallbacks cb;
    scan->setScanCallbacks(&cb);
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);
    
    alertCount = 0;
    
    while (true) {
        scan->start(3, false);
        scan->clearResults();
        
        drawMainBorderWithTitle("Skimmer Detect");
        int y = 32;
        int padX = 6;
        
        if (alertCount > 0) {
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.setTextSize(FP);
            tft.setCursor(padX, y);
            tft.print("!! THREAT DETECTED !!");
            y += 16;
        } else {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextSize(FP);
            tft.setCursor(padX, y);
            tft.print("Scanning for skimmers...");
            y += 16;
        }
        
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(padX, y);
        tft.printf("Alerts: %d", alertCount);
        y += 14;
        
        int maxShow = min(alertCount, 5);
        for (int i = alertCount - maxShow; i < alertCount; i++) {
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.setCursor(padX, y);
            tft.print(alerts[i].name + " " + String(alerts[i].rssi) + "dB");
            y += 11;
        }
        
        tft.setTextColor(tft.color565(0, 150, 0), TFT_BLACK);
        tft.drawCentreString("Esc to stop", tftWidth / 2, tftHeight - 16, 1);
        
        if (check(EscPress)) { returnToMenu = true; break; }
        esp_task_wdt_reset();
        delay(100);
    }
    
    scan->stop();
    NimBLEDevice::init("");
    vTaskDelay(100 / portTICK_PERIOD_MS);
    scan->setScanCallbacks(nullptr);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    #if defined(CONFIG_IDF_TARGET_ESP32C5)
    esp_bt_controller_deinit();
    #else
    NimBLEDevice::deinit();
    #endif
}
#endif