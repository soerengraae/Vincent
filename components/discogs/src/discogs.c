#include "discogs.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"
#include <string.h>
#include <stdlib.h>
#include "secrets.h"
#define RESPONSE_BUF_SIZE  32768

static const char *TAG = "discogs";
static char response_buf[RESPONSE_BUF_SIZE];
static int response_len = 0;

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (response_len + evt->data_len < RESPONSE_BUF_SIZE) {
                memcpy(response_buf + response_len, evt->data, evt->data_len);
                response_len += evt->data_len;
            } else {
                ESP_LOGW(TAG, "Response buffer full, truncating");
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            response_buf[response_len] = '\0';
            break;
        default:
            break;
    }
    return ESP_OK;
}

static bool http_get(const char *url)
{
    response_len = 0;
    memset(response_buf, 0, sizeof(response_buf));

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "User-Agent", "VincentScanner/1.0");

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);

    if (err != ESP_OK || status != 200) {
        ESP_LOGE(TAG, "HTTP failed: %s, status %d", esp_err_to_name(err), status);
        return false;
    }

    ESP_LOGI(TAG, "Got %d bytes", response_len);
    return true;
}

static bool http_post(const char *url)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "User-Agent", "VincentScanner/1.0");
    esp_http_client_set_header(client, "Authorization", "Discogs token=" DISCOGS_TOKEN);
    esp_http_client_set_header(client, "Content-Length", "0");
    esp_http_client_set_post_field(client, NULL, 0);

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);

    if (err != ESP_OK || status != 201) {
        ESP_LOGE(TAG, "POST failed: %s, status %d", esp_err_to_name(err), status);
        return false;
    }

    ESP_LOGI(TAG, "Added to collection");
    return true;
}

static int parse_results(DiscogsResult *results, int max_results)
{
    cJSON *root = cJSON_Parse(response_buf);
    if (!root) {
        ESP_LOGE(TAG, "JSON parse failed");
        return 0;
    }

    cJSON *result_array = cJSON_GetObjectItem(root, "results");
    if (!cJSON_IsArray(result_array)) {
        cJSON_Delete(root);
        return 0;
    }

    int count = 0;
    cJSON *r;
    cJSON_ArrayForEach(r, result_array) {
        if (count >= max_results) break;

        // Check for vinyl format
        bool has_vinyl = false;
        int vinyl_qty = 0;
        cJSON *formats = cJSON_GetObjectItem(r, "formats");
        cJSON *fmt;
        cJSON_ArrayForEach(fmt, formats) {
            cJSON *name = cJSON_GetObjectItem(fmt, "name");
            if (name && strcmp(name->valuestring, "Vinyl") == 0) {
                has_vinyl = true;
                cJSON *qty = cJSON_GetObjectItem(fmt, "qty");
                if (qty) vinyl_qty += atoi(qty->valuestring);
            }
        }
        if (!has_vinyl) continue;

        DiscogsResult *res = &results[count];
        res->vinyl_qty = vinyl_qty;

        cJSON *val;
        val = cJSON_GetObjectItem(r, "title");
        if (val) strncpy(res->title, val->valuestring, sizeof(res->title) - 1);

        val = cJSON_GetObjectItem(r, "year");
        if (val) strncpy(res->year, val->valuestring, sizeof(res->year) - 1);

        val = cJSON_GetObjectItem(r, "country");
        if (val) strncpy(res->country, val->valuestring, sizeof(res->country) - 1);

        val = cJSON_GetObjectItem(r, "thumb");
        if (val) strncpy(res->thumb, val->valuestring, sizeof(res->thumb) - 1);

        val = cJSON_GetObjectItem(r, "id");
        if (val) res->release_id = val->valueint;

        cJSON *labels = cJSON_GetObjectItem(r, "label");
        if (cJSON_IsArray(labels) && cJSON_GetArraySize(labels) > 0)
            strncpy(res->label, cJSON_GetArrayItem(labels, 0)->valuestring, sizeof(res->label) - 1);

        count++;
    }

    cJSON_Delete(root);
    return count;
}

int discogs_search_barcode(const char *barcode, DiscogsResult *results, int max_results)
{
    char url[256];
    snprintf(url, sizeof(url), "https://api.discogs.com/database/search?q=%s&type=release&token=%s", barcode, DISCOGS_TOKEN);

    if (!http_get(url)) return 0;
    return parse_results(results, max_results);
}

int discogs_add_to_collection(const int release_id) {
    char url[256];
    snprintf(url, sizeof(url), "https://api.discogs.com/users/srhgraae/collection/folders/1/releases/%d", release_id);
    ESP_LOGI("discogs", "url: %s", url);

    if (!http_post(url)) return 0;
    return -1;
}