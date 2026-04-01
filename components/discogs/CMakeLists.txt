idf_component_register(
    SRC_DIRS "src"
    INCLUDE_DIRS "include" "${CMAKE_SOURCE_DIR}/main"
    REQUIRES esp_http_client mbedtls json
)