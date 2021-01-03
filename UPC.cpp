#include "UPC.h"
#include <string.h>
#include <stdlib.h>

// maximum number of user supplied digits
// the actual number of digits in a UPC barcode is 12, which includes
// the check digit
const uint8_t MAX_DIGITS = 11;

// number of lines (single vertical bars) per digit
const uint8_t LINES_PER_DIGIT = 7;

// number of lines in fixed start sequence
const uint8_t START_SIZE = 3;

// number of lines in fixed middle sequence
const uint8_t MID_SIZE = 5;

// number of lines in fixed end sequence
const uint8_t END_SIZE = 3;

// look up table for digits on the left side (odd parity)
const uint8_t digit_table_l[10][LINES_PER_DIGIT] = {
  {0, 0, 0, 1, 1, 0, 1}, // 0
  {0, 0, 1, 1, 0, 0, 1}, // 1
  {0, 0, 1, 0, 0, 1, 1}, // 2
  {0, 1, 1, 1, 1, 0, 1}, // 3
  {0, 1, 0, 0, 0, 1, 1}, // 4
  {0, 1, 1, 0, 0, 0, 1}, // 5
  {0, 1, 0, 1, 1, 1, 1}, // 6
  {0, 1, 1, 1, 0, 1, 1}, // 7
  {0, 1, 1, 0, 1, 1, 1}, // 8
  {0, 0, 0, 1, 0, 1, 1}, // 9
};

// look up table for digits on right side (even parity)
const uint8_t digit_table_r[10][LINES_PER_DIGIT] = {
  {1, 1, 1, 0, 0, 1, 0}, // 0
  {1, 1, 0, 0, 1, 1, 0}, // 1
  {1, 1, 0, 1, 1, 0, 0}, // 2
  {1, 0, 0, 0, 0, 1, 0}, // 3
  {1, 0, 1, 1, 1, 0, 0}, // 4
  {1, 0, 0, 1, 1, 1, 0}, // 5
  {1, 0, 1, 0, 0, 0, 0}, // 6
  {1, 0, 0, 0, 1, 0, 0}, // 7
  {1, 0, 0, 1, 0, 0, 0}, // 8
  {1, 1, 1, 0, 1, 0, 0}, // 9
};

UPC::UPC(bool storeText = true) : _storeText(storeText) {
  // initialize the fixed sequences that never change with new input digits
  _pattern[0] = 1;   // start pattern
  _pattern[2] = 1;
  _pattern[46] = 1;  // middle pattern
  _pattern[48] = 1;
  _pattern[92] = 1;  // end pattern
  _pattern[94] = 1;

  // init text to zero-length
  _text[0] = '\0';
}

uint8_t* UPC::generate(uint8_t digits[11]) {
  return generateInternal(digits);
}

uint8_t* UPC::generate(const char* digits) {
  ASCIIToBCD(digits, digitsBCD);
  return generateInternal(digitsBCD);
}

uint8_t* UPC::generate(uint32_t num) {
  uint32ToBCD(num, digitsBCD);
  return generateInternal(digitsBCD);
}

char* UPC::text(void) {
  return _text;
}

uint8_t* UPC::pattern(void) {
  return _pattern;
}

uint8_t UPC::lineCount(void) {
  return (MAX_DIGITS + 1) * LINES_PER_DIGIT + START_SIZE + MID_SIZE + END_SIZE;
}

uint8_t* UPC::generateInternal(uint8_t* digits) {

  uint8_t checkDigit = calcCheckDigit(digits);

  // left group of 6 digits begins after start pattern
  uint8_t* digblock1_ptr = _pattern + START_SIZE;

  // right group of 6 digits begins after first group and middle pattern
  uint8_t* digblock2_ptr = digblock1_ptr + LINES_PER_DIGIT*6 + MID_SIZE;

  // memcpy is used because it is faster than manual loop copy
  memcpy(digblock1_ptr + 0*7, digit_table_l[digits[0]], LINES_PER_DIGIT);
  memcpy(digblock1_ptr + 1*7, digit_table_l[digits[1]], LINES_PER_DIGIT);
  memcpy(digblock1_ptr + 2*7, digit_table_l[digits[2]], LINES_PER_DIGIT);
  memcpy(digblock1_ptr + 3*7, digit_table_l[digits[3]], LINES_PER_DIGIT);
  memcpy(digblock1_ptr + 4*7, digit_table_l[digits[4]], LINES_PER_DIGIT);
  memcpy(digblock1_ptr + 5*7, digit_table_l[digits[5]], LINES_PER_DIGIT);

  memcpy(digblock2_ptr + 0*7, digit_table_r[digits[6]],  LINES_PER_DIGIT);
  memcpy(digblock2_ptr + 1*7, digit_table_r[digits[7]],  LINES_PER_DIGIT);
  memcpy(digblock2_ptr + 2*7, digit_table_r[digits[8]],  LINES_PER_DIGIT);
  memcpy(digblock2_ptr + 3*7, digit_table_r[digits[9]],  LINES_PER_DIGIT);
  memcpy(digblock2_ptr + 4*7, digit_table_r[digits[10]], LINES_PER_DIGIT);
  memcpy(digblock2_ptr + 5*7, digit_table_r[checkDigit], LINES_PER_DIGIT);

  if(_storeText) {
    for(unsigned int i = 0; i < MAX_DIGITS; ++i) {
      _text[i] = digits[i] + '0';
    }
    _text[MAX_DIGITS] = checkDigit + '0';
    _text[MAX_DIGITS + 1] = '\0';
  }

  return _pattern;
}

/**
 * UPC check digit calculation. See Wikipedia for algorithm.
 */
uint8_t UPC::calcCheckDigit(uint8_t* digits) {
  uint16_t check;
  check  = 3*(digits[0] + digits[2] + digits[4] + digits[6] + digits[8] + digits[10]);
  check += digits[1] + digits[3] + digits[5] + digits[7] + digits[9];
  check %= 10;
  return (check == 0) ? 0 : 10 - check;
}

/**
 * Convert 32 bit number to a string, then string to padded BCD.
 * Using the ultoa() is considerably faster than a simple implementation
 * to convert a number to base 10 array using mod's and divide's
 */
void UPC::uint32ToBCD(uint32_t num, uint8_t* out) {
  char str[MAX_DIGITS];
  ultoa(num, str, 10);
  ASCIIToBCD(str, out);
}

/**
 * Convert an ASCII string with only characters '0' through '9' to
 * a zero-padded BCD array.
 */
void UPC::ASCIIToBCD(char* str, uint8_t* dst) {
    uint8_t zeros;
    uint8_t len = strnlen(str, MAX_DIGITS);
    zeros = MAX_DIGITS - len;

    for(uint8_t i = 0; i < MAX_DIGITS; ++i) {
      if(i < zeros)
        dst[i] = 0;
      else
        dst[i] = str[i-zeros] - '0';
    }
}
