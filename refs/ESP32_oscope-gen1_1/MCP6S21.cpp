/*
 * SPI routines to support MCP6S21 Programmable gain Chip 
 *  - use both to get extra gain and better input impedance (10^13 ohm || 15 pF)
 *  
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */
#include "MCP6S21.h"

void MCP6S21::begin() {
  vspi = new SPIClass(VSPI);
  vspi->begin(); // default pins : SCLK 18, MOSI 21, SS 5, (no SPI out on slave MCP6S21) 
  digitalWrite(5,HIGH);
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
}

void MCP6S21::setGain(MCPgain gain) {
  uint16_t data= 0x4000 + gain; // set gain instruction
  
// 3 lsbs of gain
// 000 Gain of +1 (Default)
// 001 Gain of +2
// 010 Gain of +4
// 011 Gain of +5
// 100 Gain of +8 
// 101 Gain of +10 
// 110 Gain of +16 
// 111 Gain of +32
  vspi->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE0));
  digitalWrite(5,LOW);
  vspi->transfer16(data); // send two bytes
  digitalWrite(5,HIGH);
  vspi->endTransaction(); 
}
