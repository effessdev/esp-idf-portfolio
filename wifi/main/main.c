#include <string.h>

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

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
              .ssid = "I'll add this later",
              .password = "I'll add this later",
              .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          },
  };
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config))
}
