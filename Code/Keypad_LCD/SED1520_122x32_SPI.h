#ifndef _SED1520_122x32_SPI_H_
#define _SED1520_122x32_SPI_H_

#include "TeelSys_MCP23S17.h"
#include "SED1520.h"

/*
 * Arduino Micro Pin Connections
 * Function Arduino MCPS17
 * MISO       14      14
 * SCK        15      12
 * MOSI       16      13
 * SS         17      11
 */

class SED1520_122x32_SPI : public SED1520 {
  public:
    SED1520_122x32_SPI(TeelSys_MCP23S17 *mcp);
    ~SED1520_122x32_SPI();

    void WaitForStatus(unsigned char status, unsigned char controller);
    void WriteCommand(unsigned char commandToWrite, unsigned char ctrl);
    void WriteData(unsigned char dataToWrite);
    unsigned char ReadData();

  private:
    TeelSys_MCP23S17 *_mcp;
    unsigned char control_pins = 0;

    void InitDisplay();
    void SetControl();
    void SetControl(unsigned char ctrl);
    unsigned char GetControl();
};

#endif
