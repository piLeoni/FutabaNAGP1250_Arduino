#include "examples_all.h"
#include <math.h>
#include <vector>

// Helper to clear bitmap
static void clear_bitmap(std::vector<uint8_t>& bitmap) {
    std::fill(bitmap.begin(), bitmap.end(), 0);
}

// --------------------------------------------------------------------------
// animated_circle_filling.py (Logic seems to be radial lines in the file I read, 
// but I'll implement what was in the file)
// --------------------------------------------------------------------------
void example_animated_circle_filling(FutabaNAGP1250& vfd) {
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    uint16_t cx = 70;
    uint16_t cy = 16; // Centered vertically
    int angle = 0;
    int step_deg = 5;

    // Run one full rotation
    for (int i = 0; i < 360 / step_deg; ++i) {
        // In the python example it accumulated lines? 
        // "bitmap = vfd.draw_graphic_lines(bitmap=bitmap...)"
        // If it re-uses bitmap without clearing, it accumulates.
        // The python code: "bitmap = [[0...]] ... while True: bitmap = vfd.draw..."
        // Yes, it accumulates.
        
        FutabaNAGP1250::drawGraphicLines(bitmap, width, height, {
            {static_cast<int16_t>(cx), static_cast<int16_t>(cy), static_cast<float>(angle), 10}
        });

        auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
        vfd.displayGraphicImage(packed, width, height);
        
        angle = (angle + step_deg) % 360;
        delay(50); // Animation speed control
    }
}

// --------------------------------------------------------------------------
// animated_pixel_blocks.py
// --------------------------------------------------------------------------
void example_animated_pixel_blocks(FutabaNAGP1250& vfd) {
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    int block_sizes[] = {1, 2, 4};
    int current_size_idx = 0;
    int loop_count = 0;

    // Run for a while
    for (int i = 0; i < 50; ++i) {
        int block_size = block_sizes[current_size_idx];
        
        // Generate random blocks
        for (int y = 0; y < height; y += block_size) {
            for (int x = 0; x < width; x += block_size) {
                uint8_t val = random(2); // 0 or 1
                for (int by = 0; by < block_size && (y + by) < height; ++by) {
                    for (int bx = 0; bx < block_size && (x + bx) < width; ++bx) {
                        bitmap[(y + by) * width + (x + bx)] = val;
                    }
                }
            }
        }

        auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
        vfd.displayGraphicImage(packed, width, height);

        loop_count++;
        if (loop_count >= 5) {
            loop_count = 0;
            current_size_idx = (current_size_idx + 1) % 3;
        }
        delay(100);
    }
}

// --------------------------------------------------------------------------
// animated_radial_lines.py
// --------------------------------------------------------------------------
void example_animated_radial_lines(FutabaNAGP1250& vfd) {
    // This seems very similar to circle filling in the provided code snippet
    // I'll implement a variant that clears the bitmap each time for a "radar" effect
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    uint16_t cx = 70;
    uint16_t cy = 16;
    int angle = 0;

    for (int i = 0; i < 360; i += 5) {
        clear_bitmap(bitmap);
        FutabaNAGP1250::drawGraphicLines(bitmap, width, height, {
            {static_cast<int16_t>(cx), static_cast<int16_t>(cy), static_cast<float>(angle), 30}
        });

        auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
        vfd.displayGraphicImage(packed, width, height);
        angle = (angle + 5) % 360;
        // No delay, run fast
    }
}

// --------------------------------------------------------------------------
// animated_waveforms.py
// --------------------------------------------------------------------------
void example_animated_waveforms(FutabaNAGP1250& vfd) {
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    float phase = 0;
    
    for (int i = 0; i < 100; ++i) {
        clear_bitmap(bitmap);
        // Draw sine wave
        for (int x = 0; x < width; ++x) {
            int y = 16 + static_cast<int>(10 * sin((x * 0.1) + phase));
            if (y >= 0 && y < height) {
                bitmap[y * width + x] = 1;
            }
        }
        
        auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
        vfd.displayGraphicImage(packed, width, height);
        phase += 0.2f;
        delay(20);
    }
}

