# Phantom

Advanced ESP32-S3 offensive security platform built for the Lilygo T-Embed CC1101. Sub-GHz, WiFi, BLE, IR, RFID, GPS. Designed for hardware-level penetration testing.

![Watch Dogs Theme](./src/themes/watchdogs/boot.gif)

## Features

### WiFi
- WiFi Scanner & Connect
- WiFi AP Mode
- Beacon Spam
- Deauth Flood
- Target Attack (Deauth + Evil Portal)
- Evil Portal (Captive Portal)
- RAW Sniffer
- ARP Spoofing & Poisoning
- Responder
- TCP Client & Listener
- SSH & TelNet
- Scan Hosts with Port Scanning
- WireGuard Tunneling
- Karma Attack
- Credential Forwarding

### BLE
- BLE Scanner
- BLE Spam (iOS, Windows, Samsung, Android)
- Bad BLE (Ducky Scripts)
- AirTag Spoofer
- Notification Spoofer
- BLE Name Spammer
- BLE Audio Rickroll

### Sub-GHz (CC1101)
- Scan & Copy
- Custom Sub-GHz (Replay)
- RF Spectrum
- RF Jammer (Full & Intermittent)
- RF Bruteforce

### RFID
- PN532 Read/Write/Clone
- 125kHz Read
- NDEF Write
- Chameleon Ultra
- Amiibolink
- EMV Reader

### IR
- TV-B-Gone
- IR Receiver
- Custom IR (NEC, Samsung, RC5, RC6, Sony, etc.)

### NRF24
- NRF24 Jammer
- 2.4G Spectrum

### GPS
- GPS Tracker
- Wardriving
- Wigle Upload

### Others
- JavaScript Interpreter
- Mic Spectrum
- QR Code Generator
- SD Card Manager
- LittleFS Manager
- WebUI
- iButton
- LED Control

## Installation

### Web Flasher
Coming soon.

### Manual Flash
```bash
esptool.py --chip esp32s3 --port /dev/ttyACM0 write_flash 0x0 firmware.bin
```

### Build from Source
```bash
git clone https://github.com/quietdom/phantom-firmware.git
cd phantom-firmware
platformio run -e lilygo-t-embed-cc1101
```

## Hardware

**Supported Device:** Lilygo T-Embed CC1101

| Feature | Status |
|---------|--------|
| CC1101 Sub-GHz | Yes |
| NRF24 | Yes |
| FM Radio | Yes |
| PN532 | Yes |
| Microphone | Yes |
| BadUSB | Yes |
| RGB LED | Yes |
| Speaker | Yes |
| Battery Gauge | Yes |

## License

MIT License - Use at your own risk. For authorized security testing only.
