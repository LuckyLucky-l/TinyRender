#include "tgaimage.h"  // 包含TGA图像处理类的头文件

// 默认构造函数，初始化图像数据为空，宽度、高度和每个像素的字节数为0
TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) {
}

// 构造函数，初始化指定大小和每个像素字节数的图像
TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp) {
    unsigned long nbytes = width * height * bytespp;  // 计算图像数据的总字节数
    data = new unsigned char[nbytes];  // 分配内存存储图像数据
    memset(data, 0, nbytes);  // 将图像数据初始化为0
}

// 复制构造函数，用于复制另一个TGAImage对象
TGAImage::TGAImage(const TGAImage& img) {
    width = img.width;  // 复制宽度
    height = img.height;  // 复制高度
    bytespp = img.bytespp;  // 复制每个像素的字节数
    unsigned long nbytes = width * height * bytespp;  // 计算图像数据的总字节数
    data = new unsigned char[nbytes];  // 分配内存存储图像数据
    memcpy(data, img.data, nbytes);  // 复制图像数据到新分配的内存中
}

// 析构函数，释放图像数据缓冲区
TGAImage::~TGAImage() {
    if (data) delete[] data;  // 如果图像数据缓冲区不为空，则释放内存
}

// 赋值运算符，用于复制另一个TGAImage对象
TGAImage& TGAImage::operator=(const TGAImage& img) {
    if (this != &img) {  // 检查是否是自我赋值
        if (data) delete[] data;  // 如果当前图像数据缓冲区不为空，则释放内存
        width = img.width;  // 复制宽度
        height = img.height;  // 复制高度
        bytespp = img.bytespp;  // 复制每个像素的字节数
        unsigned long nbytes = width * height * bytespp;  // 计算图像数据的总字节数
        data = new unsigned char[nbytes];  // 分配内存存储图像数据
        memcpy(data, img.data, nbytes);  // 复制图像数据到新分配的内存中
    }
    return *this;  // 返回当前对象的引用
}

// 从指定文件读取TGA图像
bool TGAImage::read_tga_file(const char* filename) {
    if (data) delete[] data;  // 如果当前图像数据缓冲区不为空，则释放内存
    data = NULL;  // 将图像数据缓冲区指针设为NULL
    std::ifstream in;  // 创建文件输入流对象
    in.open(filename, std::ios::binary);  // 打开文件，以二进制模式读取
    if (!in.is_open()) {  // 检查文件是否成功打开
        std::cerr << "can't open file " << filename << "\n";  // 输出错误信息
        in.close();  // 关闭文件
        return false;  // 返回失败
    }
    TGA_Header header;  // 创建TGA头部结构体对象
    in.read((char*)&header, sizeof(header));  // 从文件中读取头部信息
    if (!in.good()) {  // 检查读取操作是否成功
        in.close();  // 关闭文件
        std::cerr << "an error occured while reading the header\n";  // 输出错误信息
        return false;  // 返回失败
    }
    width = header.width;  // 从头部信息中获取宽度
    height = header.height;  // 从头部信息中获取高度
    bytespp = header.bitsperpixel >> 3;  // 计算每个像素的字节数（bitsperpixel右移3位相当于除以8）
    if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
        in.close();  // 关闭文件
        std::cerr << "bad bpp (or width/height) value\n";  // 输出错误信息
        return false;  // 返回失败
    }
    unsigned long nbytes = bytespp * width * height;  // 计算图像数据的总字节数
    data = new unsigned char[nbytes];  // 分配内存存储图像数据
    if (3 == header.datatypecode || 2 == header.datatypecode) {
        in.read((char*)data, nbytes);  // 从文件中读取图像数据
        if (!in.good()) {  // 检查读取操作是否成功
            in.close();  // 关闭文件
            std::cerr << "an error occured while reading the data\n";  // 输出错误信息
            return false;  // 返回失败
        }
    }
    else if (10 == header.datatypecode || 11 == header.datatypecode) {
        if (!load_rle_data(in)) {  // 如果是RLE压缩格式，则调用load_rle_data函数解码
            in.close();  // 关闭文件
            std::cerr << "an error occured while reading the data\n";  // 输出错误信息
            return false;  // 返回失败
        }
    }
    else {
        in.close();  // 关闭文件
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";  // 输出错误信息
        return false;  // 返回失败
    }
    if (!(header.imagedescriptor & 0x20)) {  // 检查图像描述符，判断是否需要垂直翻转
        flip_vertically();  // 垂直翻转图像
    }
    if (header.imagedescriptor & 0x10) {  // 检查图像描述符，判断是否需要水平翻转
        flip_horizontally();  // 水平翻转图像
    }
    std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";  // 输出图像信息
    in.close();  // 关闭文件
    return true;  // 返回成功
}

