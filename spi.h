// File name: spi.h
// Author: Camden Downey

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

// Function prototypes
void SPI2_write(uint8_t data);
void SPI2_write_buffer(uint8_t * data, int length);

#endif