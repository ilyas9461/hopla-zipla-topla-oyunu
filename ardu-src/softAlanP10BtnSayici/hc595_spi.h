#ifndef hc595_spi_h_

#define hc595_spi_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// defines ardu pins for 74HC595 chip
#define LATCH_DIO   4
#define CLK_DIO     3
#define DATA_DIO    2


#define ledClear() writeSPI595(0x0000)
#define ledFull() writeSPI595(0xFFFF)


void init_595();
void writeSPI595(uint16_t value);
void shiftOut_595(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint16_t val);


#endif
