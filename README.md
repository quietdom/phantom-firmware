# Phantom

ESP32-S3 offensive security platform for T-Embed CC1101.

Built from scratch for hardware-level penetration testing. Sub-GHz, WiFi, BLE, IR, RFID, GPS.

![Watch Dogs Theme](./src/themes/watchdogs/boot.gif)

## Installation

**Web Flasher** - Coming soon

**Manual:**
```
esptool.py --chip esp32s3 --port /dev/ttyACM0 write_flash 0x0 firmware.bin
```

**Build:**
```
git clone https://github.com/quietdom/phantom-firmware.git
cd phantom-firmware
platformio run -e lilygo-t-embed-cc1101
```

## Modules

| Module | Tools |
|--------|-------|
| **WiFi** | Scanner, AP, Beacon Spam, Deauth, Evil Portal, Sniffer, ARP Spoof, Responder, SSH, WireGuard, Karma |
| **BLE** | Scanner, Spam (iOS/Windows/Samsung), Bad BLE, AirTag Spoof, Name Spammer |
| **Sub-GHz** | Scan/Copy, Replay, Spectrum, Jammer, Bruteforce |
| **RFID** | PN532, 125kHz, NDEF, Chameleon, Amiibolink, EMV |
| **IR** | TV-B-Gone, Receiver, Custom (NEC/Samsung/RC5/RC6/Sony) |
| **NRF24** | Jammer, 2.4G Spectrum |
| **GPS** | Tracker, Wardriving, Wigle |
| **Other** | JS Interpreter, Mic, QR Codes, SD Manager, WebUI, iButton |

## Hardware

**Board:** Lilygo T-Embed CC1101

| Peripheral | Support |
|------------|---------|
| CC1101 Sub-GHz | Yes |
| NRF24 | Yes |
| FM Radio | Yes |
| PN532 NFC | Yes |
| Microphone | Yes |
| BadUSB | Yes |
| RGB LED | Yes |
| Speaker | Yes |
| Battery Gauge | Yes |

## License

MIT. Authorized security testing only.
