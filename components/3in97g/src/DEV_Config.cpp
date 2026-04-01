/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-02-19
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

static spi_device_handle_t spi;

void GPIO_Config(void)
{
#if D_9PIN
    gpio_set_direction(EPD_PWR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(EPD_PWR_PIN, 1);
#endif

    gpio_set_direction(EPD_BUSY_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(EPD_RST_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(EPD_DC_PIN, GPIO_MODE_OUTPUT);

    spi_bus_config_t buscfg = {
        .mosi_io_num = EPD_MOSI_PIN,
        .miso_io_num = -1,
        .sclk_io_num = EPD_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    spi_device_interface_config_t devcfg = {
        .mode = 0, // CPOL=0, CPHA=0
        .clock_speed_hz = 4 * 1000000, // 4 MHz
        .spics_io_num = EPD_CS_PIN,
        .queue_size = 7,
    };
    
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);

    // gpio_set_direction(EPD_SCK_PIN, GPIO_MODE_OUTPUT);
    // gpio_set_direction(EPD_MOSI_PIN, GPIO_MODE_OUTPUT);
    // gpio_set_direction(EPD_CS_PIN, GPIO_MODE_OUTPUT);

    // gpio_set_level(EPD_CS_PIN, 1);
    // gpio_set_level(EPD_SCK_PIN, 0);
}

/******************************************************************************
function:
			SPI read and write
******************************************************************************/
void DEV_SPI_WriteByte(uint8_t data)
{
    // //SPI.beginTransaction(spi_settings);
    // gpio_set_level(EPD_CS_PIN, 0);

    // for (int i = 0; i < 8; i++)
    // {
    //     if ((data & 0x80) == 0)
    //         gpio_set_level(EPD_MOSI_PIN, 0);
    //     else
    //         gpio_set_level(EPD_MOSI_PIN, 1);

    //     data <<= 1;
    //     gpio_set_level(EPD_SCK_PIN, 1);
    //     gpio_set_level(EPD_SCK_PIN, 0);
    // }

    // //SPI.transfer(data);
    // gpio_set_level(EPD_CS_PIN, 1);
    // //SPI.endTransaction();	

    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data,
    };
    spi_device_transmit(spi, &t);
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t len)
{
    for (int i = 0; i < len; i++)
        DEV_SPI_WriteByte(pData[i]);
}


void DEV_Module_Exit(void)
{
#if D_9PIN
    gpio_set_level(EPD_PWR_PIN , 0);
#endif
    gpio_set_level(EPD_RST_PIN , 0);
}
