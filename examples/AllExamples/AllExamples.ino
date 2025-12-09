#include <Arduino.h>
#include <vector>
#include <SPI.h>

#include "FutabaNAGP1250.h"
#include "examples_all.h"

// VSPI defaults on ESP32 dev kits.
// Adjust these pins for your specific board!
#ifdef ESP32
constexpr int PIN_MOSI = 23;
constexpr int PIN_SCK = 18;
constexpr int PIN_RESET = 5;
constexpr int PIN_SBUSY = 35; // Set to -1 if not connected
#else
// Example for generic Arduino (Uno/Nano)
constexpr int PIN_MOSI = 11;
constexpr int PIN_SCK = 13;
constexpr int PIN_RESET = 9;
constexpr int PIN_SBUSY = 8; 
#endif

FutabaNAGP1250 vfd(SPI, PIN_RESET, PIN_SBUSY);

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }

    // Start SPI with custom pins on ESP32, or default on others
    #ifdef ESP32
    SPI.begin(PIN_SCK, -1, PIN_MOSI, -1);
    #else
    SPI.begin();
    #endif

    vfd.begin(FutabaNAGP1250::BASE_WINDOW_MODE_DEFAULT, 4, 0);
    vfd.setCharacterCode(FutabaNAGP1250::CHAR_CODE_PC437);  // PC437 – standard ASCII glyphs.
    vfd.setWriteLogic(FutabaNAGP1250::WRITE_MODE_OR);
    vfd.setCursorPosition(0, 0);
    
    Serial.println("Starting examples...");
}

void loop() {
    Serial.println("Running: Animated Circle Filling");
    vfd.clearWindow(0);
    example_animated_circle_filling(vfd);
    delay(1000);

    Serial.println("Running: Animated Pixel Blocks");
    vfd.clearWindow(0);
    example_animated_pixel_blocks(vfd);
    delay(1000);

    Serial.println("Running: Animated Radial Lines");
    vfd.clearWindow(0);
    example_animated_radial_lines(vfd);
    delay(1000);

    Serial.println("Running: Animated Waveforms");
    vfd.clearWindow(0);
    example_animated_waveforms(vfd);
    delay(1000);

    Serial.println("Running: Circles Lines Circuit Traces");
    vfd.clearWindow(0);
    example_circles_lines_circuit_traces(vfd);
    delay(1000);

    Serial.println("Running: Graphics And Text");
    vfd.clearWindow(0);
    example_graphics_and_text(vfd);
    delay(1000);

    Serial.println("Running: Graphics Text Boxes");
    vfd.clearWindow(0);
    example_graphics_text_boxes(vfd);
    delay(1000);

    Serial.println("Running: Graphics Text Dynamic Windows");
    vfd.clearWindow(0);
    example_graphics_text_dynamic_windows(vfd);
    delay(1000);

    Serial.println("Running: Multiple Graphics Logical OR");
    vfd.clearWindow(0);
    example_multiple_graphics_logical_or(vfd);
    delay(1000);
}