// 读取RLE（Run-Length Encoding）编码的图像数据
bool TGAImage::load_rle_data(std::ifstream& in) {
    unsigned long pixelcount = width * height;  // 计算图像的总像素数
    unsigned long currentpixel = 0;  // 当前处理的像素数
    unsigned long currentbyte = 0;  // 当前处理的字节数
    TGAColor colorbuffer;  // 创建TGA颜色对象，用于存储读取的颜色数据
    do {
        unsigned char chunkheader = 0;  // 创建块头变量
        chunkheader = in.get();  // 从文件中读取一个字节作为块头
        if (!in.good()) {  // 检查读取操作是否成功
            std::cerr << "an error occured while reading the data\n";  // 输出错误信息
            return false;  // 返回失败
        }
        if (chunkheader < 128) {  // 如果块头小于128，表示这是一个原始数据块
            chunkheader++;  // 原始数据块的实际长度为块头加1
            for (int i = 0; i < chunkheader; i++) {
                in.read((char*)colorbuffer.raw, bytespp);  // 从文件中读取颜色数据
                if (!in.good()) {  // 检查读取操作是否成功
                    std::cerr << "an error occured while reading the header\n";  // 输出错误信息
                    return false;  // 返回失败
                }
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.raw[t];  // 将颜色数据存储到图像数据缓冲区中
                currentpixel++;
                if (currentpixel > pixelcount) {  // 检查是否读取了过多的像素
                    std::cerr << "Too many pixels read\n";  // 输出错误信息
                    return false;  // 返回失败
                }
            }
        }
        else {  // 如果块头大于等于128，表示这是一个RLE压缩块
            chunkheader -= 127;  // RLE压缩块的实际长度为块头减去127
            in.read((char*)colorbuffer.raw, bytespp);  // 从文件中读取颜色数据
            if (!in.good()) {  // 检查读取操作是否成功
                std::cerr << "an error occured while reading the header\n";  // 输出错误信息
                return false;  // 返回失败
            }
            for (int i = 0; i < chunkheader; i++) {
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.raw[t];  // 将颜色数据重复存储到图像数据缓冲区中
                currentpixel++;
                if (currentpixel > pixelcount) {  // 检查是否读取了过多的像素
                    std::cerr << "Too many pixels read\n";  // 输出错误信息
                    return false;  // 返回失败
                }
            }
        }
    } while (currentpixel < pixelcount);  // 循环直到处理完所有像素
    return true;  // 返回成功
}

