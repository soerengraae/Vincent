#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>
#include <stdio.h>

/**
 * GPIO config
**/
#define EPD_SCK_PIN  GPIO_NUM_13
#define EPD_MOSI_PIN GPIO_NUM_14
#define EPD_CS_PIN   GPIO_NUM_15
#define EPD_RST_PIN  GPIO_NUM_26
#define EPD_DC_PIN   GPIO_NUM_27
#define EPD_BUSY_PIN GPIO_NUM_25

/**
 * Enable it or not according to the actual hardware used, and the corresponding pin
 */
#define D_9PIN  1
#if D_9PIN
    #define EPD_PWR_PIN GPIO_NUM_33
#endif


#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

/*------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

void GPIO_Config(void);
void DEV_SPI_WriteByte(uint8_t data);
uint8_t DEV_SPI_ReadByte();
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t len);
void DEV_Module_Exit(void);

#ifdef __cplusplus
}
#endif

#endif
