#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "sdkconfig.h" // To access values defined in the sdkconfig file

static const char *TAG = "wifi_station";

// Runs when Wi-Fi and IP events occurs
static void event_handler(
    void *arg,                   // Set to NULL if not needed
    esp_event_base_t event_base, // category of the event
    int32_t event_id,            // specific event within that category
    void *event_data // a pointer to a struct containing additional
                     // payload data specific to that event. What it
                     // contains depends on the event.
) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    esp_wifi_connect();
    ESP_LOGI(TAG, "Connect to the AP failed");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    // Convert the event_data to the correct type (since it's a generic
    // pointer by default)
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

    ESP_LOGI(TAG, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));

    // If we write two strings in C ("Got IP" and IPSTR in this case), they
    // will be concatenated together automatically. IPSTR expands to
    // "%d.%d.%d.%d".
    // IP2STR() macro evaluates to a comma-separated list of four integer
    // values (representing the four octets of an IPv4 address), which are
    // passed as individual arguments into a formatting function like
    // printf or ESP_LOGI.
    // even->ip_info.ip is the ip address itself. `&` gives the pointer
    // (memory address) to it.
  }
}

void app_main(void) {
  // ======================================
  // Step: Initialize NVS
  // ======================================

  // Requires nvs_flash.h
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // ======================================
  // Step: Initialize the network interface and event loop
  // ======================================

  ESP_ERROR_CHECK(esp_netif_init());                // Requires esp_netif.h
  ESP_ERROR_CHECK(esp_event_loop_create_default()); // Requires esp_event.h
  esp_netif_create_default_wifi_sta();              // Requires esp_wifi.h

  // ======================================
  // Step: Initialize the Wi-Fi Driver and Set Configuration
  // ======================================

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = CONFIG_WIFI_SSID,
              .password = CONFIG_WIFI_PASSWORD,
              .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          },
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  // ======================================
  // Step: Register Events and Start Wi-Fi
  // ======================================

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

  // ESP_EVENT_ANY_ID registers events with any ID, including
  // WIFI_EVENT_STA_START and WIFI_EVENT_STA_DISCONNECTED. We currently
  // only handle these two events in the event_handler.

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "Wi-Fi station started. Attempting connection...");
}
