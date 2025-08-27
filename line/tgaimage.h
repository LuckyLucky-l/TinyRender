#ifndef __IMAGE_H__

#include <iostream>  // ��������������⣬���ڱ�׼�������
#include <fstream>   // �����ļ����⣬�����ļ��Ķ�д����
#include <string.h>  // �����ַ�������⣬�����ڴ����������memcpy��memset��
#include <time.h>    // ����ʱ��⣬����ʱ����ز�������������δʹ�ã�
#include <math.h>    // ������ѧ�⣬������ѧ��������������δʹ�ã�


// ���ýṹ����뷽ʽΪ1�ֽڶ��룬ȷ���ṹ���Ա��˳��������У�����Ӷ��������ֽ�
#pragma pack(push,1)
struct TGA_Header {
    char idlength;          // ͼ���ʶ������
    char colormaptype;      // ��ɫӳ�������
    char datatypecode;      // �������ʹ���
    short colormaporigin;   // ��ɫӳ����ԭ��
    short colormaplength;   // ��ɫӳ���ĳ���
    char colormapdepth;     // ��ɫӳ������ȣ�ÿ����ɫ������λ����
    short x_origin;         // ͼ������Ļ�ϵ�X��ԭ��
    short y_origin;         // ͼ������Ļ�ϵ�Y��ԭ��
    short width;            // ͼ��Ŀ�ȣ����أ�
    short height;           // ͼ��ĸ߶ȣ����أ�
    char bitsperpixel;      // ÿ�����ص�λ����8, 24, 32�ȣ�
    char imagedescriptor;   // ͼ��������������alphaͨ����������Ϣ
};
#pragma pack(pop)  // �ָ�Ĭ�ϵĶ��뷽ʽ

// ����TGA��ɫ�ṹ��
struct TGAColor {
    union {  // ʹ�������壬ʹ������Ľṹ���Ա�������Ա����ͬһ���ڴ�
        struct {
            unsigned char b, g, r, a;  // �ֱ��ʾ��ɫ����ɫ����ɫ��͸����ͨ����ֵ
        };
        unsigned char raw[4];  // ��������ʽ������ɫ�ĸ���ͨ��
        unsigned int val;      // ��������ʽ������ɫ��ֵ
    };
    int bytespp;  // ��ʾÿ�����ص��ֽ���

    // Ĭ�Ϲ��캯������ʼ����ɫֵΪ0��ÿ�����ص��ֽ���Ϊ1
    TGAColor() : val(0), bytespp(1) {
    }

    // ���캯������ʼ��RGB��Aͨ����ֵ��������ÿ�����ص��ֽ���Ϊ4
    TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {
    }

    // ���캯������ʼ����ɫֵ��ÿ�����ص��ֽ���
    TGAColor(int v, int bpp) : val(v), bytespp(bpp) {
    }

    // ���ƹ��캯�������ڸ�����һ��TGAColor����
    TGAColor(const TGAColor& c) : val(c.val), bytespp(c.bytespp) {
    }

    // �������ʼ����ɫֵ��������ÿ�����ص��ֽ���
    TGAColor(const unsigned char* p, int bpp) : val(0), bytespp(bpp) {
        for (int i = 0; i < bpp; i++) {
            raw[i] = p[i];  // �������е�ÿ��ֵ���Ƶ�raw������
        }
    }

    // ��ֵ����������ڸ�����һ��TGAColor����
    TGAColor& operator=(const TGAColor& c) {
        if (this != &c) {  // ����Ƿ������Ҹ�ֵ
            bytespp = c.bytespp;  // ����ÿ�����ص��ֽ���
            val = c.val;          // ������ɫֵ
        }
        return *this;  // ���ص�ǰ���������
    }
};

// ����TGAͼ������
class TGAImage {
protected:
    unsigned char* data;  // ͼ�����ݻ�����
    int width;            // ͼ��Ŀ�ȣ����أ�
    int height;           // ͼ��ĸ߶ȣ����أ�
    int bytespp;          // ÿ�����ص��ֽ���

    // ��ȡRLE��Run-Length Encoding�������ͼ������
    bool load_rle_data(std::ifstream& in);

    // д��RLE�����ͼ������
    bool unload_rle_data(std::ofstream& out);
public:
    // ����ͼ���ʽö������
    enum Format {
        GRAYSCALE = 1,  // �Ҷ�ͼ��ÿ������1�ֽ�
        RGB = 3,        // RGBͼ��ÿ������3�ֽ�
        RGBA = 4        // RGBAͼ��ÿ������4�ֽڣ�����͸���ȣ�
    };

    // Ĭ�Ϲ��캯������ʼ��ͼ��
    TGAImage();

    // ���캯������ʼ��ָ����С��ÿ�������ֽ�����ͼ��
    TGAImage(int w, int h, int bpp);

    // ���ƹ��캯�������ڸ�����һ��TGAImage����
    TGAImage(const TGAImage& img);

    // ��ָ���ļ���ȡTGAͼ��
    bool read_tga_file(const char* filename);

    // ��ͼ��д��ָ���ļ�������ѡ���Ƿ�ʹ��RLEѹ��
    bool write_tga_file(const char* filename, bool rle = true);

    // ˮƽ��תͼ��
    bool flip_horizontally();

    // ��ֱ��תͼ��
    bool flip_vertically();

    // ����ͼ��ָ���Ŀ�Ⱥ͸߶�
    bool scale(int w, int h);

    // ��ȡָ��λ�õ�������ɫ
    TGAColor get(int x, int y);

    // ����ָ��λ�õ�������ɫ
    bool set(int x, int y, TGAColor c);

    // �����������ͷ�ͼ�����ݻ�����
    ~TGAImage();

    // ��ֵ����������ڸ�����һ��TGAImage����
    TGAImage& operator=(const TGAImage& img);

    // ��ȡͼ��Ŀ��
    int get_width();

    // ��ȡͼ��ĸ߶�
    int get_height();

    // ��ȡÿ�����ص��ֽ���
    int get_bytespp();

    // ��ȡͼ�����ݻ�����
    unsigned char* buffer();

    // ���ͼ�����ݻ�����
    void clear();
};

#endif //__IMAGE_H__
