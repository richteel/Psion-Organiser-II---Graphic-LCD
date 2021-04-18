#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_I2C_Keypad.h"

#define RED_PIN 6
#define YEL_PIN 7
#define GRN_PIN 14

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
byte rowPins[ROWS] = {0, 1, 2, 3, 4, 5};  //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 9, 10, 11, 12, 13};  //connect to the column pinouts of the keypad

//Variables
bool SHIFT = false;
bool CAP_LOCK = false;
bool NUM_LOCK = false;

//initialize an instance of class MCP23017
Adafruit_MCP23017 mcp;
//initialize an instance of class NewKeypad
Adafruit_I2C_Keypad customKeypad = Adafruit_I2C_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, &mcp);

void setup() {
  Serial.begin(115200);           //If you change the speed here, the receiver Arduino will also need to change the speed.
  mcp.begin();
  customKeypad.begin();
  
  mcp.pinMode(RED_PIN, OUTPUT);
  mcp.pinMode(YEL_PIN, OUTPUT);
  mcp.pinMode(GRN_PIN, OUTPUT);

  mcp.digitalWrite(RED_PIN, LOW);
  mcp.digitalWrite(YEL_PIN, LOW);
  mcp.digitalWrite(GRN_PIN, LOW);
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
      mcp.digitalWrite(RED_PIN, CAP_LOCK);
      continue;
    }

    if (SHIFT && e.bit.KEY == 0x03) { // Num Lock Key
      NUM_LOCK = !NUM_LOCK;
      mcp.digitalWrite(YEL_PIN, NUM_LOCK);
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
    if (key == 0x0D)
      Serial.println();
    else
      Serial.print((char)key);
  }
  delay(5); //Small delay between each loop
}
