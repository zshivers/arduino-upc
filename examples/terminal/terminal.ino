/*
 * Print out a UPC barcode to the serial terminal using Unicode block characters.
 *
 * Surprisingly, several smart phone barcode scanners can detect this despite
 * Arduino's serial monitor putting blank space between each line.
 */

#include "UPC.h"

UPC upc;

bool stringAcceptable(String& str);
void printUPC(UPC& upc);

void setup() {
  Serial.begin(115200);
  Serial.println(F("Enter 11 digits to print a UPC barcode to the terminal"));
}

void loop() {
  String user_input = Serial.readStringUntil('\n');
  user_input.trim();
  if(stringAcceptable(user_input)) {
    upc.generate(user_input.c_str());
    printUPC(upc);
  }
  delay(10);
}

bool stringAcceptable(String& str) {
  // must be a string of non-zero length with only numbers
  if(str.length() == 0) return false;
  for(int i = 0; i < str.length(); ++i)
    if(!isDigit(str[i])) return false;
  return true;
}

#define SPACE ' '
#define BLOCK u8"\u2588"

void printUPC(UPC& upc) {
  const unsigned int HEIGHT = 8;
  uint8_t* pattern = upc.pattern();
  Serial.println(upc.text());
  for(unsigned int v = 0; v < HEIGHT; ++v) {
    for(unsigned int x = 0; x < upc.lineCount(); x++) {
      if(pattern[x]) Serial.print(BLOCK);
      else           Serial.print(SPACE);
    }
    Serial.println("");
  }
}
