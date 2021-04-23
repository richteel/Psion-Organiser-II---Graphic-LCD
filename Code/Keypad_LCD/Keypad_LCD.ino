#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "TeelSys_I2C_Keypad.h"
#include "SED1520.h"

#define RED_PIN 1
#define YEL_PIN 0
#define GRN_PIN 9

const byte ROWS = 6;  // rows
const byte COLS = 6; // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},
  {0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B},
  {0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11},
  {0x12, 0x13, 0x14, 0x15, 0x16, 0x17},
  {0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D},
  {0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23}
};

char keys_standard[ROWS][COLS] = {
  {0x00, 0x01, 0x1E, 0x1F, 0x1D, 0x1C},
  {'a', 'b', 'c', 'd', 'e', 'f'},
  {'g', 'h', 'i', 'j', 'k', 'l'},
  {'m', 'n', 'o', 'p', 'q', 'r'},
  {'s', 't', 'u', 'v', 'w', 'x'},
  {0x1E, 0x7F, 'y', 'z', ' ', 0x0D}
};

char keys_cap[ROWS][COLS] = {
  {0x00, 0x01, 0x1E, 0x1F, 0x1D, 0x1C},
  {'A', 'B', 'C', 'D', 'E', 'F'},
  {'G', 'H', 'I', 'J', 'K', 'L'},
  {'M', 'N', 'O', 'P', 'Q', 'R'},
  {'S', 'T', 'U', 'V', 'W', 'X'},
  {0x1E, 0x08, 'Y', 'Z', ' ', 0x0D}
};

char keys_num[ROWS][COLS] = {
  {0x00, 0x01, 0x1E, 0x1F, 0x1D, 0x1C},
  {'<', '>', '(', ')', '%', '/'},
  {'=', '"', '7', '8', '9', '*'},
  {',', '$', '4', '5', '6', '-'},
  {';', ':', '1', '2', '3', '+'},
  {0x1E, 0x7F, '0', '.', ' ', 0x0D}
};

char keys_special[ROWS][COLS] = {
  {0x00, 0x01, 0x1E, 0x1F, 0x1D, 0x1C},
  {'[', ']', '{', '}', '|', '\\'},
  {'~', '\'', '&', '*', '(', '?'},
  {'`', 0x9C, '$', '%', '^', 0x09},
  {';', ':', '!', '@', '#', 0x0A},
  {0x1E, 0x7F, ')', '.', ' ', 0x0D}
};

//Inputs/outputs
byte rowPins[ROWS] = {15, 13, 11, 2, 4, 6};  //connect to the row pinouts of the keypad
byte colPins[COLS] = {14, 12, 10, 3, 5, 7};  //connect to the column pinouts of the keypad

//Variables
bool SHIFT = false;
bool CAP_LOCK = false;
bool NUM_LOCK = false;

//---------- LCD ----------
#define BL_PIN 6

//initialize an instance of class MCP23017
Adafruit_MCP23017 mcp0;
Adafruit_MCP23017 mcp1;
//initialize an instance of class NewKeypad
TeelSys_I2C_Keypad customKeypad = TeelSys_I2C_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, &mcp0);
SED1520 lcd = SED1520(&mcp1);

void setup() {
  Serial.begin(115200);           //If you change the speed here, the receiver Arduino will also need to change the speed.
  mcp0.begin();
  customKeypad.begin();
  
  mcp0.pinMode(RED_PIN, OUTPUT);
  mcp0.pinMode(YEL_PIN, OUTPUT);
  mcp0.pinMode(GRN_PIN, OUTPUT);

  mcp0.digitalWrite(RED_PIN, LOW);
  mcp0.digitalWrite(YEL_PIN, LOW);
  mcp0.digitalWrite(GRN_PIN, LOW);

  mcp1.begin(0x01);
  mcp1.pinMode(BL_PIN, OUTPUT);
  mcp1.digitalWrite(BL_PIN, LOW);

  lcd.begin();
  mcp0.digitalWrite(GRN_PIN, HIGH);
}

void loop() {
  ////////////////////////////////////////////////////////////
  ///////Now detect when a keyboard button was pressed/////////
  ////////////////////////////////////////////////////////////
  customKeypad.tick();                      //Create the rows pulses

  while (customKeypad.available()) {        //If a new push button was detected...
    keypadEvent e = customKeypad.read();    //Get what character was pressed

    /**********************************************************/
    /***                 Check Modifier Keys                ***/
    /**********************************************************/
    if (e.bit.KEY == 0x1E) { // Shift Key
      SHIFT = e.bit.EVENT == KEY_JUST_PRESSED;
      continue;
    }

    if (e.bit.EVENT != KEY_JUST_PRESSED) {
      continue;
    }

    if (SHIFT && e.bit.KEY == 0x02) { // Cap Lock Key
      CAP_LOCK = !CAP_LOCK;
      mcp0.digitalWrite(RED_PIN, CAP_LOCK);
      continue;
    }

    if (SHIFT && e.bit.KEY == 0x03) { // Num Lock Key
      NUM_LOCK = !NUM_LOCK;
      mcp0.digitalWrite(YEL_PIN, NUM_LOCK);
      continue;
    }

    byte row = e.bit.KEY / 6;
    byte col = e.bit.KEY % 6;
    byte state = 0;
    /*
       state = 0 keys_standard
       state = 1 keys_cap
       state = 2 keys_num
       state = 3 keys_special
       state = 4 keys_special2
    */

    if (CAP_LOCK && NUM_LOCK) {
      if (SHIFT)
        state = 4;
      else
        state = 3;
    }
    else if (NUM_LOCK) {
      if (SHIFT)
        state = 0;
      else
        state = 2;
    }
    else if (CAP_LOCK) {
      if (SHIFT)
        state = 0;
      else
        state = 1;
    }
    else {
      if (SHIFT)
        state = 1;
      else
        state = 0;
    }

    char key = 0x00;

    switch (state) {
      case 4:
      case 3:
        key = keys_special[row][col];
        break;
      case 2:
        key = keys_num[row][col];
        break;
      case 1:
        key = keys_cap[row][col];
        break;
      default:
        key = keys_standard[row][col];
        break;
    }

    //Serial.print("row = "); Serial.print(row); Serial.print(" col = "); Serial.println(col);
    if(key == 0x00)
      mcp1.digitalWrite(BL_PIN, !mcp1.digitalRead(BL_PIN));
    else if (key == 0x0D)
      Serial.println();
    else
      Serial.print((char)key);
  }
  delay(5); //Small delay between each loop
}
