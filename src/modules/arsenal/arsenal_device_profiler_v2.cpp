#if !LITE_VERSION
#include <Arduino.h>
#include <WiFi.h>
#include <NimBLEDevice.h>
#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <globals.h>
#include "arsenal.h"
#include "core/display.h"
#include "core/mykeyboard.h"

struct ProfiledDevice {
    String name;
    String address;
    String type;
    int rssi;
    int threatLevel;
    unsigned long firstSeen;
};

static ProfiledDevice devices[32];
static int deviceCount = 0;

static String classifyDevice(const NimBLEAdvertisedDevice *dev) {
    if (dev->haveManufacturerData()) {
        std::string mfg = dev->getManufacturerData();
        if (mfg.length() >= 2) {
            uint16_t company = (uint8_t)mfg[0] | ((uint8_t)mfg[1] << 8);
            switch (company) {
                case 0x004C: return "Apple";
                case 0x0006: return "Microsoft";
                case 0x000F: return "Broadcom";
                case 0x004D: return "Google";
                case 0x0310: return "Samsung";
                case 0x0157: return "Xiaomi";
                case 0x02E0: return "Qualcomm";
                case 0x0059: return "Nordic";
            }
        }
    }
    if (dev->haveServiceUUID()) {
        NimBLEUUID uuid = dev->getServiceUUID();
        if (uuid == NimBLEUUID("0000fee0")) return "Fitness";
        if (uuid == NimBLEUUID("0000180f")) return "Battery";
        if (uuid == NimBLEUUID("0000180a")) return "DeviceInfo";
        if (uuid == NimBLEUUID("0000fe95")) return "Xiaomi";
    }
    return "Unknown";
}

static int assessThreat(const NimBLEAdvertisedDevice *dev) {
    int threat = 0;
    if (dev->haveManufacturerData()) {
        std::string mfg = dev->getManufacturerData();
        if (mfg.length() >= 2 && (uint8_t)mfg[0] == 0x4C && (uint8_t)mfg[1] == 0x00) {
            if (mfg.length() > 2 && (uint8_t)mfg[2] == 0x12) threat += 3;
        }
    }
    if (dev->haveName()) {
        String name = dev->getName().c_str();
        name.toLowerCase();
        if (name.indexOf("hc-0") >= 0 || name.indexOf("jdy") >= 0 || name.indexOf("hm-10") >= 0) threat += 5;
        if (name.indexOf("flipper") >= 0) threat += 2;
        if (name.indexOf("rubber") >= 0 || name.indexOf("ducky") >= 0) threat += 4;
    }
    if (dev->getRSSI() > -40) threat += 1;
    return min(threat, 10);
}

class ProfilerCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice *dev) {
        String addr = dev->getAddress().toString().c_str();
        for (int i = 0; i < deviceCount; i++) {
            if (devices[i].address == addr) {
                devices[i].rssi = dev->getRSSI();
                return;
            }
        }
        if (deviceCount < 32) {
            devices[deviceCount].name = dev->haveName() ? dev->getName().c_str() : "?";
            devices[deviceCount].address = addr;
            devices[deviceCount].type = classifyDevice(dev);
            devices[deviceCount].rssi = dev->getRSSI();
            devices[deviceCount].threatLevel = assessThreat(dev);
            devices[deviceCount].firstSeen = millis();
            deviceCount++;
        }
    }
};

void arsenal_device_profiler(void) {
    NimBLEDevice::deinit(true);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    if (!NimBLEDevice::init("")) {
        displayError("BLE init failed", true);
        return;
    }

    NimBLEScan *scan = NimBLEDevice::getScan();
    ProfilerCallbacks cb;
    scan->setScanCallbacks(&cb);
    scan->setActiveScan(true);
    scan->setInterval(100);
    scan->setWindow(99);

    deviceCount = 0;

    while (true) {
        scan->start(3, false);
        scan->clearResults();

        drawMainBorderWithTitle("Profiler");
        int y = 32;
        int padX = 6;

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(FP);
        tft.setCursor(padX, y);
        tft.printf("Devices: %d", deviceCount);
        y += 14;

        int maxShow = min(deviceCount, 8);
        for (int i = 0; i < maxShow; i++) {
            uint16_t color = devices[i].threatLevel > 3 ? TFT_RED :
                             devices[i].threatLevel > 0 ? TFT_YELLOW : TFT_GREEN;
            tft.setTextColor(color, TFT_BLACK);
            tft.setCursor(padX, y);
            String line = devices[i].type.substring(0, 6) + " " + String(devices[i].rssi) + "dB";
            if (devices[i].threatLevel > 0) line += " !" + String(devices[i].threatLevel);
            tft.print(line.substring(0, 28));
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