// --------------------------------------------------------------------------
// circles_lines_circuit_traces.py
// --------------------------------------------------------------------------
void example_circles_lines_circuit_traces(FutabaNAGP1250& vfd) {
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    FutabaNAGP1250::drawGraphicCircle(bitmap, 20, 16, 10, width, height);
    FutabaNAGP1250::drawGraphicCircleFilled(bitmap, 50, 16, 8, width, height);
    
    // Connect them with lines
    FutabaNAGP1250::drawGraphicLines(bitmap, width, height, {
        {30, 16, 0, 20}
    });
    
    auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
    vfd.displayGraphicImage(packed, width, height);
    delay(2000);
}

// --------------------------------------------------------------------------
// graphics_and_text.py
// --------------------------------------------------------------------------
void example_graphics_and_text(FutabaNAGP1250& vfd) {
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    // Draw some framing
    FutabaNAGP1250::drawGraphicLines(bitmap, width, height, {
        {0, 0, 0, 140},
        {0, 31, 0, 140},
        {0, 0, 270, 32},
        {139, 0, 270, 32}
    });
    
    auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
    vfd.displayGraphicImage(packed, width, height);
    
    vfd.setCursorPosition(20, 1);
    vfd.writeText("Graphics & Text");
    delay(2000);
}

// --------------------------------------------------------------------------
// graphics_text_boxes.py
// --------------------------------------------------------------------------
void example_graphics_text_boxes(FutabaNAGP1250& vfd) {
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    FutabaNAGP1250::drawGraphicBox(bitmap, 10, 5, 40, 22, width, height, 3, false);
    FutabaNAGP1250::drawGraphicBox(bitmap, 60, 5, 40, 22, width, height, 0, true);
    
    auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
    vfd.displayGraphicImage(packed, width, height);
    
    vfd.setWriteLogic(FutabaNAGP1250::WRITE_MODE_XOR);
    vfd.setCursorPosition(75, 1);
    vfd.writeText("XOR");
    vfd.setWriteLogic(FutabaNAGP1250::WRITE_MODE_OR);
    
    delay(2000);
}

// --------------------------------------------------------------------------
// graphics_text_dynamic_windows.py
// --------------------------------------------------------------------------
void example_graphics_text_dynamic_windows(FutabaNAGP1250& vfd) {
    vfd.clearWindow(0);
    vfd.defineUserWindow(1, 10, 0, 60, 4);
    vfd.selectWindow(1);
    vfd.writeText("Win 1");
    
    vfd.defineUserWindow(2, 80, 0, 60, 4);
    vfd.selectWindow(2);
    vfd.writeText("Win 2");
    
    delay(1000);
    
    // Move windows (redefine)
    vfd.defineUserWindow(1, 10, 0, 30, 4);
    vfd.selectWindow(1);
    vfd.writeText("Small");
    
    delay(2000);
    vfd.deleteUserWindow(1);
    vfd.deleteUserWindow(2);
    vfd.selectWindow(0);
}

// --------------------------------------------------------------------------
// multiple_graphics_logical_or.py
// --------------------------------------------------------------------------
void example_multiple_graphics_logical_or(FutabaNAGP1250& vfd) {
    vfd.clearWindow(0);
    vfd.setWriteLogic(FutabaNAGP1250::WRITE_MODE_OR);
    
    uint16_t width = 140;
    uint16_t height = 32;
    std::vector<uint8_t> bitmap(width * height, 0);
    
    // Draw something
    FutabaNAGP1250::drawGraphicCircleFilled(bitmap, 40, 16, 15, width, height);
    auto packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
    vfd.displayGraphicImage(packed, width, height);
    
    // Draw something else overlapping
    clear_bitmap(bitmap);
    FutabaNAGP1250::drawGraphicBox(bitmap, 30, 10, 60, 12, width, height, 0, true);
    packed = FutabaNAGP1250::packBitmap(bitmap, width, height);
    
    // Since we are in OR mode, this should merge
    vfd.displayGraphicImage(packed, width, height);
    
    delay(2000);
}

