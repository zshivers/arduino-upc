# UPC Barcode Generator

## What can you do with this library?
Generate scannable bar codes! Depending on the input numbers, it can represent:
- Actual product bar codes, which can be scanned by many commercial scanners
- Any random data you want up to 11 decimal digits
  - Can store a full 32-bit number!

My original purpose was to generate automatically scannable timestamps for
testing display frame rates and display latencies. UPC bar codes are fast and
simple to generate and to decode, especially compared to 2D codes.

## What is generated?
All the `generate()` functions give back an array of bytes, all either 0 or 1,
that represent the barcode pattern.

The array returned should be displayed left-to-right.

    barcode[0] = Left-most line
    barcode[upc.lineCount() - 1] = Right-most line

A line is either 0 or 1:

    0 = Space (white)
    1 = Bar   (black)

## UPC details
[UPC Wikipedia Article](https://en.wikipedia.org/wiki/Universal_Product_Code)

This library specifically implements UPC-A. UPC-A has 12 digits in it,
but only 11 are usable. The last digit is used for a checksum, and is
automatically generated.

## Example
This is the minimum code required to generate the barcode pattern. Add your
own code for display of that barcode pattern onto a hardware device.
```c++
#include <UPC.h>
UPC upc;

void setup() {
}

void loop() {
  uint8_t* barcode = upc.generate("01234567890");

  for(uint8_t bar = 0; bar < upc.lineCount(); ++bar) {
    if(barcode[bar]) {
      // display a vertical line
    } else {
      // display background
    }
  }
  delay(100);
}
```

The `examples\` folder includes many useful examples, including ones for the
popular SSD1306 OLED display and even output direct to serial terminal.

## Detection on Digital Displays
If you use a phone to detect the barcode from a screen (like an OLED or TFT
display), you may run into difficulties related to screen tearing, exposure,
low resolution, etc. when trying to detect the barcode. Scanning programs vary
a lot in their ability to detect robustly.

## Optimization
This library is optimized for maximum speed, and therefore takes a decent
amount of RAM for look up tables and the resulting barcode pattern.

You might ask why the library returns a large array of 0s and 1s as bytes rather
than packing them more efficiently in memory. The reasons are:
- Optimized for minimum time from `generate()` to some sort of display
  - Classic tradeoff between computation and memory
- It is simple and fast to access a byte array and do a binary check on each value
- Since the start, end, middle, and each digit all have a number of bits not
  equal to 8, bit packing and unpacking isn't trivial or fast.

### Benchmark
Run your own benchmark to check runtime of `generate()` functions:

    examples\benchmark.ino

Arduino Uno results:

    Running Benchmarks...
    byte array: 69.74 us
    u32: 311.17 us
    string: 87.02 us

Assuming you can find a display capable of it (not easy), you could show
dynamically generated barcodes at update rates well over over __1 KHz__.

## Portability
This library was designed for Arduino, but is portable to other systems.
There are no explicit Arduino dependencies in the core library. The examples
do depend heavily on Arduino.
