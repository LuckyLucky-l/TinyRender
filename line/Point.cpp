//#include "tgaimage.h"  // ����TGAͼ�������ͷ�ļ�
//// ���峣����ɫ���󣬰�ɫ
//const TGAColor white = TGAColor(255, 255, 255, 255);
//
//// ���峣����ɫ���󣬺�ɫ
//const TGAColor red = TGAColor(255, 0, 0, 255);
//
//// ���峣����ɫ������ɫ
//const TGAColor green = TGAColor(0, 255, 0, 255);
//
//// ���峣����ɫ������ɫ
//const TGAColor blue = TGAColor(0, 0, 255, 255);
//
//// ���������������ڵ�
//int main(int argc, char** argv) {
//    // ����һ��1000x1000���ص�RGB��ʽ��TGAͼ�����
//    TGAImage image(100, 100, TGAImage::RGB);
//
//    // ����ͼ��������(800, 800)������Ϊ��ɫ
//    image.set(40, 50, white);
//
//    image.set(60, 80, red);
//
//    image.set(10, 10, green);
//
//    image.set(90, 90, blue);
//    // ��ֱ��תͼ��ʹ��ԭ�������½�
//    image.flip_vertically();  // i want to have the origin at the left bottom corner of the image
//
//    // ��ͼ��д���ļ�"output.tga"
//    image.write_tga_file("output.tga");
//
//    return 0;  // ����0����ʾ����ɹ�����
//}
