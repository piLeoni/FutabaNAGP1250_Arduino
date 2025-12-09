#include "FutabaNAGP1250.h"

#include <math.h>

// Define PI if not defined (Arduino usually defines PI)
#ifndef PI
#define PI 3.14159265358979323846f
#endif


FutabaNAGP1250::FutabaNAGP1250(SPIClass& spiPort,
                               int8_t resetPin,
                               int8_t sbusyPin,
                               uint32_t spiFrequency,
                               bool debug)
    : spi_(spiPort),
      spiSettings_(spiFrequency, LSBFIRST, SPI_MODE0),
      resetPin_(resetPin),
      sbusyPin_(sbusyPin),
      debug_(debug),
      width_(WIDTH_BASE),
      height_(HEIGHT) {}

bool FutabaNAGP1250::begin(uint8_t baseWindowMode,
                           uint8_t luminanceLevel,
                           int8_t cursorBlink,
                           uint8_t fontId,
                           uint8_t characterCode) {
    spi_.begin();

    if (resetPin_ >= 0) {
        pinMode(resetPin_, OUTPUT);
    }

    if (sbusyPin_ >= 0) {
        pinMode(sbusyPin_, INPUT);
    }

    resetDisplay();
    initialize();
    defineBaseWindow(baseWindowMode);
    setLuminance(luminanceLevel);
    setFont(fontId);
    setCharacterCode(characterCode);

    if (cursorBlink >= 0) {
        setCursorBlink(static_cast<uint8_t>(cursorBlink));
    }

    return true;
}

void FutabaNAGP1250::resetDisplay() const {
    if (resetPin_ < 0) {
        return;
    }

    digitalWrite(resetPin_, HIGH);
    delay(1);
    digitalWrite(resetPin_, LOW);
    delay(100);
    digitalWrite(resetPin_, HIGH);
    delay(100);
}

void FutabaNAGP1250::initialize() {
    sendBytes({0x1B, 0x40});
}

void FutabaNAGP1250::setLuminance(uint8_t level) {
    level = constrain(level, static_cast<uint8_t>(1), static_cast<uint8_t>(8));
    sendBytes({0x1F, 0x58, level});
}

void FutabaNAGP1250::setCursorBlink(uint8_t mode) {
    mode = constrain(mode, static_cast<uint8_t>(0), static_cast<uint8_t>(1));
    sendBytes({0x1F, 0x43, mode});
}

void FutabaNAGP1250::setCursorPosition(uint16_t x, uint16_t y) {
    if (x > 255 || y > 3) {
        return;
    }
    sendBytes({0x1F, 0x24, static_cast<uint16_t>(x & 0xFF), static_cast<uint16_t>((x >> 8) & 0xFF),
               static_cast<uint16_t>(y & 0xFF), static_cast<uint16_t>((y >> 8) & 0xFF)});
}

void FutabaNAGP1250::setWriteLogic(uint8_t mode) {
    mode = constrain(mode, static_cast<uint8_t>(0), static_cast<uint8_t>(3));
    sendBytes({0x1F, 0x77, mode});
}

void FutabaNAGP1250::setModeMD1() {
    sendBytes({0x1F, 0x01});
}

void FutabaNAGP1250::setModeMD2() {
    sendBytes({0x1F, 0x02});
}

void FutabaNAGP1250::setModeMD3() {
    sendBytes({0x1F, 0x03});
}

void FutabaNAGP1250::clearWindow(uint8_t window) {
    if (window > 4) {
        return;
    }
    sendBytes({0x1F, 0x28, 0x77, 0x01, window});
    sendBytes({0x0C});
}

void FutabaNAGP1250::defineBaseWindow(uint8_t mode) {
    mode = constrain(mode, static_cast<uint8_t>(0), static_cast<uint8_t>(1));
    width_ = (mode == 0) ? WIDTH_BASE : WIDTH_EXTENDED;
    height_ = HEIGHT;
    sendBytes({0x1F, 0x28, 0x77, 0x10, mode});
}

void FutabaNAGP1250::writeText(const char* text) {
    if (!text) {
        return;
    }
    std::vector<uint16_t> payload;
    payload.reserve(strlen(text));
    while (*text) {
        payload.push_back(static_cast<uint8_t>(*text++));
    }
    sendBytes(payload.data(), payload.size());
}

