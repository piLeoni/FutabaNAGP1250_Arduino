#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <initializer_list>
#include <vector>

/**
 * Futaba NAGP1250 vacuum fluorescent display driver for Arduino compatible environments.
 *
 * This is a C++ port of the MicroPython driver located in `futaba/NAGP1250.py`.
 * The class focuses on the same public API surface that the MicroPython examples expect
 * (graphics primitives, bitmap packing, logical write modes, window handling, etc.)
 * so sketches can be written in a similar style.
 */
class FutabaNAGP1250 {
public:
    static constexpr uint16_t WIDTH_BASE = 140;
    static constexpr uint16_t WIDTH_EXTENDED = 256;
    static constexpr uint16_t HEIGHT = 32;

    enum WriteMode : uint8_t {
        WRITE_MODE_NORMAL = 0,
        WRITE_MODE_OR = 1,
        WRITE_MODE_AND = 2,
        WRITE_MODE_XOR = 3,
    };

    enum BaseWindowMode : uint8_t {
        BASE_WINDOW_MODE_DEFAULT = 0,
        BASE_WINDOW_MODE_EXTENDED = 1,
    };

    enum FontId : uint8_t {
        FONT_AMERICA = 0x00,
        FONT_FRANCE = 0x01,
        FONT_GERMANY = 0x02,
        FONT_ENGLAND = 0x03,
        FONT_DENMARK_1 = 0x04,
        FONT_SWEDEN = 0x05,
        FONT_ITALY = 0x06,
        FONT_SPAIN_1 = 0x07,
        FONT_JAPAN = 0x08,
        FONT_NORWAY = 0x09,
        FONT_DENMARK_2 = 0x0A,
        FONT_SPAIN_2 = 0x0B,
        FONT_LATIN_AMERICA = 0x0C,
        FONT_KOREA = 0x0D,
    };

    enum CharacterCode : uint8_t {
        CHAR_CODE_PC437 = 0x00,
        CHAR_CODE_KATAKANA = 0x01,
        CHAR_CODE_PC850 = 0x02,
        CHAR_CODE_PC860 = 0x03,
        CHAR_CODE_PC863 = 0x04,
        CHAR_CODE_PC865_NORDIC = 0x05,
        CHAR_CODE_WPC1252 = 0x10,
        CHAR_CODE_PC866 = 0x11,
        CHAR_CODE_PC852 = 0x12,
        CHAR_CODE_PC858 = 0x13,
    };

    struct GraphicLine {
        int16_t x;
        int16_t y;
        float angle_deg;
        uint16_t length;
    };

    FutabaNAGP1250(SPIClass& spiPort = SPI,
                   int8_t resetPin = -1,
                   int8_t sbusyPin = -1,
                   uint32_t spiFrequency = 115200,
                   bool debug = false);

    bool begin(uint8_t baseWindowMode = BASE_WINDOW_MODE_DEFAULT,
               uint8_t luminanceLevel = 4,
               int8_t cursorBlink = 0,
               uint8_t fontId = FONT_AMERICA,
               uint8_t characterCode = CHAR_CODE_PC437);

    void resetDisplay() const;
    void setLuminance(uint8_t level);
    void setCursorBlink(uint8_t mode);
    void setCursorPosition(uint16_t x, uint16_t y);
    void setWriteLogic(uint8_t mode);
    void setModeMD1();
    void setModeMD2();
    void setModeMD3();
    void clearWindow(uint8_t window = 0);
    void defineBaseWindow(uint8_t mode);
    void writeText(const char* text);
    void writeText(const String& text);
    void setFont(uint8_t fontId);
    void setCharacterCode(uint8_t codePage);
    
    // Parity with Python library
    void setHorizontalScrollSpeed(uint8_t speed);
    void setReverseDisplay(uint8_t mode);
    void blinkDisplay(uint8_t pattern, uint8_t normalTime, uint8_t blinkTime, uint8_t repetition);
    void home();
    void lineFeed();
    void backspace();
    void horizontalTab();
    void wait(uint8_t duration);
    void selectWindow(uint8_t windowNum);
    void screenSaver(uint8_t pattern);
    void carriageReturn();
    void displayScroll(uint16_t shiftBytes, uint16_t repeatCount, uint8_t speed);
    void defineUserWindow(uint8_t windowNum, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void deleteUserWindow(uint8_t windowNum, bool clear = true);
    void setFontMagnification(uint8_t h, uint8_t v);
    void setCharacterSpacing(uint8_t mode);


    void displayGraphicImage(const std::vector<uint8_t>& image,
                             uint16_t width,
                             uint16_t height);

    static std::vector<uint8_t> packBitmap(const std::vector<uint8_t>& bitmap,
                                           uint16_t width,
                                           uint16_t height);

    static void drawGraphicLines(std::vector<uint8_t>& bitmap,
                                 uint16_t width,
                                 uint16_t height,
                                 const std::vector<GraphicLine>& lines);

    static void drawGraphicCircle(std::vector<uint8_t>& bitmap, 
                                  uint16_t cx, uint16_t cy, uint16_t radius, 
                                  uint16_t width, uint16_t height);

    static void drawGraphicCircleFilled(std::vector<uint8_t>& bitmap, 
                                        uint16_t cx, uint16_t cy, uint16_t radius, 
                                        uint16_t width, uint16_t height);

    static void drawGraphicBox(std::vector<uint8_t>& bitmap, 
                               uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                               uint16_t bitmapWidth, uint16_t bitmapHeight,
                               uint16_t radius = 0, bool fill = false);

private:
    void initialize();
    void sendBytes(const uint16_t* data, size_t length, bool waitBusy = true);
    void sendBytes(std::initializer_list<uint16_t> list, bool waitBusy = true);
    void waitForBusy(uint32_t timeoutUs = 10000) const;

    SPIClass& spi_;
    SPISettings spiSettings_;
    int8_t resetPin_;
    int8_t sbusyPin_;
    bool debug_;

    uint16_t width_;
    uint16_t height_;
    std::vector<uint8_t> txBuffer_;
};
