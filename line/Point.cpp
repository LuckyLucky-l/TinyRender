//#include "tgaimage.h"  // 包含TGA图像处理类的头文件
//// 定义常量颜色对象，白色
//const TGAColor white = TGAColor(255, 255, 255, 255);
//
//// 定义常量颜色对象，红色
//const TGAColor red = TGAColor(255, 0, 0, 255);
//
//// 定义常量颜色对象，绿色
//const TGAColor green = TGAColor(0, 255, 0, 255);
//
//// 定义常量颜色对象，蓝色
//const TGAColor blue = TGAColor(0, 0, 255, 255);
//
//// 主函数，程序的入口点
//int main(int argc, char** argv) {
//    // 创建一个1000x1000像素的RGB格式的TGA图像对象
//    TGAImage image(100, 100, TGAImage::RGB);
//
//    // 设置图像中坐标(800, 800)的像素为白色
//    image.set(40, 50, white);
//
//    image.set(60, 80, red);
//
//    image.set(10, 10, green);
//
//    image.set(90, 90, blue);
//    // 垂直翻转图像，使得原点在左下角
//    image.flip_vertically();  // i want to have the origin at the left bottom corner of the image
//
//    // 将图像写入文件"output.tga"
//    image.write_tga_file("output.tga");
//
//    return 0;  // 返回0，表示程序成功结束
//}
