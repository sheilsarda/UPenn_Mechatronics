/*
 * SPI routines to support MCP6S21 Programmable gain Chip 
 *  - use both to get extra gain and better input impedance (10^13 ohm || 15 pF)
 *  - protect ESP32 potentially...
 *  - note;may slow down input slightly, 
 *  - - @K=1 has 12Mhz GBWP,  4V/us slew, FPBW=0.3MHz
 *  - - @k=10 has 2Mhz GBWP, 11V/us slew, FPBW=1.6MHz
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */

#ifndef MCP6S21_H
#define MCP6S21_H

#include <SPI.h>
#include <Arduino.h>
static const int SPICLK = 1000000; // 1MHz
enum MCPgain { ONE, TWO, FOUR, FIVE, EIGHT, TEN, SIXTEEN, THIRTYTWO};

class MCP6S21
{
private:
  SPIClass *vspi = NULL;
  
public:
  void begin();
  void setGain(MCPgain);
};


#endif
