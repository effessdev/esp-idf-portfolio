# WiFi

There are two modes. For example:

- **STA (station) mode**: our phone when we turn on WiFi to connect to our home router
- **AP (access point) mode**: our phone when we turn on Hotspot

## What this project uses

We use **STA (Station) mode** in this project. It is the most important mode since most times your ESP32 needs to connect to the internet.

## Before building

Set Wi-Fi SSID and password using Menuconfig.

## Steps to implement WiFi Station

### Step 1: Initialize NVS Flash.

When the Wi-Fi driver starts up, it reads calibration data (like RF calibration values and MAC addresses) from NVS. If NVS hasn't been initialized and mounted in flash memory first, the Wi-Fi driver will fail to start.

### Next steps

Please refer [main.c](/main/main.c).

## `.threshold.authmode = WIFI_AUTH_WPA2_PSK`

This is the most common authentication security mode (even though newer standards like WPA3 exist).

- WPA2 (Wi-Fi Protected Access 2): It means that we encrypt the data sent over the air so no one can eavesdrop on your traffic.

- PSK (Pre-Shared Key): This means it uses a standard password (the Wi-Fi passphrase you type in) that is shared between the router and your device to authenticate the connection.
