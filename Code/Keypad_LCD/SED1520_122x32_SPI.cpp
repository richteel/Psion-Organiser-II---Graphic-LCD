#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SED1520_122x32_SPI.h"

SED1520_122x32_SPI::SED1520_122x32_SPI(TeelSys_MCP23S17 *mcp) : SED1520() {
  _mcp = mcp;
}

SED1520_122x32_SPI::~SED1520_122x32_SPI() {

}

void SED1520_122x32_SPI::WaitForStatus(unsigned char status, unsigned char controller) {
  char tmp;
  SetControl(control_pins & ~SED1520_A0);
  SetControl(control_pins | SED1520_RW);
  _mcp->portMode(SED1520_DATA_PORT, INPUT);
  _mcp->writeGPIO(SED1520_DATA_PORT, 0xFF);

  do
  {
    if (controller == 0)
    {
      SetControl(control_pins | SED1520_E1);
      tmp = _mcp->readGPIO(SED1520_DATA_PORT);
      SetControl(control_pins & ~SED1520_E1);
    }
    else
    {
      SetControl(control_pins | SED1520_E2);
      tmp = _mcp->readGPIO(SED1520_DATA_PORT);
      SetControl(control_pins & ~SED1520_E2);
    }
  } while (tmp & status);

  _mcp->portMode(SED1520_DATA_PORT, OUTPUT);
}

void SED1520_122x32_SPI::WriteCommand(unsigned char commandToWrite, unsigned char ctrl) {
  WaitForStatus(0x80, ctrl);

  SetControl(control_pins & ~SED1520_A0);
  SetControl(control_pins & ~SED1520_RW);

  _mcp->writeGPIO(SED1520_DATA_PORT, commandToWrite);

  if (ctrl)
  {
    SetControl(control_pins | SED1520_E2);
    SetControl(control_pins & ~SED1520_E2);
  }
  else
  {
    SetControl(control_pins | SED1520_E1);
    SetControl(control_pins & ~SED1520_E1);
  }
}

void SED1520_122x32_SPI::WriteData(unsigned char dataToWrite) {
  WaitForStatus(0x80, 0);
  WaitForStatus(0x80, 1);
  SetControl(control_pins | SED1520_A0);
  SetControl(control_pins & ~SED1520_RW);
  _mcp->writeGPIO(SED1520_DATA_PORT, dataToWrite);

  if (lcd_x < 61)
  {
    SetControl(control_pins | SED1520_E1);
    SetControl(control_pins & ~SED1520_E1);
  }
  else
  {
    SetControl(control_pins | SED1520_E2);
    SetControl(control_pins & ~SED1520_E2);
  }
  lcd_x++;
  if (lcd_x >= SCREEN_WIDTH)
    lcd_x = 0;  
}

unsigned char SED1520_122x32_SPI::ReadData() {
  unsigned char tmp;

  WaitForStatus(0x80, 0);
  WaitForStatus(0x80, 1);
  SetControl(control_pins | SED1520_A0);
  SetControl(control_pins | SED1520_RW);
  _mcp->portMode(SED1520_DATA_PORT, INPUT);
  _mcp->writeGPIO(SED1520_DATA_PORT, 0xFF);
  if (lcd_x < 61)
  {
    SetControl(control_pins | SED1520_E1);
    SetControl(control_pins & ~SED1520_E1);
    SetControl(control_pins | SED1520_E1);
    tmp = _mcp->readGPIO(SED1520_DATA_PORT);
    SetControl(control_pins & ~SED1520_E1);
  }
  else
  {
    SetControl(control_pins | SED1520_E2);
    SetControl(control_pins & ~SED1520_E2);
    SetControl(control_pins | SED1520_E2);
    tmp = _mcp->readGPIO(SED1520_DATA_PORT);
    SetControl(control_pins & ~SED1520_E2);
  }
  _mcp->portMode(SED1520_DATA_PORT, OUTPUT);
  lcd_x++;
  if (lcd_x > 121)
    lcd_x = 0;
  return tmp;
}

void SED1520_122x32_SPI::InitDisplay() {
  _mcp->portMode(SED1520_DATA_PORT, OUTPUT);
  _mcp->portMode(SED1520_CONTROL_PORT, OUTPUT);
  GetControl();
}

unsigned char SED1520_122x32_SPI::GetControl() {
  control_pins = _mcp->readGPIO(SED1520_CONTROL_PORT);
  return control_pins;
}

void SED1520_122x32_SPI::SetControl() {
  SetControl(control_pins);
}

void SED1520_122x32_SPI::SetControl(unsigned char ctrl) {
  control_pins = ctrl;
  _mcp->writeGPIO(SED1520_CONTROL_PORT, control_pins);
}
