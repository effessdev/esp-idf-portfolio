# MQTT

Before you can use it, first set up Wi-Fi.

## WiFi

There are two modes. For example:

- **STA (station) mode**: our phone when we turn on WiFi to connect to our home router
- **AP (access point) mode**: our phone when we turn on Hotspot

### What this project uses

We use **STA (Station) mode** in this project. It is the most important mode since most times your ESP32 needs to connect to the internet.

### Before building

Set Wi-Fi SSID and password using Menuconfig.

### Steps to implement WiFi Station

#### Step 1: Initialize NVS Flash.

When the Wi-Fi driver starts up, it reads calibration data (like RF calibration values and MAC addresses) from NVS. If NVS hasn't been initialized and mounted in flash memory first, the Wi-Fi driver will fail to start.

#### Next steps

Please refer [main.c](/main/main.c).

### Networking Stack Layers (in architectural order)

#### 1. Wi-Fi Driver (esp_wifi)

The lowest layer — handles raw radio communication: scanning, associating with an AP, encryption, and sending/receiving 802.11 frames. It has no concept of IP addresses; it just moves raw packets in and out.

> **What are 802.11 frames?**
>
> 802.11 frames are the basic units of data defined by the IEEE 802.11 standard — the standard that defines Wi-Fi. Just like Ethernet has “Ethernet frames,” Wi-Fi has its own frame format for anything transmitted over the air.

> **Why doesn't the Wi-Fi driver need any concept of IP addresses?**
>
> Every Wi-Fi radio (your ESP32, your laptop, the AP) has a MAC address. The Wi-Fi driver only needs to identify which AP to associate with (AP has a MAC address called the BSSID), address frames to a specific device ("this frame is for MAC address X"), let the AP know which device sent the frame.

#### 2. Network Interface Abstraction (esp_netif)

The glue layer — sits between the driver and the TCP/IP stack. It registers I/O functions so the driver can hand packets up and receive packets down, and exposes a uniform API (get IP, set DNS, start DHCP, etc.) that works the same way across Wi-Fi, Ethernet, or PPP.

#### 3. TCP/IP Stack (lwIP)

The top layer — handles IP addressing, DHCP, DNS, and routing. It’s what actually turns raw frames into a usable network connection with an IP address.

> **Why can't lwIP also use MAC addresses to send data? Why create something called an IP?**
>
> A MAC address like `AA:BB:CC:11:22:33` tells you nothing about where that device physically is. It’s just a flat, random-ish identifier. There’s no way to look at a MAC address and know “this device is somewhere in Mumbai” vs. “this device is in Ohio.”
>
> IP addresses, by contrast, are hierarchical — 142.250.X.X can be grouped and routed toward Google’s network, 192.168.X.X is “local network,” etc. This hierarchy is what lets routers make efficient decisions (“send this toward that direction”) without knowing about every device on Earth.

### MAC address vs IP: A mental model

Think of mailing a package across the country via multiple trucking companies:

- **MAC address** = the label on the truck saying “take this to the next depot” — rewritten at every depot
- **IP address** = the shipping label inside the box, written once, saying “ultimate destination: 123 Main St” — never changes, regardless of how many trucks it passes through

That’s exactly why `esp_netif/lwIP` exist: your Wi-Fi driver’s MAC addressing is only good for the first hop — getting a frame from your ESP32 to your router. Everything beyond that (reaching a server across the internet) requires the persistent, hierarchical IP addressing that only lwIP understands and manages.

### `.threshold.authmode = WIFI_AUTH_WPA2_PSK`

This is the most common authentication security mode (even though newer standards like WPA3 exist).

- WPA2 (Wi-Fi Protected Access 2): It means that we encrypt the data sent over the air so no one can eavesdrop on your traffic.

- PSK (Pre-Shared Key): This means it uses a standard password (the Wi-Fi passphrase you type in) that is shared between the router and your device to authenticate the connection.

## MQTT

### QoS (Quality of Service levels)

- QoS 0 (At most once): "Fire and forget." The message is sent once with no acknowledgment. If the network drops, the message is lost.

- QoS 1 (At least once): Guaranteed delivery. The sender retransmits the message until it receives an acknowledgment (PUBACK). This can result in duplicate messages.

- QoS 2 (Exactly once): Slowest and safest. Uses a four-step handshake to ensure the message arrives one and only one time without duplicates.

### Retain flag

The retain flag tells the MQTT broker to save the last published message for a specific topic.

- Retain = 1: The broker stores the message. When a new client subscribes to the topic later, the broker immediately sends them this last saved message.

- Retain = 0: The broker does not store the message. New subscribers get nothing until the device publishes a new one.

## `esp_event_loop_post()`

Syntax:

```c
ESP_ERROR_CHECK(esp_event_post(MY_CUSTOM_EVENT_BASE, MY_EVENT_ID,
                               &my_data, sizeof(my_data),
                               portMAX_DELAY));
```

- **`portMAX_DELAY`**: A FreeRTOS macro that evaluates to the maximum possible value of type `TickType_t` (typically `0xffffffffUL` on 32-bit systems). When passed as a timeout argument to FreeRTOS blocking functions (such as `xQueueSend`, `xSemaphoreTake`, or `esp_event_post`), it instructs the calling task to wait indefinitely (block forever) until the operation succeeds or the resource becomes available, rather than timing out after a specific duration.

### Use Cases

The default event loop is a general-purpose dispatcher. You can use it for any custom application logic without needing to create a separate event loop, including:

- Inter-task communication
- Sensor data notifications
- Button press events
- Custom component states
