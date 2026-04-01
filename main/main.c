#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "wifi.h"
#include "nvs_flash.h"
#include "discogs.h"
#include "secrets.h"

// eink includes
#include "DEV_Config.h"
#include "EPD_3in97g.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "ImageData.h"
#include "vincent_fonts.h"

static const char *TAG = "Vincent";

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "wifi_init");
    wifi_init(WIFI_SSID, WIFI_PASSWORD);

    DiscogsResult results[DISCOGS_MAX_RESULTS];
    int found = discogs_search_barcode("8719262042599", results, DISCOGS_MAX_RESULTS);

    // Create a new image cache
    uint8_t *image;
    uint32_t Imagesize = ((EPD_3IN97G_WIDTH % 4 == 0)? (EPD_3IN97G_WIDTH / 4 ): (EPD_3IN97G_WIDTH / 4 + 1)) * EPD_3IN97G_HEIGHT;
    if((image = (uint8_t *)malloc(Imagesize)) == NULL) {
        ESP_LOGI(TAG, "Failed to apply for black memory...");
        while(1);
    }

    ESP_LOGI(TAG, "Paint_NewImage");
    Paint_NewImage(image, EPD_3IN97G_WIDTH, EPD_3IN97G_HEIGHT, 0, EPD_3IN97G_WHITE);
    Paint_SetScale(4);

    GPIO_Config();

    ESP_LOGI(TAG, "EPD_3IN97G_Init_Fast");
    EPD_3IN97G_Init_Fast();
    ESP_LOGI(TAG, "Paint_SelectImage");
    Paint_SelectImage(image);
    ESP_LOGI(TAG, "Paint_Clear");
    Paint_Clear(EPD_3IN97G_WHITE);
    vTaskDelay(pdMS_TO_TICKS(2000));
    char str[128];
    if (found == 0) {
        ESP_LOGI(TAG, "No results found");
    } else {
        for (int i = 0; i < found; i++) {
            ESP_LOGI(TAG, "---");
            ESP_LOGI(TAG, "%s", results[i].title);
            snprintf(str, sizeof(str), "%s", results[i].title);
            int y = Vincent_DrawString(10, 0, str, &Font36, EPD_3IN97G_BLACK, EPD_3IN97G_WHITE);
            ESP_LOGI(TAG, "%s | %s", results[i].year, results[i].country);
            snprintf(str, sizeof(str), "%s | %s", results[i].year, results[i].country);
            y = Vincent_DrawString(10, y + Font36.Height, str, &Font36, EPD_3IN97G_BLACK, EPD_3IN97G_WHITE);
            ESP_LOGI(TAG, "%s", results[i].label);
            snprintf(str, sizeof(str), "%s", results[i].label);
            y = Vincent_DrawString(10, y + Font36.Height, str, &Font36, EPD_3IN97G_BLACK, EPD_3IN97G_WHITE);
            ESP_LOGI(TAG, "%d %s", results[i].vinyl_qty, results[i].vinyl_qty > 1 ? "Discs" : "Disc");
            snprintf(str, sizeof(str), "%d %s", results[i].vinyl_qty, results[i].vinyl_qty > 1 ? "Discs" : "Disc");
            y = Vincent_DrawString(10, y + Font36.Height, str, &Font36, EPD_3IN97G_BLACK, EPD_3IN97G_WHITE);
            // char buf[32];
            // int j;
            // for (j = 0; j < results[i].vinyl_qty && j < 10; j++) {
            //     ESP_LOGI(TAG, "%s %d %s %d", "i: ", i, "discs: ", results[i].vinyl_qty);
            //     buf[i*2]     = VINYL_GLYPH;
            //     buf[i*2 + 1] = ' ';
            // }
            // buf[j*2] = '\0';
            // Paint_DrawString_EN(10, 225, buf, &Font36, EPD_3IN97G_WHITE, EPD_3IN97G_BLACK);
            EPD_3IN97G_Display(image);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }

    ESP_LOGI(TAG, "Attempting to add %s to collection", results[0].title);
    if (discogs_add_to_collection(results[0].release_id) == 0)
        ESP_LOGI(TAG, "Added %s to collection", results[0].title);

    gpio_reset_pin(2);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(2, GPIO_MODE_OUTPUT);
    gpio_set_level(2, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_reset_pin(2);

    // ESP_LOGI(TAG, "EPD_3IN97G_Init");
    // EPD_3IN97G_Init();
    // ESP_LOGI(TAG, "EPD_3IN97G_Clear");
    // EPD_3IN97G_Clear(EPD_3IN97G_WHITE); // White
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // ESP_LOGI(TAG, "Paint_NewImage");
    // Paint_SelectImage(image);

    // ESP_LOGI(TAG, "EPD_3IN97G_Init_Fast");
    // EPD_3IN97G_Init_Fast();
    ESP_LOGI(TAG, "EPD_3IN97G_Display");
    EPD_3IN97G_Display(qr_image);
    vTaskDelay(pdMS_TO_TICKS(3000));

    ESP_LOGI(TAG, "EPD_3IN97G_Sleep");
    EPD_3IN97G_Sleep();
    vTaskDelay(pdMS_TO_TICKS(2000));
    DEV_Module_Exit();

    while (1) {
        // Wait for ADXL345 tap interrupt
        // Activate barcode scanner
        // Wait for UART RX interrupt (implement timeout - go to sleep)
        // Search discogs for barcode
        // Update e-ink to show result
        // Wait for double tap to confirm record (implement timeout - if more than one record found, go forward in queue, else go to sleep)
        // Check if already in collection: if not, add it, else skip
        // Display QR code on e-ink
        // Power down e-ink
        // Go to sleep
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}