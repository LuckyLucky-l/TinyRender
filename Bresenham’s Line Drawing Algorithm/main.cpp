#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <limits> // 别忘了加这个头文件

const TGAColor white = TGAColor(255, 255, 255, 255); // 白色：B=255, G=255, R=255
const TGAColor red = TGAColor(255, 0, 0, 255); // 红色：R=255 → B=0, G=0, R=255
const TGAColor green = TGAColor(0, 255, 0, 255); // 绿色：G=255 → B=0, G=255, R=0
const TGAColor blue = TGAColor(0, 0, 255, 255); // 蓝色：B=255 → B=255, G=0, R=0
const TGAColor yellow = TGAColor(0, 255, 255, 255); // 黄色：R=255, G=255 → B=0, G=255, R=255


constexpr int width = 800;
constexpr int height = 800;

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) {
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep)
    {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax > bx)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int error = 0;
    for (float x = ax; x < bx; x++) {
        //float t = (x - ax) / static_cast<float>(bx - ax);
        //int y = std::round(ay + (by - ay) * t);
        if (steep)
        {
            framebuffer.set(y, x, color);
        }
        else {

            framebuffer.set(x, y, color);
        }
        error += 2 * std::abs(by - ay);
        if (error > bx - ax)
        {
            y += by > ay ? 1 : -1;
            error -= 2 * (bx - ax);
        }
        //y += (by - ay) / static_cast<float>(bx - ax);
    }

}



int main(int argc, char** argv) {
    Model* model;

    if (2 == argc) {
        model = new Model(argv[1]);  //命令行控制方式构造model
    }
    else {
        model = new Model("../obj/african_head.obj"); //代码方式构造model
    }
    // 计算模型的边界
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    //找到x,y轴的最小值和最大值
    for (int i = 0; i < model->nverts(); i++) {
        Vec3f v = model->vert(i);
        min_x = std::min(min_x, v.x);
        max_x = std::max(max_x, v.x);
        min_y = std::min(min_y, v.y);
        max_y = std::max(max_y, v.y);
    }
    //计算缩放因子，选出 X 和 Y 中跨度较大的那个，作为统一的缩放因子，确保模型在屏幕上不会被拉伸变形
    float span_x = max_x - min_x;
    float span_y = max_y - min_y;
    float span = std::max(span_x, span_y);

    //width - 40 和 height - 40：表示你希望在图像边缘保留 20 像素的边距（左右各 20，总共 40），避免模型贴边。
    float scale = std::min((float)(width - 40) / span, (float)(height - 40) / span);

    //计算偏移量，使得模型居中显示
    float offset_x = (width - scale * span_x) / 2.0f;
    float offset_y = (height - scale * span_y) / 2.0f;


    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++) {
        // 获取当前面的顶点索引列表
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);

            int x0 = offset_x + (v0.x - min_x) * scale;
            int y0 = offset_y + (v0.y - min_y) * scale;
            int x1 = offset_x + (v1.x - min_x) * scale;
            int y1 = offset_y + (v1.y - min_y) * scale;

            line(x0, y0, x1, y1, image, white);
        }
    }


    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}
