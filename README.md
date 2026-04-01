# Vincent

An ESP32-based vinyl record scanner that looks up barcodes in the [Discogs](https://www.discogs.com/) database and displays album information on a 3.97" e-ink screen. Still a work in progress.

## Features

- Scan vinyl record barcodes and search the Discogs database in real time
- Display album title, year, country, label, and disc count on a Waveshare 3.97" e-ink display (800x480, 4-color)
- Automatically add found records to your Discogs collection
- Secure HTTPS communication with the Discogs API
- Low-power design with deep sleep support

## Hardware

- **MCU:** ESP32 (dual-core Xtensa)
- **Display:** Waveshare 3.97" e-ink (SPI)
- **Barcode scanner:** UART-connected

### E-ink Display Wiring

| Signal | GPIO |
|--------|------|
| SCK    | 13   |
| MOSI   | 14   |
| CS     | 15   |
| RST    | 26   |
| DC     | 27   |
| BUSY   | 25   |
| PWR    | 33   |