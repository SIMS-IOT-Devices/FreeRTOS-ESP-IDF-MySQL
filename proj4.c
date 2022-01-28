#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "my_data.h"

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

// POST ------------------------------------------------------------------------
esp_err_t client_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void client_post_function()
{
    esp_http_client_config_t config_post = {
        .url = "http://192.168.1.186:8085/post",
        .method = HTTP_METHOD_POST,
        .event_handler = client_post_handler};
        
    esp_http_client_handle_t client_post = esp_http_client_init(&config_post);

    char  *post_data = "{\"data\" : \"Data from ESP32\"\n}";
    esp_http_client_set_post_field(client_post, post_data, strlen(post_data));
    esp_http_client_set_header(client_post, "Content-Type", "application/json");

    printf("1 ...........\n");
    esp_http_client_perform(client_post);
    printf("2 ...........\n\n");
    esp_http_client_cleanup(client_post);
}

// GET -------------------------------------------------------------------------
esp_err_t client_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

static void client_get_function()
{
    esp_http_client_config_t config_get = {
        .url = "http://worldclockapi.com/api/json/utc/now",
        .method = HTTP_METHOD_GET,
        .event_handler = client_get_handler};
        
    esp_http_client_handle_t client_get = esp_http_client_init(&config_get);

    printf("1 ...........\n");
    esp_http_client_perform(client_get);
    printf("2 ...........\n\n");
    esp_http_client_cleanup(client_get);
}

static void client_get_function_local()
{
    esp_http_client_config_t config_get = {
        .url = "http://192.168.1.186:8085/get",
        .method = HTTP_METHOD_GET,
        .event_handler = client_get_handler};
        
    esp_http_client_handle_t client_get = esp_http_client_init(&config_get);

    printf("1 ...........\n");
    esp_http_client_perform(client_get);
    printf("2 ...........\n\n");
    esp_http_client_cleanup(client_get);
}

// MAIN ------------------------------------------------------------------------
void app_main(void)
{
    nvs_flash_init();
    wifi_connection();

    vTaskDelay(3000 / portTICK_PERIOD_MS);
    printf("WIFI was initiated ...........\n\n");

    client_get_function();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    client_get_function_local();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    client_post_function();
}
