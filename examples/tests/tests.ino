/*
 * Unit tests to check the UPC library functionality is correct
 */

#include <UPC.h>

// table of digits and fixed sequences used in UPC
#define START_END 1, 0, 1
#define MIDDLE 0, 1, 0, 1, 0
#define L0 0, 0, 0, 1, 1, 0, 1
#define L1 0, 0, 1, 1, 0, 0, 1
#define L2 0, 0, 1, 0, 0, 1, 1
#define L3 0, 1, 1, 1, 1, 0, 1
#define L4 0, 1, 0, 0, 0, 1, 1
#define L5 0, 1, 1, 0, 0, 0, 1
#define L6 0, 1, 0, 1, 1, 1, 1
#define L7 0, 1, 1, 1, 0, 1, 1
#define L8 0, 1, 1, 0, 1, 1, 1
#define L9 0, 0, 0, 1, 0, 1, 1
#define R0 1, 1, 1, 0, 0, 1, 0
#define R1 1, 1, 0, 0, 1, 1, 0
#define R2 1, 1, 0, 1, 1, 0, 0
#define R3 1, 0, 0, 0, 0, 1, 0
#define R4 1, 0, 1, 1, 1, 0, 0
#define R5 1, 0, 0, 1, 1, 1, 0
#define R6 1, 0, 1, 0, 0, 0, 0
#define R7 1, 0, 0, 0, 1, 0, 0
#define R8 1, 0, 0, 1, 0, 0, 0
#define R9 1, 1, 1, 0, 1, 0, 0

// manually calculated checksums for test sequences used
#define CHK_T0  R0  // digits 00000000000
#define CHK_T1  R5  // digits 01234567890
#define CHK_T2  R9  // digits 67890012345
#define CHK_T3  R7  // digits 00000000001

UPC upc;

void runTest(String testName, bool *testFn(void));
bool compareBarcode(uint8_t* expected, uint8_t* actual);
bool test_allZeros();
bool test_seq1();
bool test_seq2();
bool test_shortInput();
bool test_longInput();
bool test_inputU32();
bool test_inputArray();

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Running Tests...");
  runTest("Digits 00000000000", test_allZeros);
  runTest("Digits 01234567890", test_seq1);
  runTest("Digits 67890012345", test_seq2);
  runTest("Short input", test_shortInput);
  runTest("Long input", test_longInput);
  runTest("u32 input", test_inputU32);
  runTest("Array input", test_inputArray);
  Serial.print("\n\n");
  while(1);
}

void runTest(String testName, bool (*testFn)(void)) {
  bool passed = testFn();
  String result = passed ? String("Passed") : String("Failed");
  Serial.println(String(testName) + String(": ") + result);
}

bool compareBarcode(uint8_t* expected, uint8_t* actual) {
  for(unsigned int i = 0; i < 95; ++i) {
    if(expected[i] != actual[i]) {
      return false;
    }
  }
  return true;
}

// All zeros is a special case for the check digit
bool test_allZeros() {
  const uint8_t expected[95] = {
    START_END,
    L0, L0, L0, L0, L0, L0,
    MIDDLE,
    R0, R0, R0, R0, R0, CHK_T0,
    START_END};
  uint8_t* actual = upc.generate("000000000000");
  return compareBarcode(expected, actual);
}

// The combination of test_seq1 and test_seq2 uses all possible digits
// on the left and right sides of the barcode
bool test_seq1() {
  const uint8_t expected[95] = {
    START_END,
    L0, L1, L2, L3, L4, L5,
    MIDDLE,
    R6, R7, R8, R9, R0, CHK_T1,
    START_END};
  uint8_t* actual = upc.generate("01234567890");
  return compareBarcode(expected, actual);
}

bool test_seq2() {
  const uint8_t expected[95] = {
    START_END,
    L6, L7, L8, L9, L0, L0,
    MIDDLE,
    R1, R2, R3, R4, R5, CHK_T2,
    START_END};
  uint8_t* actual = upc.generate("67890012345");
  return compareBarcode(expected, actual);
}

// Short inputs should be left-padded with zeros
bool test_shortInput() {
  const uint8_t expected[95] = {
    START_END,
    L0, L0, L0, L0, L0, L0,
    MIDDLE,
    R0, R0, R0, R0, R1, CHK_T3,
    START_END};
  uint8_t* actual = upc.generate("1");
  return compareBarcode(expected, actual);
}

// Long inputs should be truncated
bool test_longInput() {
  const uint8_t expected[95] = {
    START_END,
    L0, L1, L2, L3, L4, L5,
    MIDDLE,
    R6, R7, R8, R9, R0, CHK_T1,
    START_END};
  uint8_t* actual = upc.generate("012345678900123456789001234567890");
  return compareBarcode(expected, actual);
}

// test proper conversion and padding of a 32 bit number as the input
bool test_inputU32() {
  const uint8_t expected[95] = {
    START_END,
    L0, L1, L2, L3, L4, L5,
    MIDDLE,
    R6, R7, R8, R9, R0, CHK_T1,
    START_END};
  uint8_t* actual = upc.generate(1234567890ul);
  return compareBarcode(expected, actual);
}

// pass in a fixed length array
bool test_inputArray() {
  const uint8_t expected[95] = {
    START_END,
    L0, L1, L2, L3, L4, L5,
    MIDDLE,
    R6, R7, R8, R9, R0, CHK_T1,
    START_END};
  uint8_t input[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  uint8_t* actual = upc.generate(input);
  return compareBarcode(expected, actual);
}
