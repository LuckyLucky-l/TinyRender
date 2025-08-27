#ifndef __MODEL_H__  // 如果 __MODEL_H__ 没有被定义，则定义
#define __MODEL_H__  // 定义 __MODEL_H__，防止重复包含

#include <vector>    // 包含向量库，用于存储动态数组
#include "geometry.h" // 包含几何库头文件，假设其中定义了 Vec3f 类型

class Model {
private:
    std::vector<Vec3f> verts_;  // 存储模型顶点的向量
    std::vector<std::vector<int>> faces_;  // 存储模型面的顶点索引向量

public:
    Model(const char* filename);  // 构造函数，接受文件名作为参数，用于加载模型数据
    ~Model();  // 析构函数，用于清理模型对象

    int nverts();  // 返回模型中顶点的数量
    int nfaces();  // 返回模型中面的数量

    Vec3f vert(int i);  // 返回指定索引的顶点坐标
    std::vector<int> face(int idx);  // 返回指定索引的面的顶点索引列表
};

#endif // __MODEL_H__  // 结束条件编译，防止重复包含