void FutabaNAGP1250::writeText(const String& text) {
    std::vector<uint16_t> payload;
    payload.reserve(text.length());
    for (size_t i = 0; i < text.length(); ++i) {
        payload.push_back(static_cast<uint8_t>(text[i]));
    }
    sendBytes(payload.data(), payload.size());
}

void FutabaNAGP1250::setFont(uint8_t fontId) {
    if (fontId > 0x0D) {
        return;
    }
    sendBytes({0x1B, 0x52, fontId});
}

void FutabaNAGP1250::setCharacterCode(uint8_t codePage) {
    sendBytes({0x1B, 0x74, codePage});
}

void FutabaNAGP1250::setHorizontalScrollSpeed(uint8_t speed) {
    speed = constrain(speed, static_cast<uint8_t>(0), static_cast<uint8_t>(31));
    sendBytes({0x1F, 0x73, speed});
}

void FutabaNAGP1250::setReverseDisplay(uint8_t mode) {
    mode = constrain(mode, static_cast<uint8_t>(0), static_cast<uint8_t>(1));
    sendBytes({0x1F, 0x72, mode});
}

void FutabaNAGP1250::blinkDisplay(uint8_t pattern, uint8_t normalTime, uint8_t blinkTime, uint8_t repetition) {
    if (pattern > 2 || normalTime < 1 || blinkTime < 1 || repetition < 1) {
        return; // Basic validation
    }
    sendBytes({0x1F, 0x28, 0x61, 0x11, pattern, normalTime, blinkTime, repetition});
}

void FutabaNAGP1250::home() {
    sendBytes({0x0B});
}

void FutabaNAGP1250::lineFeed() {
    sendBytes({0x0A});
}

void FutabaNAGP1250::backspace() {
    sendBytes({0x08});
}

void FutabaNAGP1250::horizontalTab() {
    sendBytes({0x09});
}

void FutabaNAGP1250::wait(uint8_t duration) {
    sendBytes({0x1F, 0x28, 0x61, 0x01, duration});
}

void FutabaNAGP1250::selectWindow(uint8_t windowNum) {
    if (windowNum > 4) return;
    sendBytes({0x1F, 0x28, 0x77, 0x01, windowNum});
}

void FutabaNAGP1250::screenSaver(uint8_t pattern) {
    if (pattern > 4) return;
    sendBytes({0x1F, 0x28, 0x61, 0x40, pattern});
}

void FutabaNAGP1250::carriageReturn() {
    sendBytes({0x0D});
}

void FutabaNAGP1250::displayScroll(uint16_t shiftBytes, uint16_t repeatCount, uint8_t speed) {
    if (shiftBytes > 1023 || repeatCount < 1 || speed > 255) return;
    
    uint8_t wl = shiftBytes & 0xFF;
    uint8_t wh = (shiftBytes >> 8) & 0xFF;
    uint8_t cl = repeatCount & 0xFF;
    uint8_t ch = (repeatCount >> 8) & 0xFF;

    std::vector<uint16_t> payload = {
        0x1F, 0x28, 0x61, 0x10,
        static_cast<uint16_t>(wl), static_cast<uint16_t>(wh),
        static_cast<uint16_t>(cl), static_cast<uint16_t>(ch),
        static_cast<uint16_t>(speed)
    };
    sendBytes(payload.data(), payload.size(), true);
}

void FutabaNAGP1250::defineUserWindow(uint8_t windowNum, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (windowNum < 1 || windowNum > 4) return;
    if (x > 279 || y > 3) return;
    if (w < 1 || w > 280 || h < 1 || h > 4) return;

    std::vector<uint16_t> payload = {
        0x1F, 0x28, 0x77, 0x02,
        static_cast<uint16_t>(windowNum),
        0x01,
        static_cast<uint16_t>(x & 0xFF), static_cast<uint16_t>((x >> 8) & 0xFF),
        static_cast<uint16_t>(y & 0xFF), static_cast<uint16_t>((y >> 8) & 0xFF),
        static_cast<uint16_t>(w & 0xFF), static_cast<uint16_t>((w >> 8) & 0xFF),
        static_cast<uint16_t>(h & 0xFF), static_cast<uint16_t>((h >> 8) & 0xFF)
    };
    sendBytes(payload.data(), payload.size());
}

void FutabaNAGP1250::deleteUserWindow(uint8_t windowNum, bool clear) {
    if (windowNum < 1 || windowNum > 4) return;
    if (clear) {
        clearWindow(windowNum);
    }
    sendBytes({0x1F, 0x28, 0x77, 0x02, static_cast<uint16_t>(windowNum), 0x00});
}

