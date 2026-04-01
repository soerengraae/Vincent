#pragma once
#include <stdint.h>

// Proportional font struct — replaces sFONT for Vincent's custom fonts
typedef struct {
    const uint8_t *table;       // glyph bitmaps, each padded to MaxWidth
    const uint8_t *widths;      // per-glyph advance width, indexed by (char - 32)
    uint16_t MaxWidth;          // bitmap stride per glyph (widest glyph)
    uint16_t Height;            // cell height, same for all glyphs
} sPropFont;

extern sPropFont Font36;

// Draw a proportional string. Returns the x position after the last glyph.
// Drop-in for EPD_Paint_DrawString_EN for Vincent's fonts.
int Vincent_DrawString(int x, int y, const char *str,
                       const sPropFont *font,
                       uint16_t color_fg, uint16_t color_bg);

// Returns pixel width of a string without drawing it (useful for centering)
int Vincent_MeasureString(const char *str, const sPropFont *font);