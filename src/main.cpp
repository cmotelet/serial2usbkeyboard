#include <Arduino.h>
/*
 * Comment out the line for your layout
 */

//#define LAYOUT_US_ENGLISH // Is default
//#define LAYOUT_CANADIAN_FRENCH
//#define LAYOUT_CANADIAN_MULTILINGUAL
//#define LAYOUT_DANISH
//#define LAYOUT_FINNISH
#define LAYOUT_FRENCH
//#define LAYOUT_FRENCH_BELGIAN
//#define LAYOUT_FRENCH_SWISS
//#define LAYOUT_GERMAN
//#define LAYOUT_GERMAN_MAC
//#define LAYOUT_GERMAN_SWISS
//#define LAYOUT_ICELANDIC
//#define LAYOUT_IRISH
//#define LAYOUT_ITALIAN
//#define LAYOUT_NORWEGIAN
//#define LAYOUT_PORTUGUESE
//#define LAYOUT_PORTUGUESE_BRAZILIAN
//#define LAYOUT_SPANISH
//#define LAYOUT_SPANISH_LATIN_AMERICA
//#define LAYOUT_SWEDISH
//#define LAYOUT_TURKISH
//#define LAYOUT_UNITED_KINGDOM
//#define LAYOUT_US_INTERNATIONAL
#include "SoftSerial_INT0.h"
#include "DigiKeyboard.h"
 
#define TX_PIN PB1
#define RX_PIN PB2
#define SERIAL_BAUD_RATE        19200 //Adjust here the serial rate (57600 is the higher supported rate)
 
SoftSerial KBSerial(RX_PIN, TX_PIN);

static uint8_t escCount; // count repetive escape key and if == 3 revert debug mode
static int debugMode;

void typeInHex(uint16_t readKey) {
  if (debugMode)  {
    int8_t i=0;
    uint8_t rem;
    char hexnum[5];
    while(readKey != 0) {
      rem = readKey % 16;
      if(rem < 10)
        rem = rem + 48;
      else
        rem = rem + 55;
      hexnum[i] = rem;
      i++;
      readKey = readKey / 16;
    }
    DigiKeyboard.write("0x");
    for(i = i - 1; i >= 0; i--)
      DigiKeyboard.write(hexnum[i]);
  }
}

void escSeqToKeyStroke(uint16_t keyCode) {
  uint16_t keyStroke;

  switch (keyCode) {
    case 0x0:
      keyStroke = 0;
      break;
    case 0x08: // BS
      keyStroke = KEY_BACKSPACE;
      break;
    case 0x09: // TAB
      keyStroke = KEY_TAB;
      break;
    case 0x0a: // CR
    case 0x0d: // LF
      keyStroke = KEY_ENTER;
      break;
    case 0x1b: // ESC
      keyStroke = KEY_ESC;
      break;
    case 0x32: // INS
      keyStroke = KEY_INSERT;
      break;
    case 0x33: // DEL
      keyStroke = KEY_DELETE;
      break;
    case 0x41: // A
      keyStroke = KEY_UP;
      break;
    case 0x42: // B
      keyStroke = KEY_DOWN;
      break;
    case 0x43: // C
      keyStroke = KEY_RIGHT;
      break;
    case 0x44: // D
      keyStroke = KEY_LEFT;
      break;
    case 0x31: // 1
      keyStroke = KEY_HOME;
      break;
    case 0x34: // 4
      keyStroke = KEY_END;
      break;
    case 0x35: // 5
      keyStroke = KEY_PAGE_UP;
      break;
    case 0x36: // 6
      keyStroke = KEY_PAGE_DOWN;
      break;
    case 0x50: // P
      keyStroke = KEY_F1;
      break;
    case 0x51: // Q
      keyStroke = KEY_F2;
      break;
    case 0x52: // R
      keyStroke = KEY_F3;
      break;
    case 0x53: // S
      keyStroke = KEY_F4;
      break;
    case 0x3136: // 1 6
      keyStroke = KEY_F5;
      break;
    case 0x3137: // 1 7
      keyStroke = KEY_F6;
      break;
    case 0x3138: // 1 8
      keyStroke = KEY_F7;
      break;
    case 0x3139: // 1 9
      keyStroke = KEY_F8;
      break;
    case 0x3230: // 2 0
      keyStroke = KEY_F9;
      break;
    case 0x3231: // 2 1
      keyStroke = KEY_F10;
      break;
    case 0x3233: // 2 3
      keyStroke = KEY_F11;
      break;
    case 0x3234: // 2 3
      keyStroke = KEY_F12;
      break;
    default:
      keyStroke = 0;
      if (debugMode) {
        DigiKeyboard.write("[");
        typeInHex(keyCode);
        DigiKeyboard.write("]");
      }
      break;
  }
  DigiKeyboard.sendKeyStroke(keyStroke);
}

void setup()
{
  KBSerial.begin(SERIAL_BAUD_RATE);
  escCount = 0;
  debugMode = 0;
}

void loop() {
  static uint8_t readKey;
  static uint16_t escSeq;
  static int count;

  count = 0;
  escSeq = 0;
  if(KBSerial.available()) {
    if (debugMode) DigiKeyboard.write("|");
    do {
      readKey = KBSerial.read();
      if (debugMode && count) DigiKeyboard.write(" ");
      typeInHex(readKey);
      switch (count) {
        case 0:
          if (readKey == 0x1b) { // Start escape sequence detected
            count++;
            escCount++;
          }
          break;
        case 1:
          count++; // skip '[' or 'O' in escape sequence
          escCount = 0;
          break;
        case 3:
          if (readKey == 0x7e) break;// skip '~' at end of some escape sequence
          escSeq = escSeq << (8 * (count -2));
        case 2:
          escSeq += readKey;
          count++;
          break;
        default:
         count++; // skip '~' end of some escape sequence
         break;
      }
    } while(KBSerial.available());
    if (debugMode) DigiKeyboard.write("|");
    if (count > 1) { // it's an escape sequence => translate to key
      escSeqToKeyStroke(escSeq);
    } else { // it's an unique key
      if ((escCount == 3) && (readKey == 0x20)) { // 3 escape key followed by 'space' => revert debug mode
        debugMode = !debugMode;
        readKey = 0;
      }
      if (readKey != 0x1b) escCount = 0; // reset escape key counter if not a escape key
      if (readKey < 0x20) { // special translation for key below 0x20
        escSeqToKeyStroke(readKey);
      } else {
        DigiKeyboard.write(readKey);
      }
    }
  }
  DigiKeyboard.update(); // Update the USB connection (maintain alive the connection)
}
