#ifndef _TeelSys_I2C_Keypad_H_
#define _TeelSys_I2C_Keypad_H_

#include "Adafruit_MCP23017.h"
#include "Adafruit_Keypad_Ringbuffer.h"
#include "Arduino.h"
#include <string.h>

#define makeKeymap(x) ((byte *)x) ///< cast the passed key characters to bytes

#define KEY_JUST_RELEASED (0) ///< key has been released
#define KEY_JUST_PRESSED (1)  ///< key has been pressed

/**************************************************************************/
/*!
    @brief  key event structure
*/
/**************************************************************************/
union keypadEvent {
  struct {
    uint8_t KEY : 8;   ///< the keycode
    uint8_t EVENT : 8; ///< the edge
  } bit;               ///< bitfield format
  uint16_t reg;        ///< register format
};

/**************************************************************************/
/*!
    @brief  Class for interfacing GPIO with a diode-multiplexed keypad
*/
/**************************************************************************/
class TeelSys_I2C_Keypad {
public:
  TeelSys_I2C_Keypad(byte *userKeymap, byte *row, byte *col, int numRows,
                  int numCols, Adafruit_MCP23017 *mcp);
  ~TeelSys_I2C_Keypad();
  void begin();

  void tick();

  bool justPressed(byte key, bool clear = true);
  bool justReleased(byte key);
  bool isPressed(byte key);
  bool isReleased(byte key);
  int available();
  keypadEvent read();
  void clear();

private:
  byte *_userKeymap;
  byte *_row;
  byte *_col;
  volatile byte *_keystates;
  Adafruit_Keypad_Ringbuffer _eventbuf;
  Adafruit_MCP23017 *_mcp;

  int _numRows;
  int _numCols;

  volatile byte *getKeyState(byte key);
};

#endif
