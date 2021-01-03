/*
 * Benchmark the UPC generate() functions
 *
 * Useful for determining the maximum possible framerate for displaying
 * a UPC barcode on your specific hardware. Does not include display hardware
 * refresh time or time taken by graphics library!
 */

#include <UPC.h>

// turn off storing the text of the digits to increase speed
UPC upc(false);

void test_InputByteArray();
void test_InputU32();
void test_InputString();

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Running Benchmarks...");
  runTest("byte array", test_InputByteArray);
  runTest("u32", test_InputU32);
  runTest("string", test_InputString);
  Serial.print("\n\n");
}

void runTest(String testName, void *testFn(void)) {
  const int NUM_TRIALS = 10000;
  unsigned long t_start, t_end;
  float timePerCall;

  t_start = micros();
  for(int trial = 0; trial < NUM_TRIALS; ++trial) {
    testFn();
  }
  t_end = micros();
  timePerCall = (float)(t_end - t_start) / NUM_TRIALS;

  Serial.println(String(testName) + String(": ") +
    String(timePerCall) + String(" us"));
}

void test_InputByteArray() {
  uint8_t code[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  uint8_t* barcode = upc.generate(code);
}
void test_InputU32() {
  uint8_t* barcode = upc.generate(1234567890ul);
}
void test_InputString() {
  uint8_t* barcode = upc.generate("01234567890");
}
