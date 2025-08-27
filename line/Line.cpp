#include "tgaimage.h"

const TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
const TGAColor green = { 0, 255,   0, 255 };
const TGAColor red = { 0,   0, 255, 255 };
const TGAColor blue = { 255, 128,  64, 255 };
const TGAColor yellow = { 0, 200, 255, 255 };

int main() {
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 7, ay = 3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}