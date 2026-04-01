#ifndef __EPD_3IN97G_H_
#define __EPD_3IN97G_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_3IN97G_WIDTH        800
// Initialization must use this resolution
#define EPD_3IN97G_HEIGHT_INIT  680
#define EPD_3IN97G_HEIGHT       480

// Color
#define  EPD_3IN97G_BLACK   0b00
#define  EPD_3IN97G_WHITE   0b01
#define  EPD_3IN97G_YELLOW  0b10
#define  EPD_3IN97G_RED     0b11

#ifdef __cplusplus
extern "C" {
#endif

void EPD_3IN97G_Init(void);
void EPD_3IN97G_Init_Fast(void);
void EPD_3IN97G_Clear(const uint8_t color);
void EPD_3IN97G_Display(const uint8_t *Image);
void EPD_3IN97G_DisplayPart(uint8_t *Image, uint16_t xstart, uint16_t ystart, uint16_t image_width, uint16_t image_heigh);
void EPD_3IN97G_Sleep(void);

#ifdef __cplusplus
}
#endif

#endif