void FutabaNAGP1250::setFontMagnification(uint8_t h, uint8_t v) {
    if (h < 1 || h > 4 || v < 1 || v > 4) return;
    sendBytes({0x1F, 0x28, 0x67, 0x40, h, v});
}

void FutabaNAGP1250::setCharacterSpacing(uint8_t mode) {
    if (mode > 3) return;
    sendBytes({0x1F, 0x28, 0x67, 0x03, mode});
}


void FutabaNAGP1250::displayGraphicImage(const std::vector<uint8_t>& image,
                                         uint16_t width,
                                         uint16_t height) {
    if (width == 0 || width > WIDTH_EXTENDED) {
        return;
    }
    if (height == 0 || height > HEIGHT || (height % 8) != 0) {
        return;
    }

    const uint16_t byteRows = height / 8;
    if (image.size() != static_cast<size_t>(width * byteRows)) {
        return;
    }

    std::vector<uint16_t> header = {
        0x1F,
        0x28,
        0x66,
        0x11,
        static_cast<uint16_t>(width & 0xFF),
        static_cast<uint16_t>((width >> 8) & 0xFF),
        static_cast<uint16_t>(byteRows & 0xFF),
        static_cast<uint16_t>((byteRows >> 8) & 0xFF),
        0x01,
    };

    // Perform a single SPI transaction for the entire packet (Header + Image)
    // to ensure continuity and correct CS handling if managed externally.
    spi_.beginTransaction(spiSettings_);
    
    // Send Header
    for (uint16_t item : header) {
        if (sbusyPin_ >= 0) {
            while (digitalRead(sbusyPin_) == HIGH) {}
        }
        spi_.transfer(static_cast<uint8_t>(item));
        if (sbusyPin_ < 0) delayMicroseconds(400);
    }

    // Send Image Data
    for (uint8_t byte : image) {
        if (sbusyPin_ >= 0) {
            while (digitalRead(sbusyPin_) == HIGH) {}
        }
        spi_.transfer(byte);
        if (sbusyPin_ < 0) delayMicroseconds(400);
    }
    
    spi_.endTransaction();
    waitForBusy();
}

std::vector<uint8_t> FutabaNAGP1250::packBitmap(const std::vector<uint8_t>& bitmap,
                                                uint16_t width,
                                                uint16_t height) {
    std::vector<uint8_t> packed;
    if (bitmap.size() != static_cast<size_t>(width * height)) {
        return packed;
    }

    packed.reserve(static_cast<size_t>(width) * ((height + 7) / 8));
    for (uint16_t x = 0; x < width; ++x) {
        for (uint16_t row = 0; row < height; row += 8) {
            uint8_t byte = 0;
            for (uint8_t bit = 0; bit < 8 && (row + bit) < height; ++bit) {
                const uint16_t index = (row + bit) * width + x;
                if (bitmap[index]) {
                    byte |= (1 << (7 - bit));
                }
            }
            packed.push_back(byte);
        }
    }
    return packed;
}

void FutabaNAGP1250::drawGraphicLines(std::vector<uint8_t>& bitmap,
                                      uint16_t width,
                                      uint16_t height,
                                      const std::vector<GraphicLine>& lines) {
    for (const auto& line : lines) {
        const float angleRad = line.angle_deg * DEG_TO_RAD;
        const float degX = cosf(angleRad);
        const float degY = -sinf(angleRad);
        for (uint16_t i = 0; i < line.length; ++i) {
            const int16_t x = static_cast<int16_t>(roundf(line.x + degX * i));
            const int16_t y = static_cast<int16_t>(roundf(line.y + degY * i));
            if (x >= 0 && x < static_cast<int16_t>(width) && y >= 0 && y < static_cast<int16_t>(height)) {
                bitmap[y * width + x] = 1;
            }
        }
    }
}

void FutabaNAGP1250::drawGraphicCircle(std::vector<uint8_t>& bitmap, 
                                       uint16_t cx, uint16_t cy, uint16_t radius, 
                                       uint16_t width, uint16_t height) {
    int16_t x = radius;
    int16_t y = 0;
    int16_t d = 1 - radius;

    auto plot = [&](int16_t px, int16_t py) {
        if (px >= 0 && px < width && py >= 0 && py < height) {
            bitmap[py * width + px] = 1;
        }
    };

    while (x >= y) {
        plot(cx + x, cy + y);
        plot(cx + y, cy + x);
        plot(cx - y, cy + x);
        plot(cx - x, cy + y);
        plot(cx - x, cy - y);
        plot(cx - y, cy - x);
        plot(cx + y, cy - x);
        plot(cx + x, cy - y);
        y++;
        if (d < 0) {
            d += 2 * y + 1;
        } else {
            x--;
            d += 2 * (y - x) + 1;
        }
    }
}

