#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "esp_http_client.h"

/* ========= SoftAP 配置 ========= */
#define AP_SSID     "xiao"
#define AP_PASS     "12345678"
#define AP_CHANNEL  1
#define MAX_STA_CONN  4

/* ========= 从机固定 IP（我们让 WROOM 固定 192.168.4.2） ========= */
#define PEER_IP   "192.168.4.2"
#define PEER_PORT 80

static const char *TAG = "S3_SOFTAP";

/* ====== 最小 HTTP GET 发送函数 ====== */
static esp_err_t send_cmd(const char *cmd)
{
    char url[128];
    snprintf(url, sizeof(url), "http://%s:%d/arm?cmd=%s", PEER_IP, PEER_PORT, cmd);

    esp_http_client_config_t cfg = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 1000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) return ESP_FAIL;

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);

    ESP_LOGI(TAG, "HTTP -> %s, err=%s, status=%d",
             url, esp_err_to_name(err), status);

    esp_http_client_cleanup(client);
    return err;
}

static void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.ap.ssid, AP_SSID, sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strlen(AP_SSID);
    strncpy((char *)wifi_config.ap.password, AP_PASS, sizeof(wifi_config.ap.password));
    wifi_config.ap.channel = AP_CHANNEL;
    wifi_config.ap.max_connection = MAX_STA_CONN;

    if (strlen(AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    } else {
        wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "SoftAP started. SSID=%s, IP=192.168.4.1", AP_SSID);
}

/* ====== MVP：用一个假“手势事件”循环代替真实识别 ======
   你之后只要把 send_cmd("ok") 挪到手势识别的 OK 分支即可 */
static void gesture_mvp_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(4000)); // 等 WROOM 连上来

    while (1) {
        // 模拟：每 3 秒发送一次 ok/left/right
        send_cmd("ok");
        vTaskDelay(pdMS_TO_TICKS(1000));
        send_cmd("left");
        vTaskDelay(pdMS_TO_TICKS(1000));
        send_cmd("right");
        vTaskDelay(pdMS_TO_TICKS(1000));
        send_cmd("stop");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_softap();

    xTaskCreate(gesture_mvp_task, "gesture_mvp_task", 4096, NULL, 5, NULL);
}
