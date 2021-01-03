#pragma once
#include <inttypes.h>

class UPC {
public:
  /**
   * Constructor for UPC class.
   * storeText: if true, store a copy of the digits as a null-terminated string
   *            each time generate() is called.
   */
  UPC(bool storeText = true);

  /**
   * Generate a barcode from BCD digits
   * Example:
   *  uint8_t digits[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
   *  generate(digits);
   * Assumes input length is exactly 11 and only numbers 0-9 are used.
   * This is the fastest function.
   */
  uint8_t* generate(uint8_t digits[11]);

  /**
   * Generate a barcode from a null-terminated string with characters '0' through '9'
   * Example:
   *   generate("012344567890")
   * Only the first 11 characters will be used.
   */
  uint8_t* generate(const char* digits);

  /**
   * Generate a barcode from a 32-bit number.
   * This is best for displaying timestamps from millis() or micros().
   * This is the slowest function due to conversion from uint32 to BCD array.
   * Note that 32-bit numbers can only represent up to 10 decimal digits.
   */
  uint8_t* generate(uint32_t digits);

  /**
   * Return size of barcode array.
   * Use this for loops that iterate over pattern array.
   * Example:
   *  for(uint8_t i = 0; i < upc.lineCount(); ++i) ...
   */
  uint8_t lineCount(void);

  /**
   * Return the text of the digits used in generate(). Can be used to print a
   * label similar to UPC bar codes on products.
   *
   * Notes:
   *  - Will return zero-length string if generate() was never called
   *  - Must have storeText = true in the initializer, which is on by default.
   */
  char* text(void);

  /**
   * Return pointer to 1D array of the barcode pattern.
   * Must call generate() prior to retreving the pattern.
   */
  uint8_t* pattern(void);

private:
  uint8_t* generateInternal(uint8_t* digits);
  uint8_t calcCheckDigit(uint8_t* digits);
  void uint32ToBCD(uint32_t num, uint8_t* out);
  void ASCIIToBCD(char* str, uint8_t* dst);
  uint8_t digitsBCD[12];
  char _text[13];
  uint8_t _pattern[95];
  bool _storeText;
};
