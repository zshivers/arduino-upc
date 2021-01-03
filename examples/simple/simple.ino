/*
 * Simple examples of how to use the UPC library.
 *
 * Hardware required:
 *  - SSD1306 OLED display using SPI pins
 *
 * Alternatively, you can adapt this code to any display with support
 * from Adafruit's GFX library. Your display must be at least 95 pixels
 * wide or tall to display a UPC barcode.
 */

#include <SPI.h>
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
}

void loop() {
  // use a string
  upc.generate("01234567890");
  display.clearDisplay();
  drawUPC(upc);
  display.display();
  delay(2000);

  // use a large number
  upc.generate(1234567890ul); // don't forget the ul at the end
  display.clearDisplay();
  drawUPC(upc);
  display.display();
  delay(2000);

  // use an array of digits
  uint8_t digits[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  upc.generate(1234567890ul); // don't forget the ul at the end
  display.clearDisplay();
  drawUPC(upc);
  display.display();
  delay(2000);
}

void drawUPC(UPC& upc) {

  // calculate the bar code coordinates
  const int barcode_height = SCREEN_HEIGHT;
  int barcode_x = (SCREEN_WIDTH - upc.lineCount()) / 2; // center horizontally
  int barcode_y = 0;

  // draw barcode, centered to screen
  // this is done by drawing a vertical bar at each index of the UPC
  // generated pattern array
  uint8_t* pattern = upc.pattern();
  for(int x = 0; x < upc.lineCount(); x++) {
    if(pattern[x] == 1) {
      display.writeFastVLine(x + barcode_x, barcode_y, barcode_height, WHITE);
    }
  }
}
