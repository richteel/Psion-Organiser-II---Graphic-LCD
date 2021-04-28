#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SED1520.h"


SED1520::SED1520() {

}

SED1520::~SED1520() {

}

void SED1520::begin() {
  Init();
}

void SED1520::Init() {
  InitDisplay();
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

void SED1520::Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy) {
  unsigned char i, j;
  for (j = 0; j < dy / 8; j++)
  {
    GoTo(x, y + j);
    for (i = 0; i < dx; i++)
      WriteData(pgm_read_byte(bmp++));
  }
}
