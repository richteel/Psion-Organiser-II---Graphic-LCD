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

// Graphics
void SED1520::Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a)
{
  unsigned char j; // zmienna pomocnicza
  // rysowanie linii pionowych (boki)
  for (j = 0; j < a; j++) {
    SetPixel(x, y + j, color);
    SetPixel(x + b - 1, y + j, color);
  }
  // rysowanie linii poziomych (podstawy)
  for (j = 0; j < b; j++) {
    SetPixel(x + j, y, color);
    SetPixel(x + j, y + a - 1, color);
  }
}
//

void SED1520::Circle(unsigned char cx, unsigned char cy , unsigned char radius)
{
  int x, y, xchange, ychange, radiusError;
  x = radius;
  y = 0;
  xchange = 1 - 2 * radius;
  ychange = 1;
  radiusError = 0;
  while (x >= y)
  {
    SetPixel(cx + x, cy + y, color);
    SetPixel(cx - x, cy + y, color);
    SetPixel(cx - x, cy - y, color);
    SetPixel(cx + x, cy - y, color);
    SetPixel(cx + y, cy + x, color);
    SetPixel(cx - y, cy + x, color);
    SetPixel(cx - y, cy - x, color);
    SetPixel(cx + y, cy - x, color);
    y++;
    radiusError += ychange;
    ychange += 2;
    if ( 2 * radiusError + xchange > 0 )
    {
      x--;
      radiusError += xchange;
      xchange += 2;
    }
  }
}
//

void SED1520::Line(unsigned int X1, unsigned int Y1, unsigned int X2, unsigned int Y2)
{
  int CurrentX, CurrentY, Xinc, Yinc,
      Dx, Dy, TwoDx, TwoDy,
      TwoDxAccumulatedError, TwoDyAccumulatedError;

  Dx = (X2 - X1); // obliczenie sk³adowej poziomej
  Dy = (Y2 - Y1); // obliczenie sk³adowej pionowej

  TwoDx = Dx + Dx; // podwojona sk³adowa pozioma
  TwoDy = Dy + Dy; // podwojona sk³adowa pionowa

  CurrentX = X1; // zaczynamy od X1
  CurrentY = Y1; // oraz Y1

  Xinc = 1; // ustalamy krok zwiêkszania pozycji w poziomie
  Yinc = 1; // ustalamy krok zwiêkszania pozycji w pionie

  if (Dx < 0) // jesli sk³adowa pozioma jest ujemna
  {
    Xinc = -1; // to bêdziemy siê "cofaæ" (krok ujemny)
    Dx = -Dx;  // zmieniamy znak sk³adowej na dodatni
    TwoDx = -TwoDx; // jak równie¿ podwojonej sk³adowej
  }

  if (Dy < 0) // jeœli sk³adowa pionowa jest ujemna
  {
    Yinc = -1; // to bêdziemy siê "cofaæ" (krok ujemny)
    Dy = -Dy; // zmieniamy znak sk³adowej na dodatki
    TwoDy = -TwoDy; // jak równiez podwojonej sk³adowej
  }

  SetPixel(X1, Y1, color); // stawiamy pierwszy krok (zapalamy pierwszy piksel)

  if ((Dx != 0) || (Dy != 0)) // sprawdzamy czy linia sk³ada siê z wiêcej ni¿ jednego punktu ;)
  {
    // sprawdzamy czy sk³adowa pionowa jest mniejsza lub równa sk³adowej poziomej
    if (Dy <= Dx) // jeœli tak, to idziemy "po iksach"
    {
      TwoDxAccumulatedError = 0; // zerujemy zmienn¹
      do // ruszamy w drogê
      {
        CurrentX += Xinc; // do aktualnej pozycji dodajemy krok
        TwoDxAccumulatedError += TwoDy; // a tu dodajemy podwojon¹ sk³adow¹ pionow¹
        if (TwoDxAccumulatedError > Dx) // jeœli TwoDxAccumulatedError jest wiêkszy od Dx
        {
          CurrentY += Yinc; // zwiêkszamy aktualn¹ pozycjê w pionie
          TwoDxAccumulatedError -= TwoDx; // i odejmujemy TwoDx
        }
        SetPixel(CurrentX, CurrentY, color); // stawiamy nastêpny krok (zapalamy piksel)
      } while (CurrentX != X2); // idziemy tak d³ugo, a¿ osi¹gniemy punkt docelowy
    }
    else // w przeciwnym razie idziemy "po igrekach"
    {
      TwoDyAccumulatedError = 0;
      do
      {
        CurrentY += Yinc;
        TwoDyAccumulatedError += TwoDx;
        if (TwoDyAccumulatedError > Dy)
        {
          CurrentX += Xinc;
          TwoDyAccumulatedError -= TwoDy;
        }
        SetPixel(CurrentX, CurrentY, color);
      } while (CurrentY != Y2);
    }
  }
}
