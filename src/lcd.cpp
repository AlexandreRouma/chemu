#include "lcd.h"

LCD::LCD(int width, int height, int pixSize, int pixSpacing, int margin, uint32_t bgColor, uint32_t whiteColor, uint32_t blackColor) {
    this->width = width;
    this->height = height;
    this->pixSize = pixSize;
    this->pixSpacing = pixSpacing;
    this->margin = margin;
    this->bgColor = bgColor;
    this->whiteColor = whiteColor;
    this->blackColor = blackColor;

    pixDelta = pixSize + pixSpacing;

    // Compute framebuffer size
    fbWidth = width*pixSize + (width - 1)*pixSpacing + margin*2;
    fbHeight = height*pixSize + (height - 1)*pixSpacing + margin*2;

    // Allocate framebuffer and bitbuffer
    fb = new uint32_t[fbWidth*fbHeight];
    bb = new uint8_t[width*height];

    // Init bitbuffer
    memset(bb, 0, width*height);

    // Fill background
    for (int i = 0; i < fbWidth*fbHeight; i++) {
        fb[i] = bgColor;
    }

    // Initial render
    render(0, 0, width, height);
}

int LCD::getFBWidth() {
    return fbWidth;
}

int LCD::getFBHeight() {
    return fbHeight;
}

const uint32_t* LCD::getFB() {
    return fb;
}

void LCD::blit(int page, int column, const uint8_t* bits, int count) {
    // Blit to bitbuffer
    for (int i = 0; i < count; i++) {
        int col = (column+i)%width;
        int p = (page + (column+i)/width)%(height/8);
        bb[(p*8 + 7)*width + col] = (bits[i] >> 0) & 1;
        bb[(p*8 + 6)*width + col] = (bits[i] >> 1) & 1;
        bb[(p*8 + 5)*width + col] = (bits[i] >> 2) & 1;
        bb[(p*8 + 4)*width + col] = (bits[i] >> 3) & 1;
        bb[(p*8 + 3)*width + col] = (bits[i] >> 4) & 1;
        bb[(p*8 + 2)*width + col] = (bits[i] >> 5) & 1;
        bb[(p*8 + 1)*width + col] = (bits[i] >> 6) & 1;
        bb[(p*8 + 0)*width + col] = (bits[i] >> 7) & 1;
    }

    // Render page (TODO: Can be more optimized)
    render(0, 0, width, height);
}

void LCD::render(int x, int y, int w, int h) {
    int endY = y + h;
    int endX = x + w;
    for (int _y = y; _y < endY; _y++) {
        for (int _x = x; _x < endX; _x++) {
            int xx = _x*pixDelta + margin;
            int yy = _y*pixDelta + margin;

            uint32_t pixColor = bb[_y*width + _x] ? blackColor:whiteColor;

            for (int i = 0; i < pixSize; i++) {
                for (int j = 0; j < pixSize; j++) {
                    fb[(yy+i)*fbWidth + xx+j] = pixColor;
                }
            }
        }
    }
}