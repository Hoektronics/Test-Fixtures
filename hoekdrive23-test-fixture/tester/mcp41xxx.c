#include "SPI.h"

void mcp41xxx_init(byte cs_pin)
{
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
}

byte mcp41xxx_write(byte cs_pin, byte data, bool pot0, bool pot1)
{
  byte command = (0x01 << 5) | (pot1 << 1) | pot0;
  
  digitalWrite(pin, LOW);
  SPI.transfer(command);
  SPI.transfer(data);
  digitalWrite(pin, HIGH);
  
  return result;
}

byte mcp41xxx_shutdown(byte cs_pin, bool pot0, bool pot1)
{
  byte command = (0x02 << 5) | (pot1 << 1) | pot0;
  
  digitalWrite(pin, LOW);
  SPI.transfer(command);
  SPI.transfer(0);
  digitalWrite(pin, HIGH);
  
  return result;
}

byte mcp41xxx_get_wiper_position(int total_resistance, int resistance_aw)
{
  //from the datasheet
  //resistance_aw = (total_resistance * (256-position)) / 256
  //resistance_aw * 256 = total_resistance * (256-position)
  //((resistance_aw * 256) / total_resistance) = 256-position
  //((resistance_aw * 256) / total_resistance) = 256-position
  
  return 256 - ((resistance_aw * 256) / total_resistance);
}