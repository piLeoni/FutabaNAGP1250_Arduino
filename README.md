# Futaba NAGP1250 Arduino Library

![Video Stream Demo](https://raw.githubusercontent.com/piLeoni/FutabaNAGP1250_Arduino/refs/heads/main/video_stream.gif)

A high-performance Arduino/PlatformIO driver for the **Futaba NAGP1250** Vacuum Fluorescent Display (VFD).

This library is a C++ port of the [MicroPython driver](https://github.com/AlmightyOatmeal/MicroPython_Futaba_NAGP1250) by Catlin Kintsugi, optimized for speed and hardware flow control.

## Features
- **High-Speed Graphics:** Logic-optimized for streaming animations (circles, lines, boxes).
- **Hardware Flow Control:** Supports the `SBUSY` pin for maximum frame rate without corruption.
- **Text & Windows:** Full support for hardware text layers, windowing, and scrolling.
- **Robust SPI:** Uses `LSBFIRST` and correct timing to ensure glitch-free operation on ESP32 and other MCUs.

## Wiring (ESP32 Example)

| VFD Pin | ESP32 Pin | Description |
|---------|-----------|-------------|
| **SIN** | GPIO 23   | MOSI (Data) |
| **CLK** | GPIO 18   | SCK (Clock) |
| **RES** | GPIO 5    | Reset       |
| **BSY** | GPIO 35   | SBUSY (Flow Control) |
| **GND** | GND       | Ground      |
| **VCC** | 5V        | Power       |

> **Note:** The `SBUSY` connection is highly recommended. If connected, the library automatically detects it and switches to a high-speed polling mode, allowing for smooth 30+ FPS animations. If disconnected, it falls back to safe timing delays (~400µs per byte).

## Getting Started

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps =
    FutabaNAGP1250
```

### Basic Usage
```cpp
#include <FutabaNAGP1250.h>

// Pin Definitions
#define PIN_MOSI  23
#define PIN_SCK   18
#define PIN_RESET 5
#define PIN_SBUSY 35

FutabaNAGP1250 vfd(SPI, PIN_RESET, PIN_SBUSY);

void setup() {
    SPI.begin(PIN_SCK, -1, PIN_MOSI, -1);
    
    // Initialize VFD (140x32 resolution)
    vfd.begin();
    vfd.setLuminance(8); // Max brightness
    
    // Text
    vfd.setCursorPosition(0, 0);
    vfd.writeText("Hello World!");
    
    // Graphics
    std::vector<uint8_t> bitmap(140 * 32 / 8, 0); // Packed buffer
    // ... draw to bitmap ...
    vfd.displayGraphicImage(bitmap, 140, 32);
}

void loop() {
}
```

## Streaming & Performance
For video or fast animations, ensure you connect the **SBUSY** pin. The library utilizes a tight polling loop to synchronize perfectly with the VFD's processing speed, eliminating buffer overflows and visual corruption while maximizing throughput.

## Advanced Examples

### Video Streaming (Node.js + ESP32)
For a high-performance video streaming demonstration (30FPS with compression and dithering), check out the companion project:
[**FutabaNAGP1250_Stream**](https://github.com/pleoni/FutabaNAGP1250_Stream)