// 将图像写入指定文件，可以选择是否使用RLE压缩
bool TGAImage::write_tga_file(const char* filename, bool rle) {
    unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };  // 开发者区域引用，通常为0
    unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };  // 扩展区域引用，通常为0
    unsigned char footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };  // 文件尾，固定字符串
    std::ofstream out;  // 创建文件输出流对象
    out.open(filename, std::ios::binary);  // 打开文件，以二进制模式写入
    if (!out.is_open()) {  // 检查文件是否成功打开
        std::cerr << "can't open file " << filename << "\n";  // 输出错误信息
        out.close();  // 关闭文件
        return false;  // 返回失败
    }
    TGA_Header header;  // 创建TGA头部结构体对象
    memset((void*)&header, 0, sizeof(header));  // 初始化头部信息为0
    header.bitsperpixel = bytespp << 3;  // 设置每个像素的位数（bytespp左移3位相当于乘以8）
    header.width = width;  // 设置图像宽度
    header.height = height;  // 设置图像高度
    header.datatypecode = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));  // 设置数据类型代码，根据是否使用RLE压缩和图像格式
    header.imagedescriptor = 0x20;  // 设置图像描述符，表示图像原点在左上角
    out.write((char*)&header, sizeof(header));  // 将头部信息写入文件
    if (!out.good()) {  // 检查写入操作是否成功
        out.close();  // 关闭文件
        std::cerr << "can't dump the tga file\n";  // 输出错误信息
        return false;  // 返回失败
    }
    if (!rle) {
        out.write((char*)data, width * height * bytespp);  // 如果不使用RLE压缩，直接写入原始图像数据
        if (!out.good()) {  // 检查写入操作是否成功
            std::cerr << "can't unload raw data\n";  // 输出错误信息
            out.close();  // 关闭文件
            return false;  // 返回失败
        }
    }
    else {
        if (!unload_rle_data(out)) {  // 如果使用RLE压缩，调用unload_rle_data函数进行压缩并写入
            out.close();  // 关闭文件
            std::cerr << "can't unload rle data\n";  // 输出错误信息
            return false;  // 返回失败
        }
    }
    out.write((char*)developer_area_ref, sizeof(developer_area_ref));  // 写入开发者区域引用
    if (!out.good()) {  // 检查写入操作是否成功
        std::cerr << "can't dump the tga file\n";  // 输出错误信息
        out.close();  // 关闭文件
        return false;  // 返回失败
    }
    out.write((char*)extension_area_ref, sizeof(extension_area_ref));  // 写入扩展区域引用
    if (!out.good()) {  // 检查写入操作是否成功
        std::cerr << "can't dump the tga file\n";  // 输出错误信息
        out.close();  // 关闭文件
        return false;  // 返回失败
    }
    out.write((char*)footer, sizeof(footer));  // 写入文件尾
    if (!out.good()) {  // 检查写入操作是否成功
        std::cerr << "can't dump the tga file\n";  // 输出错误信息
        out.close();  // 关闭文件
        return false;  // 返回失败
    }
    out.close();  // 关闭文件
    return true;  // 返回成功
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TGAImage::unload_rle_data(std::ofstream& out) {
    const unsigned char max_chunk_length = 128;  // 定义最大块长度为128
    unsigned long npixels = width * height;  // 计算图像的总像素数
    unsigned long curpix = 0;  // 当前处理的像素数
    while (curpix < npixels) {  // 循环直到处理完所有像素
        unsigned long chunkstart = curpix * bytespp;  // 当前块的起始字节位置
        unsigned long curbyte = curpix * bytespp;  // 当前处理的字节位置
        unsigned char run_length = 1;  // 当前块的长度
        bool raw = true;  // 标记当前块是否为原始数据块
        while (curpix + run_length < npixels && run_length < max_chunk_length) {
            bool succ_eq = true;  // 标记当前像素是否与下一个像素相同
            for (int t = 0; succ_eq && t < bytespp; t++) {
                succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);  // 比较当前像素和下一个像素的每个通道
            }
            curbyte += bytespp;  // 移动到下一个像素
            if (1 == run_length) {
                raw = !succ_eq;  // 如果是第一个像素且与下一个像素不同，则标记为原始数据块
            }
            if (raw && succ_eq) {
                run_length--;  // 如果是原始数据块且当前像素与下一个像素相同，则减少长度并退出
                break;
            }
            if (!raw && !succ_eq) {
                break;  // 如果是RLE压缩块且当前像素与下一个像素不同，则退出
            }
            run_length++;  // 增加块长度
        }
        curpix += run_length;  // 移动到下一个块的起始位置
        out.put(raw ? run_length - 1 : run_length + 127);  // 写入块头
        if (!out.good()) {  // 检查写入操作是否成功
            std::cerr << "can't dump the tga file\n";  // 输出错误信息
            return false;  // 返回失败
        }
        out.write((char*)(data + chunkstart), (raw ? run_length * bytespp : bytespp));  // 写入图像数据
        if (!out.good()) {  // 检查写入操作是否成功
            std::cerr << "can't dump the tga file\n";  // 输出错误信息
            return false;  // 返回失败
        }
    }
    return true;  // 返回成功
}

// 获取指定位置的像素颜色
TGAColor TGAImage::get(int x, int y) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {  // 检查图像数据缓冲区是否存在以及坐标是否有效
        return TGAColor();  // 返回一个默认颜色对象
    }
    return TGAColor(data + (x + y * width) * bytespp, bytespp);  // 返回指定位置的像素颜色
}

// 设置指定位置的像素颜色
bool TGAImage::set(int x, int y, TGAColor c) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {  // 检查图像数据缓冲区是否存在以及坐标是否有效
        return false;  // 返回失败
    }
    memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);  // 将颜色数据复制到指定位置
    return true;  // 返回成功
}

