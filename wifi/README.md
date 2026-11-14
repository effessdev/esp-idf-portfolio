# WiFi

There are two modes. For example:

- **STA (station) mode**: our phone when we turn on WiFi to connect to our home router
- **AP (access point) mode**: our phone when we turn on Hotspot

## What this project uses

We use **STA (Station) mode** in this project. It is the most important mode since most times your ESP32 needs to connect to the internet.

## Steps to implement WiFi Station

### Initialize NVS Flash.

When the Wi-Fi driver starts up, it reads calibration data (like RF calibration values and MAC addresses) from NVS. If NVS hasn't been initialized and mounted in flash memory first, the Wi-Fi driver will fail to start.
