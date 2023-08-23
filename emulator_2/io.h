#ifndef IO_H
#define IO_H

#include <stdint.h>

uint8_t io_in8(uint16_t address);
void io_out8(uint16_t address, uint8_t value);

#endif