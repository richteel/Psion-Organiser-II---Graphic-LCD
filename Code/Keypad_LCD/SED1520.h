// Adapted from: http://en.radzio.dxp.pl/sed1520/

#ifndef _SED1520_H_
#define _SED1520_H_

#include "font_5x7.h"

#define DISPLAY_ON 0xAF
#define DISPLAY_OFF 0xAE
#define DISPLAY_START_LINE 0xC0
#define PAGE_ADDRESS_SET 0xB8
#define COLUMN_ADDRESS_SET 0x00
#define ADC_CLOCKWISE 0xA0
#define ADC_COUNTERCLOCKWISE 0xA1
#define STATIC_DRIVE_ON 0xA5
#define STATIC_DRIVE_OFF 0xA4
#define DUTY_RATIO_16 0xA8
#define DUTY_RATIO_32 0xA9
#define READ_MODIFY_WRITE 0xE0
#define END_READ_MODIFY 0xEE
#define RESET 0xE2

#define SCREEN_WIDTH  122

#define SED1520_DATA_PORT   MCP23017_PORT_B
#define SED1520_CONTROL_PORT  MCP23017_PORT_A

#define SED1520_A0 (1 << 0)
#define SED1520_E1 (1 << 1)
#define SED1520_E2 (1 << 2)
#define SED1520_RW (1 << 3)
#define SED1520_BL (1 << 6)

class SED1520 {
  public:
    SED1520();
    ~SED1520();
    void begin();
    void Init();
    void GoTo(unsigned char x, unsigned char y);
    void ClearScreen();
    void WriteChar(char c);
    void WriteString(char *s);
    void SetPixel(unsigned char x, unsigned char y, unsigned char color);
    void Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy);

    virtual void WaitForStatus(unsigned char status, unsigned char controller) = 0;
    virtual void WriteCommand(unsigned char commandToWrite, unsigned char ctrl) = 0;
    virtual void WriteData(unsigned char dataToWrite) = 0;
    virtual unsigned char ReadData() = 0;

    // Graphics
    void Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
    void Circle(unsigned char cx, unsigned char cy , unsigned char radius);
    void Line(unsigned int X1, unsigned int Y1, unsigned int X2, unsigned int Y2);

    unsigned char color = 1;

  protected:
    unsigned char lcd_x = 0, lcd_y = 0;

    virtual void InitDisplay() = 0;

    // Graphics
};

#endif
