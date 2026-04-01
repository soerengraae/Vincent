idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES driver wifi nvs_flash esp_http_client 3in97g discogs
)