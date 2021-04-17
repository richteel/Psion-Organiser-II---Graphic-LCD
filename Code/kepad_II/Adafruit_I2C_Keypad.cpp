#include "Adafruit_I2C_Keypad.h"

#define _KEY_PRESSED_POS (1)
#define _KEY_PRESSED (1UL << _KEY_PRESSED_POS)

#define _JUST_PRESSED_POS (2)
#define _JUST_PRESSED (1UL << _JUST_PRESSED_POS)

#define _JUST_RELEASED_POS (3)
#define _JUST_RELEASED (1UL << _JUST_RELEASED_POS)

#define _KEYPAD_SETTLING_DELAY 20

/**************************************************************************/
/*!
    @brief  default constructor
    @param  userKeymap a multidimensional array of key characters
    @param  rowPort is the port of the MCP23017 that is connected to the rows of the keypad
    @param  colPort is the port of the MCP23017 that is connected to the columns of the keypad
    @param  numRows the number of rows on the keypad
    @param  numCols the number of columns on the keypad
*/
/**************************************************************************/
Adafruit_I2C_Keypad::Adafruit_I2C_Keypad(byte *userKeymap, uint8_t rowPort, uint8_t colPort,
    int numRows, int numCols, Adafruit_MCP23017 *mcp) {
  _userKeymap = userKeymap;
  _rowPort = rowPort;
  _colPort = colPort;
  _numRows = numRows;
  _numCols = numCols;
  _mcp = mcp;

  _keystates = NULL;
}

/**************************************************************************/
/*!
    @brief  default destructor
*/
/**************************************************************************/
Adafruit_I2C_Keypad::~Adafruit_I2C_Keypad() {
  if (_keystates != NULL) {
    free((void *)_keystates);
  }
}

/**************************************************************************/
/*!
    @brief  get the state of a key with the given name
    @param  key the name of the key to be checked
*/
/**************************************************************************/
volatile byte *Adafruit_I2C_Keypad::getKeyState(byte key) {
  for (int i = 0; i < _numRows * _numCols; i++) {
    if (_userKeymap[i] == key) {
      return _keystates + i;
    }
  }
  return NULL;
}

/**************************************************************************/
/*!
    @brief  read the array of switches and place any events in the buffer.
*/
/**************************************************************************/
void Adafruit_I2C_Keypad::tick() {
  uint8_t evt;

  int i = 0;
  for (int r = 0; r < _numRows; r++) {
    _mcp->writeGPIO(_rowPort, ~(0x01 << r)); // row pin on _rowPort set to 0 (low)

    delayMicroseconds(_KEYPAD_SETTLING_DELAY);
    uint8_t scanValue = _mcp->readGPIO(MCP23017_PORT_B);

    for (int c = 0; c < _numCols; c++) {
      i = r * _numCols + c;

      // Was the key in the column pressed?
      bool pressed = ((scanValue >> c) & 0x01 == 0x01);
      // Serial.print((int)pressed);
      volatile byte *state = _keystates + i;
      byte currentState = *state;
      if (pressed && !(currentState & _KEY_PRESSED)) {
        currentState |= (_JUST_PRESSED | _KEY_PRESSED);
        evt = KEY_JUST_PRESSED;
        _eventbuf.store_char(evt);
        _eventbuf.store_char(*(_userKeymap + i));
      } else if (!pressed && (currentState & _KEY_PRESSED)) {
        currentState |= _JUST_RELEASED;
        currentState &= ~(_KEY_PRESSED);
        evt = KEY_JUST_RELEASED;
        _eventbuf.store_char(evt);
        _eventbuf.store_char(*(_userKeymap + i));
      }
      *state = currentState;
    }
  }
}

/**************************************************************************/
/*!
    @brief  set all the pin modes and set up variables.
*/
/**************************************************************************/
void Adafruit_I2C_Keypad::begin() {
  _keystates = (volatile byte *)malloc(_numRows * _numCols);
  memset((void *)_keystates, 0, _numRows * _numCols);
  // Set port direction
  _mcp->portMode(_rowPort, OUTPUT);
  _mcp->portMode(_colPort, INPUT_PULLUP);
  _mcp->portPolarity(_colPort, true);
  _mcp->writeGPIO(_rowPort, 0xff); // write 0xff to rows
}

/**************************************************************************/
/*!
    @brief  check if the given key has just been pressed since the last tick.
    @param  key the name of the key to be checked
    @param  clear whether to reset the state (default yes) post-check
    @returns    true if it has been pressed, false otherwise.
*/
/**************************************************************************/
bool Adafruit_I2C_Keypad::justPressed(byte key, bool clear) {
  volatile byte *state = getKeyState(key);
  bool val = (*state & _JUST_PRESSED) != 0;

  if (clear)
    *state &= ~(_JUST_PRESSED);

  return val;
}

/**************************************************************************/
/*!
    @brief  check if the given key has just been released since the last tick.
    @param  key the name of the key to be checked
    @returns    true if it has been released, false otherwise.
*/
/**************************************************************************/
bool Adafruit_I2C_Keypad::justReleased(byte key) {
  volatile byte *state = getKeyState(key);
  bool val = (*state & _JUST_RELEASED) != 0;

  *state &= ~(_JUST_RELEASED);

  return val;
}

/**************************************************************************/
/*!
    @brief  check if the given key is currently pressed
    @param  key the name of the key to be checked
    @returns    true if it is currently pressed, false otherwise.
*/
/**************************************************************************/
bool Adafruit_I2C_Keypad::isPressed(byte key) {
  return (*getKeyState(key) & _KEY_PRESSED) != 0;
}

/**************************************************************************/
/*!
    @brief  check if the given key is currently released
    @param  key the name of the key to be checked
    @returns    true if it is currently released, false otherwise.
*/
/**************************************************************************/
bool Adafruit_I2C_Keypad::isReleased(byte key) {
  return (*getKeyState(key) & _KEY_PRESSED) == 0;
}

/**************************************************************************/
/*!
    @brief  check how many events are in the keypads buffer
    @returns    the number of events currently in the buffer
*/
/**************************************************************************/
int Adafruit_I2C_Keypad::available() {
  return (_eventbuf.available() >> 1);
}

/**************************************************************************/
/*!
    @brief  pop the next event off of the FIFO
    @returns    the next event in the FIFO
*/
/**************************************************************************/
keypadEvent Adafruit_I2C_Keypad::read() {
  keypadEvent k;
  k.bit.EVENT = _eventbuf.read_char();
  k.bit.KEY = _eventbuf.read_char();

  return k;
}

/**************************************************************************/
/*!
    @brief Clear out the event buffer and all the key states
*/
/**************************************************************************/
void Adafruit_I2C_Keypad::clear() {
  _eventbuf.clear();
  for (int i = 0; i < _numRows * _numCols; i++)
    *(_keystates + i) = 0;
}
