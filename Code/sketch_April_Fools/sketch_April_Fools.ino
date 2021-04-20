// SOURCE: https://www.htlinux.com/esp8266-json-weather-display/

#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "logo.h"
#include "menu.h"

U8G2_SED1520_122X32_F u8g2(U8G2_R0, 8, 9, 10, 11, 4, 5, 6, 7, /*dc=*/ A0, /*e1=*/ A3, /*e2=*/ A2, /* reset=*/  A4);   // Set R/W to low!
bool pcSerialConnected = true;

int state = 0;
const int delayMs = 2000;

void u8g2Print(const char *s, u8g2_uint_t line) {
  u8g2_uint_t y = (12 * line);
  if (line > 1) {
    y += 2;
  }
  u8g2.setCursor(0, y);
  u8g2.setFont(u8g2_font_unifont_t_latin); //first font
  u8g2.print(s);
}

void setup()
{
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFontDirection(0);

  //  u8g2.clearBuffer();
  //  u8g2Print("Booting", 1);
  //  u8g2.sendBuffer();

  Serial.begin(115200);   // PC for debuging

  /****** BEGIN: Serial Connection to PC ******/
  // Wait for PC Serial to connect (Typically < 300ms)
  unsigned long startMillis = millis();
  unsigned long endMillis = millis();
  while (!Serial) {
    endMillis = millis();
    if (endMillis - startMillis >= 1000) {
      pcSerialConnected = false;
      break;
    }
  }
  if (pcSerialConnected) {
    Serial.print("Time of Serial 0 = "); Serial.print(endMillis - startMillis); Serial.println(" ms");
  }
  /****** END: Serial Connection to PC ******/

  state = 0;
}

void loop() {
  state++;
  delay(delayMs);

  switch (state) {
    case 1:
      u8g2.clearBuffer();
      u8g2Print("  Organiser G", 1);
      u8g2Print("  ©Psion 1989", 2); // © Copyright Psion PLC 1989
      u8g2.sendBuffer();
      delay(delayMs);
      break;
    case 2:
      u8g2.clearBuffer();
      u8g2.firstPage();
      do {
        u8g2.drawXBMP( 0, 0, Psion_logo_122x32_20bw_width, Psion_logo_122x32_20bw_height, Psion_logo_122x32_20bw_bits);
      } while ( u8g2.nextPage() );
      u8g2.sendBuffer();
      delay(delayMs);
      break;
    case 3:
      u8g2.clearBuffer();
      u8g2.firstPage();
      do {
        u8g2.drawXBMP( 0, 0, Menu_width, Menu_height, Menu_bits);
      } while ( u8g2.nextPage() );
      u8g2.sendBuffer();
      break;
    default:
      delay(delayMs);
      state = state - 1;
      break;
  }

}
