#pragma once
#include <stdint.h>
#include <mutex>

class LCD {
public:
    LCD(int width, int height, int pixSize, int pixSpacing, int margin, uint32_t bgColor, uint32_t whiteColor, uint32_t blackColor);

    int getFBWidth();
    int getFBHeight();

    const uint32_t* getFB();

    void blit(int page, int column, const uint8_t* bits, int count);

private:
    void render(int x, int y, int w, int h);

    int width;
    int height;
    int pixSize;
    int pixSpacing;
    int margin;
    int pixDelta;
    int fbWidth;
    int fbHeight;
    uint32_t bgColor, whiteColor, blackColor;

    uint32_t* fb;
    uint8_t* bb;
};