#ifndef __IMAGE_H__

#include <iostream>  // 包含输入输出流库，用于标准输入输出
#include <fstream>   // 包含文件流库，用于文件的读写操作
#include <string.h>  // 包含字符串处理库，用于内存操作函数如memcpy、memset等
#include <time.h>    // 包含时间库，用于时间相关操作（本代码中未使用）
#include <math.h>    // 包含数学库，用于数学函数（本代码中未使用）


// 设置结构体对齐方式为1字节对齐，确保结构体成员按顺序紧密排列，不添加额外的填充字节
#pragma pack(push,1)
struct TGA_Header {
    char idlength;          // 图像标识符长度
    char colormaptype;      // 颜色映射表类型
    char datatypecode;      // 数据类型代码
    short colormaporigin;   // 颜色映射表的原点
    short colormaplength;   // 颜色映射表的长度
    char colormapdepth;     // 颜色映射表的深度（每个颜色索引的位数）
    short x_origin;         // 图像在屏幕上的X轴原点
    short y_origin;         // 图像在屏幕上的Y轴原点
    short width;            // 图像的宽度（像素）
    short height;           // 图像的高度（像素）
    char bitsperpixel;      // 每个像素的位数（8, 24, 32等）
    char imagedescriptor;   // 图像描述符，包含alpha通道和其他信息
};
#pragma pack(pop)  // 恢复默认的对齐方式

// 定义TGA颜色结构体
struct TGAColor {
    union {  // 使用联合体，使得下面的结构体成员和数组成员共享同一块内存
        struct {
            unsigned char b, g, r, a;  // 分别表示蓝色、绿色、红色和透明度通道的值
        };
        unsigned char raw[4];  // 以数组形式访问颜色的各个通道
        unsigned int val;      // 以整数形式访问颜色的值
    };
    int bytespp;  // 表示每个像素的字节数

    // 默认构造函数，初始化颜色值为0，每个像素的字节数为1
    TGAColor() : val(0), bytespp(1) {
    }

    // 构造函数，初始化RGB和A通道的值，并设置每个像素的字节数为4
    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {
    }

    // 构造函数，初始化颜色值和每个像素的字节数
    TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
    }

    // 复制构造函数，用于复制另一个TGAColor对象
    TGAColor(const TGAColor& c) : val(c.val), bytespp(c.bytespp) {
    }

    // 从数组初始化颜色值，并设置每个像素的字节数
    TGAColor(const unsigned char* p, int bpp) : val(0), bytespp(bpp) {
        for (int i = 0; i < bpp; i++) {
            raw[i] = p[i];  // 将数组中的每个值复制到raw数组中
        }
    }

    // 赋值运算符，用于复制另一个TGAColor对象
    TGAColor& operator=(const TGAColor& c) {
        if (this != &c) {  // 检查是否是自我赋值
            bytespp = c.bytespp;  // 复制每个像素的字节数
            val = c.val;          // 复制颜色值
        }
        return *this;  // 返回当前对象的引用
    }
};

// 定义TGA图像处理类
class TGAImage {
protected:
    unsigned char* data;  // 图像数据缓冲区
    int width;            // 图像的宽度（像素）
    int height;           // 图像的高度（像素）
    int bytespp;          // 每个像素的字节数

    // 读取RLE（Run-Length Encoding）编码的图像数据
    bool load_rle_data(std::ifstream& in);

    // 写入RLE编码的图像数据
    bool unload_rle_data(std::ofstream& out);
public:
    // 定义图像格式枚举类型
    enum Format {
        GRAYSCALE = 1,  // 灰度图像，每个像素1字节
        RGB = 3,        // RGB图像，每个像素3字节
        RGBA = 4        // RGBA图像，每个像素4字节（包含透明度）
    };

    // 默认构造函数，初始化图像
    TGAImage();

    // 构造函数，初始化指定大小和每个像素字节数的图像
    TGAImage(int w, int h, int bpp);

    // 复制构造函数，用于复制另一个TGAImage对象
    TGAImage(const TGAImage& img);

    // 从指定文件读取TGA图像
    bool read_tga_file(const char* filename);

    // 将图像写入指定文件，可以选择是否使用RLE压缩
    bool write_tga_file(const char* filename, bool rle = true);

    // 水平翻转图像
    bool flip_horizontally();

    // 垂直翻转图像
    bool flip_vertically();

    // 缩放图像到指定的宽度和高度
    bool scale(int w, int h);

    // 获取指定位置的像素颜色
    TGAColor get(int x, int y);

    // 设置指定位置的像素颜色
    bool set(int x, int y, TGAColor c);

    // 析构函数，释放图像数据缓冲区
    ~TGAImage();

    // 赋值运算符，用于复制另一个TGAImage对象
    TGAImage& operator=(const TGAImage& img);

    // 获取图像的宽度
    int get_width();

    // 获取图像的高度
    int get_height();

    // 获取每个像素的字节数
    int get_bytespp();

    // 获取图像数据缓冲区
    unsigned char* buffer();

    // 清空图像数据缓冲区
    void clear();
};

#endif //__IMAGE_H__
