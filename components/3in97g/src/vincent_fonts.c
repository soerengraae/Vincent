#include "vincent_fonts.h"
#include "GUI_Paint.h"

int Vincent_DrawString(int x, int y, const char *str,
                       const sPropFont *font,
                       uint16_t color_fg, uint16_t color_bg)
{
    int cx = x;
    while (*str) {
        unsigned char c = (unsigned char)*str;
        if (c < 32 || c > 126) { str++; continue; }

        if ((cx + font->MaxWidth) > Paint.Width ) {
            cx = x - font->widths[c - 32];
            y += font->Height;
        }

        int idx        = c - 32;
        int glyph_w    = font->widths[idx];
        int bytes_row  = (font->MaxWidth + 7) / 8;
        const uint8_t *glyph = font->table + idx * bytes_row * font->Height;

        for (int row = 0; row < font->Height; row++) {
            for (int col = 0; col < glyph_w; col++) {
                uint8_t byte = glyph[row * bytes_row + col / 8];
                int bit = (byte >> (7 - (col % 8))) & 1;
                Paint_SetPixel(cx + col, y + row, bit ? color_fg : color_bg);
            }
        }
        cx += glyph_w;
        str++;
    }
    return y;
}

int Vincent_MeasureString(const char *str, const sPropFont *font)
{
    int w = 0;
    while (*str) {
        unsigned char c = (unsigned char)*str;
        if (c >= 32 && c <= 126)
            w += font->widths[c - 32];
        str++;
    }
    return w;
}