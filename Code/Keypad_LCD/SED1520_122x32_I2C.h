#ifndef _SED1520_122x32_I2C_H_
#define _SED1520_122x32_I2C_H_

#include "Adafruit_MCP23017.h"
#include "SED1520.h"

class SED1520_122x32_I2C : public SED1520 {
  public:
    SED1520_122x32_I2C(Adafruit_MCP23017 *mcp);
    ~SED1520_122x32_I2C();

    void WaitForStatus(unsigned char status, unsigned char controller);
    void WriteCommand(unsigned char commandToWrite, unsigned char ctrl);
    void WriteData(unsigned char dataToWrite);
    unsigned char ReadData();

  private:
    Adafruit_MCP23017 *_mcp;
    unsigned char control_pins = 0;

    void InitDisplay();
    void SetControl();
    void SetControl(unsigned char ctrl);
    unsigned char GetControl();
};

#endif
