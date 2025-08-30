#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

Model* model = NULL;
const int width = 200;
const int height = 200;

void line(Vec2i p0,Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = std::abs(p0.x-p1.x) < std::abs(p0.y-p1.y);
    if (steep)
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
    }
    if (p0.x > p1.x)
    {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }
    int y = p0.y;
    int error = 0;
    for (int x = p0.x; x <= p1.x; x++)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error += 2 * std::abs(p1.y - p0.y);
        if (error> p1.x - p0.x)
        {
            y += p1.y > p0.y ? 1 : -1;
            error -= 2 * (p1.x - p0.x);
        }
    }
}
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    if (t0.y > t1.y)std::swap(t0, t1);
    if (t0.y > t2.y)std::swap(t0, t2);
    if (t1.y > t2.y)std::swap(t1, t2);

    //����t2-t0�����ı���
    int total_height = t2.y - t0.y;

    for (int y = t0.y; y <=t1.y; y++)
    {
        
        int segment_height = t1.y - t0.y + 1;

        //�������ӣ���һ��
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t0.y) / segment_height;

        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;

        image.set(A.x, y, red);
        image.set(B.x,y, green);
    }
}

int main(int argc, char** argv) {
    TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

    triangle(t0[0], t0[1], t0[2], image, red);
    //triangle(t1[0], t1[1], t1[2], image, white);
    //triangle(t2[0], t2[1], t2[2], image, green);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
}
