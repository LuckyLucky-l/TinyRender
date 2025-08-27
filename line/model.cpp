#include <iostream>    // 包含标准输入输出流库
#include <string>      // 包含字符串库
#include <fstream>     // 包含文件流库，用于文件的读写操作
#include <sstream>     // 包含字符串流库，用于字符串的读写操作
#include <vector>      // 包含向量库，用于存储动态数组
#include "model.h"    // 包含自定义的模型头文件

// Model 类的构造函数，接受一个文件名作为参数，并初始化 verts_ 和 faces_ 为空
Model::Model(const char* filename) : verts_(), faces_() {
    std::ifstream in;   // 创建一个文件输入流对象
    in.open(filename, std::ifstream::in);  // 打开指定的文件
    if (in.fail()) return;  // 如果文件打开失败，直接返回，不进行后续操作

    std::string line;  // 用于存储从文件中读取的每一行
    while (!in.eof()) {  // 当文件未结束时，循环读取每一行
        std::getline(in, line);  // 读取一行并存储到 line 中
        std::istringstream iss(line.c_str());  // 创建一个字符串输入流对象，用于解析 line
        char trash;  // 用于存储解析过程中不需要的字符（如空格、换行符等）

        // 如果当前行是以 "v " 开头的，表示这是一个顶点信息行
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;  // 读取并丢弃 "v " 前面的字符
            Vec3f v;  // 创建一个 Vec3f 类型的对象来存储顶点坐标
            for (int i = 0; i < 3; i++) iss >> v.raw[i];  // 依次读取 x, y, z 坐标并存储到 v 中
            verts_.push_back(v);  // 将顶点添加到 verts_ 向量中
        }
        // 如果当前行是以 "f " 开头的，表示这是一个面信息行
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;  // 创建一个向量来存储面的顶点索引
            int itrash, idx;  // idx 用于存储顶点索引，itrash 用于存储不需要的字符（如纹理坐标索引）
            iss >> trash;  // 读取并丢弃 "f " 前面的字符
            // 循环读取面的顶点索引，格式为 "v/vt/vn"（顶点/纹理坐标/法线）
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--;  // OBJ 文件中顶点索引从 1 开始，C++ 中数组索引从 0 开始，因此需要减 1
                f.push_back(idx);  // 将顶点索引添加到 f 向量中
            }
            faces_.push_back(f);  // 将面的顶点索引向量添加到 faces_ 向量中
        }
    }
    // 输出顶点和面的数量，用于调试
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

// Model 类的析构函数
Model::~Model() {
    // 这里没有显式清理资源，假设 Vec3f 和 faces_ 可以自动清理
}

// 返回模型中顶点的数量
int Model::nverts() {
    return (int)verts_.size();  // 返回 verts_ 向量的大小
}

// 返回模型中面的数量
int Model::nfaces() {
    return (int)faces_.size();  // 返回 faces_ 向量的大小
}

// 返回指定索引的面的顶点索引列表
std::vector<int> Model::face(int idx) {
    return faces_[idx];  // 返回 faces_ 向量中指定索引的元素
}

// 返回指定索引的顶点坐标
Vec3f Model::vert(int i) {
    return verts_[i];  // 返回 verts_ 向量中指定索引的元素
}
