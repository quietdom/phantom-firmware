<div align="center">

# Phantom

ESP32-S3 offensive security platform for T-Embed CC1101.

![Watch Dogs Theme](./src/themes/watchdogs/boot.gif)

## Install

**Web Flasher** - Coming soon

**Manual:**
```
esptool.py --chip esp32s3 --port /dev/ttyACM0 write_flash 0x0 firmware.bin
```

## Build

```
git clone https://github.com/quietdom/phantom-firmware.git
cd phantom-firmware
platformio run -e lilygo-t-embed-cc1101
```

## License

MIT

</div>