void FutabaNAGP1250::drawGraphicCircleFilled(std::vector<uint8_t>& bitmap, 
                                             uint16_t cx, uint16_t cy, uint16_t radius, 
                                             uint16_t width, uint16_t height) {
    int16_t x = radius;
    int16_t y = 0;
    int16_t d = 1 - radius;

    auto drawSpan = [&](int16_t yOffset, int16_t xLeft, int16_t xRight) {
        int16_t yPos = cy + yOffset;
        if (yPos >= 0 && yPos < height) {
            for (int16_t xPos = cx - xLeft; xPos <= cx + xRight; ++xPos) {
                if (xPos >= 0 && xPos < width) {
                    bitmap[yPos * width + xPos] = 1;
                }
            }
        }
    };

    while (x >= y) {
        drawSpan(y, x, x);
        drawSpan(-y, x, x);
        drawSpan(x, y, y);
        drawSpan(-x, y, y);
        y++;
        if (d < 0) {
            d += 2 * y + 1;
        } else {
            x--;
            d += 2 * (y - x) + 1;
        }
    }
}

void FutabaNAGP1250::drawGraphicBox(std::vector<uint8_t>& bitmap, 
                                    uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
                                    uint16_t bitmapWidth, uint16_t bitmapHeight,
                                    uint16_t radius, bool fill) {
    if (bitmap.size() < static_cast<size_t>(bitmapWidth * bitmapHeight)) return;

    int16_t x0 = constrain(x, 0, bitmapWidth - 1);
    int16_t x1 = constrain(x + w - 1, 0, bitmapWidth - 1);
    int16_t y0 = constrain(y, 0, bitmapHeight - 1);
    int16_t y1 = constrain(y + h - 1, 0, bitmapHeight - 1);
    
    int16_t actualW = x1 - x0;
    int16_t actualH = y1 - y0;
    if (actualW < 0 || actualH < 0) return;

    radius = constrain(radius, 0, min(actualW / 2, actualH / 2));
    if (radius < 1) radius = 1; // Ensure at least minimal radius logic applies, though 0 is passed for sharp corners
    // Python logic: radius = max(1, min(radius, min((x1 - x0) // 2, (y1 - y0) // 2)))
    // If radius passed is 0, logic should handle sharp box.
    // Python logic forces radius to be at least 1.
    radius = max(1, min(static_cast<int>(radius), min(actualW / 2, actualH / 2)));

    // Fill interior rectangle (excluding corners)
    if (fill) {
        for (int16_t yi = y0 + radius; yi <= y1 - radius; ++yi) {
            for (int16_t xi = x0 + 1; xi < x1; ++xi) { // Python: range(x0 + 1, x1)
                 if (yi >= 0 && yi < bitmapHeight && xi >= 0 && xi < bitmapWidth) bitmap[yi * bitmapWidth + xi] = 1;
            }
        }
        for (int16_t yi = y0 + 1; yi < y0 + radius; ++yi) {
            for (int16_t xi = x0 + radius; xi <= x1 - radius; ++xi) { // Python: range(x0 + radius, x1 - radius + 1)
                 if (yi >= 0 && yi < bitmapHeight && xi >= 0 && xi < bitmapWidth) bitmap[yi * bitmapWidth + xi] = 1;
            }
        }
        for (int16_t yi = y1 - radius + 1; yi < y1; ++yi) {
            for (int16_t xi = x0 + radius; xi <= x1 - radius; ++xi) {
                 if (yi >= 0 && yi < bitmapHeight && xi >= 0 && xi < bitmapWidth) bitmap[yi * bitmapWidth + xi] = 1;
            }
        }
    }

    // Horizontal edges
    for (int16_t xi = x0 + radius; xi <= x1 - radius; ++xi) {
        if (xi >= 0 && xi < bitmapWidth) {
            if (y0 >= 0 && y0 < bitmapHeight) bitmap[y0 * bitmapWidth + xi] = 1;
            if (y1 >= 0 && y1 < bitmapHeight) bitmap[y1 * bitmapWidth + xi] = 1;
        }
    }

    // Vertical edges
    for (int16_t yi = y0 + radius; yi <= y1 - radius; ++yi) {
        if (yi >= 0 && yi < bitmapHeight) {
            if (x0 >= 0 && x0 < bitmapWidth) bitmap[yi * bitmapWidth + x0] = 1;
            if (x1 >= 0 && x1 < bitmapWidth) bitmap[yi * bitmapWidth + x1] = 1;
        }
    }

    // Corners
    int16_t steps = radius * 2;
    for (int16_t i = 0; i <= steps; ++i) {
        float theta = (PI / 2.0f) * ((float)i / steps);
        int16_t dx = static_cast<int16_t>(round(radius * cos(theta)));
        int16_t dy = static_cast<int16_t>(round(radius * sin(theta)));

        // Top-left
        int16_t px = x0 + radius - dx;
        int16_t py = y0 + radius - dy;
        if (px >= 0 && px < bitmapWidth && py >= 0 && py < bitmapHeight) bitmap[py * bitmapWidth + px] = 1;
        if (fill) {
            for (int16_t fy = py + 1; fy < y0 + radius; ++fy) {
                if (fy >= 0 && fy < bitmapHeight) bitmap[fy * bitmapWidth + px] = 1;
            }
        }

        // Top-right
        px = x1 - radius + dx;
        py = y0 + radius - dy;
        if (px >= 0 && px < bitmapWidth && py >= 0 && py < bitmapHeight) bitmap[py * bitmapWidth + px] = 1;
        if (fill) {
            for (int16_t fy = py + 1; fy < y0 + radius; ++fy) {
                if (fy >= 0 && fy < bitmapHeight) bitmap[fy * bitmapWidth + px] = 1;
            }
        }

        // Bottom-left
        px = x0 + radius - dx;
        py = y1 - radius + dy;
        if (px >= 0 && px < bitmapWidth && py >= 0 && py < bitmapHeight) bitmap[py * bitmapWidth + px] = 1;
        if (fill) {
            for (int16_t fy = y1 - radius + 1; fy < py; ++fy) {
                if (fy >= 0 && fy < bitmapHeight) bitmap[fy * bitmapWidth + px] = 1;
            }
        }

        // Bottom-right
        px = x1 - radius + dx;
        py = y1 - radius + dy;
        if (px >= 0 && px < bitmapWidth && py >= 0 && py < bitmapHeight) bitmap[py * bitmapWidth + px] = 1;
        if (fill) {
            for (int16_t fy = y1 - radius + 1; fy < py; ++fy) {
                if (fy >= 0 && fy < bitmapHeight) bitmap[fy * bitmapWidth + px] = 1;
            }
        }
    }
}

