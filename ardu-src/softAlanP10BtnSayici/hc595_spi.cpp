#include "hc595_spi.h"

void init_595()
{

  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);

}

/* Write a decimal number between 0 and 9 to one of the 4 digits of the display */
void writeSPI595(uint16_t value)
{
  digitalWrite(LATCH_DIO, LOW);
  // (MSB)(xxxx xxL10L9 L8L7L6L5 L4L3L2L1)(LSB)S
  //MSB first 
  shiftOut_595(DATA_DIO, CLK_DIO, MSBFIRST, value);  

  digitalWrite(LATCH_DIO, HIGH);
}

void shiftOut_595(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint16_t val)
{
  uint16_t i;

  //for (i = 0; i < 8; i++)
  for (i = 0; i < 16; i++)
  {
    if (bitOrder == LSBFIRST)
    {
      digitalWrite(dataPin, val & 1);
      val >>= 1;
    }
    else
    {
      //if ((val & 128) == 0)
      if ((val & 0x8000) == 0) //32768
        digitalWrite(dataPin, LOW); //128 -- > 0b1000 0000 for 8 bit
      else
        digitalWrite(dataPin, HIGH); //128 -- > 0b1000 0000 for 8 bit
      val <<= 1;
    }

    digitalWrite(clockPin, LOW);
    digitalWrite(clockPin, HIGH);
  }
}

