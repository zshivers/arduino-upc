/*
 * Show a UPC barcode on a hardware display based on user serial input.
 *
 * Hardware required:
 *  - SSD1306 OLED display using SPI pins
 *
 * Alternatively, you can adapt this code to any display with support
 * from Adafruit's GFX library. Your display must be at least 95 pixels
 * wide or tall to display a UPC barcode.
 */

#include "UPC.h"

// --- Display specific ---
// Hardware connections for Arduino UNO:
//  DC -> Pin 6
//  CS -> Pin 7
//  RES -> Pin 8
//  D0 -> Pin 13
//  D1 -> Pin 11
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
// --- End Display specific ---

UPC upc;

void setup() {
  Serial.begin(115200);

  // --- Display specific ---
  display.begin(SSD1306_SWITCHCAPVCC);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.invertDisplay(true);
  // --- End Display specific ---

  // display an inital default barcode
  String defaultCode = String("01234567890");
  updateUPC(defaultCode);
}

bool stringAcceptable(String& str) {
  // must be a string of non-zero length with only numbers
  if(str.length() == 0) return false;
  for(int i = 0; i < str.length(); ++i)
    if(!isDigit(str[i])) return false;
  return true;
}

void loop() {
  String user_input = Serial.readStringUntil('\n');
  user_input.trim();
  if(stringAcceptable(user_input)) {
    updateUPC(user_input);
  }
  delay(10);
}

void updateUPC(String& input) {
  upc.generate(input.c_str());
  display.clearDisplay();
  drawUPCwithLabel(upc);
  display.display();
}

void drawUPCwithLabel(UPC& upc) {

  // get the height and width of text
  String text = String(upc.text());
  int16_t x1, y1; // not used
  uint16_t text_width, text_height;
  display.getTextBounds(text, 0, 0, &x1, &y1, &text_width, &text_height);

  // calculate the bar code coordinates
  const int barcode_height = 32;
  int barcode_x = (SCREEN_WIDTH - upc.lineCount()) / 2;
  int barcode_y = (SCREEN_HEIGHT - barcode_height - (text_height + 6)) / 2;

  // draw barcode, centered to screen
  // this is done by drawing a vertical bar at each index of the UPC
  // generated pattern array
  uint8_t* pattern = upc.pattern();
  for(int x = 0; x < upc.lineCount(); x++) {
    if(pattern[x] == 1) {
      display.writeFastVLine(x + barcode_x, barcode_y, barcode_height, WHITE);
    }
  }

  // draw text of the number in the barcode, centered under barcode
  int text_y = barcode_y + barcode_height + 3;
  display.setCursor((SCREEN_WIDTH - text_width)/2, text_y);
  display.print(text);
}
