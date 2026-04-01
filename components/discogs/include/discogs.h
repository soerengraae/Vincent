#pragma once
#include <stdbool.h>

#define DISCOGS_MAX_RESULTS 5

typedef struct {
    char title[128];
    char year[8];
    char country[64];
    char label[128];
    char thumb[256];
    int vinyl_qty;
    int release_id;
} DiscogsResult;

int discogs_search_barcode(const char *barcode, DiscogsResult *results, int max_results);
int discogs_add_to_collection(const int release_id);