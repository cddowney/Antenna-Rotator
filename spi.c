// File name: spi.c
// Author: Camden
// Purpose: Low-level, simple SPI functions

#include "spi.h"
#include <xc.h>

void SPI2_write(uint8_t data)
{
    while (SPI2STATbits.SPITBF);    // wait for TX space
    SPI2BUF = data;                 // start transmission
    while (SPI2STATbits.SPIBUSY);   // wait until done
    (void)SPI2BUF;                  // discard RX
}

void SPI2_write_buffer(uint8_t *data, int length)
{
    for (int i = 0; i < length; i++)
        SPI2_write(data[i]);
}