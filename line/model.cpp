#include <iostream>    // ������׼�����������
#include <string>      // �����ַ�����
#include <fstream>     // �����ļ����⣬�����ļ��Ķ�д����
#include <sstream>     // �����ַ������⣬�����ַ����Ķ�д����
#include <vector>      // ���������⣬���ڴ洢��̬����
#include "model.h"    // �����Զ����ģ��ͷ�ļ�

// Model ��Ĺ��캯��������һ���ļ�����Ϊ����������ʼ�� verts_ �� faces_ Ϊ��
Model::Model(const char* filename) : verts_(), faces_() {
    std::ifstream in;   // ����һ���ļ�����������
    in.open(filename, std::ifstream::in);  // ��ָ�����ļ�
    if (in.fail()) return;  // ����ļ���ʧ�ܣ�ֱ�ӷ��أ������к�������

    std::string line;  // ���ڴ洢���ļ��ж�ȡ��ÿһ��
    while (!in.eof()) {  // ���ļ�δ����ʱ��ѭ����ȡÿһ��
        std::getline(in, line);  // ��ȡһ�в��洢�� line ��
        std::istringstream iss(line.c_str());  // ����һ���ַ����������������ڽ��� line
        char trash;  // ���ڴ洢���������в���Ҫ���ַ�����ո񡢻��з��ȣ�

        // �����ǰ������ "v " ��ͷ�ģ���ʾ����һ��������Ϣ��
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;  // ��ȡ������ "v " ǰ����ַ�
            Vec3f v;  // ����һ�� Vec3f ���͵Ķ������洢��������
            for (int i = 0; i < 3; i++) iss >> v.raw[i];  // ���ζ�ȡ x, y, z ���겢�洢�� v ��
            verts_.push_back(v);  // ��������ӵ� verts_ ������
        }
        // �����ǰ������ "f " ��ͷ�ģ���ʾ����һ������Ϣ��
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;  // ����һ���������洢��Ķ�������
            int itrash, idx;  // idx ���ڴ洢����������itrash ���ڴ洢����Ҫ���ַ�������������������
            iss >> trash;  // ��ȡ������ "f " ǰ����ַ�
            // ѭ����ȡ��Ķ�����������ʽΪ "v/vt/vn"������/��������/���ߣ�
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--;  // OBJ �ļ��ж��������� 1 ��ʼ��C++ ������������ 0 ��ʼ�������Ҫ�� 1
                f.push_back(idx);  // ������������ӵ� f ������
            }
            faces_.push_back(f);  // ����Ķ�������������ӵ� faces_ ������
        }
    }
    // ��������������������ڵ���
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

// Model �����������
Model::~Model() {
    // ����û����ʽ������Դ������ Vec3f �� faces_ �����Զ�����
}

// ����ģ���ж��������
int Model::nverts() {
    return (int)verts_.size();  // ���� verts_ �����Ĵ�С
}

// ����ģ�����������
int Model::nfaces() {
    return (int)faces_.size();  // ���� faces_ �����Ĵ�С
}

// ����ָ����������Ķ��������б�
std::vector<int> Model::face(int idx) {
    return faces_[idx];  // ���� faces_ ������ָ��������Ԫ��
}

// ����ָ�������Ķ�������
Vec3f Model::vert(int i) {
    return verts_[i];  // ���� verts_ ������ָ��������Ԫ��
}