void FutabaNAGP1250::sendBytes(const uint16_t* data, size_t length, bool waitBusy) {
    if (!data || !length) {
        return;
    }

    txBuffer_.clear();
    txBuffer_.reserve(length * 2);

    for (size_t i = 0; i < length; ++i) {
        const uint16_t item = data[i];
        if (item <= 0xFF) {
            txBuffer_.push_back(item & 0xFF);
        } else {
            const uint8_t low = item & 0xFF;
            const uint8_t high = (item >> 8) & 0xFF;
            txBuffer_.push_back(low);
            txBuffer_.push_back(high);
        }
    }

    spi_.beginTransaction(spiSettings_);
    for (uint8_t byte : txBuffer_) {
        if (sbusyPin_ >= 0) {
            while (digitalRead(sbusyPin_) == HIGH) {}
        }
        spi_.transfer(byte);
        // VFDs can be slow to process bytes, especially in Read-Modify-Write modes (OR/AND/XOR).
        // If SBUSY is NOT connected (-1), we must delay 400us to prevent overflow.
        if (sbusyPin_ < 0) {
             delayMicroseconds(400); 
        }
    }
    spi_.endTransaction();

    if (waitBusy) {
        waitForBusy();
    }
}

void FutabaNAGP1250::sendBytes(std::initializer_list<uint16_t> list, bool waitBusy) {
    sendBytes(list.begin(), list.size(), waitBusy);
}

void FutabaNAGP1250::waitForBusy(uint32_t timeoutUs) const {
    if (sbusyPin_ < 0) {
        return;
    }

    const uint32_t start = micros();
    while (digitalRead(sbusyPin_)) {
        if (micros() - start > timeoutUs) {
            if (debug_) {
                Serial.println(F("WARNING: SBUSY timeout"));
            }
            break;
        }
        delayMicroseconds(10);
    }
}