// 获取每个像素的字节数
int TGAImage::get_bytespp() {
    return bytespp;  // 返回每个像素的字节数
}

// 获取图像的宽度
int TGAImage::get_width() {
    return width;  // 返回图像的宽度
}

// 获取图像的高度
int TGAImage::get_height() {
    return height;  // 返回图像的高度
}

// 水平翻转图像
bool TGAImage::flip_horizontally() {
    if (!data) return false;  // 检查图像数据缓冲区是否存在
    int half = width >> 1;  // 计算图像宽度的一半
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < height; j++) {
            TGAColor c1 = get(i, j);  // 获取左侧像素颜色
            TGAColor c2 = get(width - 1 - i, j);  // 获取右侧像素颜色
            set(i, j, c2);  // 将右侧像素颜色设置到左侧
            set(width - 1 - i, j, c1);  // 将左侧像素颜色设置到右侧
        }
    }
    return true;  // 返回成功
}

// 垂直翻转图像
bool TGAImage::flip_vertically() {
    if (!data) return false;  // 检查图像数据缓冲区是否存在
    unsigned long bytes_per_line = width * bytespp;  // 计算每行的字节数
    unsigned char* line = new unsigned char[bytes_per_line];  // 创建临时缓冲区存储一行图像数据
    int half = height >> 1;  // 计算图像高度的一半
    for (int j = 0; j < half; j++) {
        unsigned long l1 = j * bytes_per_line;  // 当前行的起始字节位置
        unsigned long l2 = (height - 1 - j) * bytes_per_line;  // 对应的倒数第j行的起始字节位置
        memmove((void*)line, (void*)(data + l1), bytes_per_line);  // 将当前行数据复制到临时缓冲区
        memmove((void*)(data + l1), (void*)(data + l2), bytes_per_line);  // 将倒数第j行数据复制到当前行
        memmove((void*)(data + l2), (void*)line, bytes_per_line);  // 将临时缓冲区的数据复制到倒数第j行
    }
    delete[] line;  // 释放临时缓冲区
    return true;  // 返回成功
}

// 获取图像数据缓冲区
unsigned char* TGAImage::buffer() {
    return data;  // 返回图像数据缓冲区的指针
}

// 清空图像数据缓冲区
void TGAImage::clear() {
    memset((void*)data, 0, width * height * bytespp);  // 将图像数据缓冲区的所有字节设置为0
}

// 缩放图像到指定的宽度和高度
bool TGAImage::scale(int w, int h) {
    if (w <= 0 || h <= 0 || !data) return false;  // 检查目标宽度、高度是否有效以及图像数据缓冲区是否存在
    unsigned char* tdata = new unsigned char[w * h * bytespp];  // 分配内存存储缩放后的图像数据
    int nscanline = 0;  // 当前目标扫描线的起始位置
    int oscanline = 0;  // 当前原始扫描线的起始位置
    int erry = 0;  // 垂直方向的误差
    unsigned long nlinebytes = w * bytespp;  // 目标图像每行的字节数
    unsigned long olinebytes = width * bytespp;  // 原始图像每行的字节数
    for (int j = 0; j < height; j++) {
        int errx = width - w;  // 水平方向的误差
        int nx = -bytespp;  // 当前目标像素的起始位置
        int ox = -bytespp;  // 当前原始像素的起始位置
        for (int i = 0; i < width; i++) {
            ox += bytespp;  // 移动到下一个原始像素
            errx += w;  // 增加水平方向的误差
            while (errx >= (int)width) {  // 如果水平方向的误差大于等于图像宽度
                errx -= width;  // 减去图像宽度
                nx += bytespp;  // 移动到下一个目标像素
                memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);  // 复制原始像素到目标像素
            }
        }
        erry += h;  // 增加垂直方向的误差
        oscanline += olinebytes;  // 移动到下一行原始像素
        while (erry >= (int)height) {  // 如果垂直方向的误差大于等于图像高度
            if (erry >= (int)height << 1)  // 如果误差大于等于两倍高度，表示需要跳过一行
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);  // 复制当前扫描线到下一行
            erry -= height;  // 减去图像高度
            nscanline += nlinebytes;  // 移动到下一行目标像素
        }
    }
    delete[] data;  // 释放原始图像数据缓冲区
    data = tdata;  // 将目标图像数据缓冲区赋值给当前对象
    width = w;  // 更新宽度
    height = h;  // 更新高度
    return true;  // 返回成功
}
