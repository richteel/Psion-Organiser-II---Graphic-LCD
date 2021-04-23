#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SED1520.h"


SED1520::SED1520(Adafruit_MCP23017 *mcp) {
  _mcp = mcp;
}

SED1520::~SED1520() {

}

void SED1520::begin() {
  Init();
}

void SED1520::InitPorts() {
  _mcp->portMode(SED1520_DATA_PORT, OUTPUT);
  _mcp->portMode(SED1520_CONTROL_PORT, OUTPUT);
}

void SED1520::WaitForStatus(unsigned char status, unsigned char controller) {
  char tmp;
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_A0);
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_RW);
  _mcp->portMode(SED1520_DATA_PORT, INPUT);
  _mcp->writeGPIO(SED1520_DATA_PORT, 0xFF);

  do
  {
    if (controller == 0)
    {
      _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E1);
      tmp = _mcp->readGPIO(SED1520_DATA_PORT);
      _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E1);
    }
    else
    {
      _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E2);
      tmp = _mcp->readGPIO(SED1520_DATA_PORT);
      _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E2);
    }
  } while (tmp & status);

  _mcp->portMode(SED1520_DATA_PORT, OUTPUT);
}

void SED1520::WriteCommand(unsigned char commandToWrite, unsigned char ctrl) {
  WaitForStatus(0x80, ctrl);

  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_A0);
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_RW);

  _mcp->writeGPIO(SED1520_DATA_PORT, commandToWrite);

  if (ctrl)
  {
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E2);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E2);
  }
  else
  {
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E1);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E1);
  }
}

void SED1520::GoTo(unsigned char x, unsigned char y) {
  lcd_x = x;
  lcd_y = y;

  if (x < (SCREEN_WIDTH / 2))
  {
    WriteCommand(COLUMN_ADDRESS_SET | lcd_x, 0);
    WriteCommand(PAGE_ADDRESS_SET | lcd_y, 0);
    WriteCommand(COLUMN_ADDRESS_SET | 0, 1);
    WriteCommand(PAGE_ADDRESS_SET | lcd_y, 1);
  }
  else
  {
    WriteCommand(COLUMN_ADDRESS_SET | (lcd_x - (SCREEN_WIDTH / 2)), 1);
    WriteCommand(PAGE_ADDRESS_SET | lcd_y, 1);
  }
}

void SED1520::WriteData(unsigned char dataToWrite) {
  WaitForStatus(0x80, 0);
  WaitForStatus(0x80, 1);
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_A0);
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_RW);
  _mcp->writeGPIO(SED1520_DATA_PORT, dataToWrite);

  if (lcd_x < 61)
  {
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E1);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E1);
  }
  else
  {
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E2);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E2);
  }
  lcd_x++;
  if (lcd_x >= SCREEN_WIDTH)
    lcd_x = 0;
}

unsigned char SED1520::ReadData() {
  unsigned char tmp;

  WaitForStatus(0x80, 0);
  WaitForStatus(0x80, 1);
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_A0);
  _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_RW);
  _mcp->portMode(SED1520_DATA_PORT, INPUT);
  _mcp->writeGPIO(SED1520_DATA_PORT, 0xFF);
  if (lcd_x < 61)
  {
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E1);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E1);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E1);
    tmp = _mcp->readGPIO(SED1520_DATA_PORT);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E1);
  }
  else
  {
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E2);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E2);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) | SED1520_E2);
    tmp = _mcp->readGPIO(SED1520_DATA_PORT);
    _mcp->writeGPIO(SED1520_CONTROL_PORT, _mcp->readGPIO(SED1520_CONTROL_PORT) & ~SED1520_E2);
  }
  _mcp->portMode(SED1520_DATA_PORT, OUTPUT);
  lcd_x++;
  if (lcd_x > 121)
    lcd_x = 0;
  return tmp;
}

void SED1520::ClearScreen() {
  char j, i;
  for (j = 0; j < 4; j++)
  {
    GoTo(0, j);
    for (i = 0; i < SCREEN_WIDTH; i++)
    {
      WriteData(0);
    }
  }
  GoTo(0, 0);
}

void SED1520::WriteChar(char c) {
  char i;
  c -= 32;
  for (i = 0; i < 5; i++)
    WriteData(pgm_read_byte(font5x7 + (5 * c) + i));
  WriteData(0x00);
}

void SED1520::WriteString(char * s) {
  while (*s)
  {
    WriteChar(*s++);
  }
}

void SED1520::SetPixel(unsigned char x, unsigned char y, unsigned char color) {
  unsigned char temp;
  GoTo(x, y / 8);
  temp = ReadData();
  GoTo(x, y / 8);
  if (color)
    WriteData(temp | (1 << (y % 8)));
  else
    WriteData(temp & ~(1 << (y % 8)));
}

void SED1520::Init() {
  InitPorts();
  WriteCommand(RESET, 0);
  WriteCommand(RESET, 1);
  WaitForStatus(0x10, 0);
  WaitForStatus(0x10, 1);
  WriteCommand(DISPLAY_ON, 0);
  WriteCommand(DISPLAY_ON, 1);
  WriteCommand(DISPLAY_START_LINE | 0, 0);
  WriteCommand(DISPLAY_START_LINE | 0, 1);
  ClearScreen();
}

void SED1520::Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy) {
  unsigned char i, j;
  for (j = 0; j < dy / 8; j++)
  {
    GoTo(x, y + j);
    for (i = 0; i < dx; i++)
      WriteData(pgm_read_byte(bmp++));
  }
}

unsigned char SED1520::GetControl() {
  control_pins = _mcp->readGPIO(SED1520_CONTROL_PORT);
  return control_pins;
}

void SED1520::SetControl() {
  SetControl(control_pins);
}

void SED1520::SetControl(unsigned char ctrl) {
  control_pins = ctrl;
  _mcp->writeGPIO(SED1520_CONTROL_PORT, control_pins);
}
