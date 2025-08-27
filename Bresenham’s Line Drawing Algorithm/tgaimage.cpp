#include "tgaimage.h"  // ����TGAͼ�������ͷ�ļ�

// Ĭ�Ϲ��캯������ʼ��ͼ������Ϊ�գ���ȡ��߶Ⱥ�ÿ�����ص��ֽ���Ϊ0
TGAImage::TGAImage() : data(NULL), width(0), height(0), bytespp(0) {
}

// ���캯������ʼ��ָ����С��ÿ�������ֽ�����ͼ��
TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp) {
    unsigned long nbytes = width * height * bytespp;  // ����ͼ�����ݵ����ֽ���
    data = new unsigned char[nbytes];  // �����ڴ�洢ͼ������
    memset(data, 0, nbytes);  // ��ͼ�����ݳ�ʼ��Ϊ0
}

// ���ƹ��캯�������ڸ�����һ��TGAImage����
TGAImage::TGAImage(const TGAImage& img) {
    width = img.width;  // ���ƿ��
    height = img.height;  // ���Ƹ߶�
    bytespp = img.bytespp;  // ����ÿ�����ص��ֽ���
    unsigned long nbytes = width * height * bytespp;  // ����ͼ�����ݵ����ֽ���
    data = new unsigned char[nbytes];  // �����ڴ�洢ͼ������
    memcpy(data, img.data, nbytes);  // ����ͼ�����ݵ��·�����ڴ���
}

// �����������ͷ�ͼ�����ݻ�����
TGAImage::~TGAImage() {
    if (data) delete[] data;  // ���ͼ�����ݻ�������Ϊ�գ����ͷ��ڴ�
}

// ��ֵ����������ڸ�����һ��TGAImage����
TGAImage& TGAImage::operator=(const TGAImage& img) {
    if (this != &img) {  // ����Ƿ������Ҹ�ֵ
        if (data) delete[] data;  // �����ǰͼ�����ݻ�������Ϊ�գ����ͷ��ڴ�
        width = img.width;  // ���ƿ��
        height = img.height;  // ���Ƹ߶�
        bytespp = img.bytespp;  // ����ÿ�����ص��ֽ���
        unsigned long nbytes = width * height * bytespp;  // ����ͼ�����ݵ����ֽ���
        data = new unsigned char[nbytes];  // �����ڴ�洢ͼ������
        memcpy(data, img.data, nbytes);  // ����ͼ�����ݵ��·�����ڴ���
    }
    return *this;  // ���ص�ǰ���������
}

// ��ָ���ļ���ȡTGAͼ��
bool TGAImage::read_tga_file(const char* filename) {
    if (data) delete[] data;  // �����ǰͼ�����ݻ�������Ϊ�գ����ͷ��ڴ�
    data = NULL;  // ��ͼ�����ݻ�����ָ����ΪNULL
    std::ifstream in;  // �����ļ�����������
    in.open(filename, std::ios::binary);  // ���ļ����Զ�����ģʽ��ȡ
    if (!in.is_open()) {  // ����ļ��Ƿ�ɹ���
        std::cerr << "can't open file " << filename << "\n";  // ���������Ϣ
        in.close();  // �ر��ļ�
        return false;  // ����ʧ��
    }
    TGA_Header header;  // ����TGAͷ���ṹ�����
    in.read((char*)&header, sizeof(header));  // ���ļ��ж�ȡͷ����Ϣ
    if (!in.good()) {  // ����ȡ�����Ƿ�ɹ�
        in.close();  // �ر��ļ�
        std::cerr << "an error occured while reading the header\n";  // ���������Ϣ
        return false;  // ����ʧ��
    }
    width = header.width;  // ��ͷ����Ϣ�л�ȡ���
    height = header.height;  // ��ͷ����Ϣ�л�ȡ�߶�
    bytespp = header.bitsperpixel >> 3;  // ����ÿ�����ص��ֽ�����bitsperpixel����3λ�൱�ڳ���8��
    if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
        in.close();  // �ر��ļ�
        std::cerr << "bad bpp (or width/height) value\n";  // ���������Ϣ
        return false;  // ����ʧ��
    }
    unsigned long nbytes = bytespp * width * height;  // ����ͼ�����ݵ����ֽ���
    data = new unsigned char[nbytes];  // �����ڴ�洢ͼ������
    if (3 == header.datatypecode || 2 == header.datatypecode) {
        in.read((char*)data, nbytes);  // ���ļ��ж�ȡͼ������
        if (!in.good()) {  // ����ȡ�����Ƿ�ɹ�
            in.close();  // �ر��ļ�
            std::cerr << "an error occured while reading the data\n";  // ���������Ϣ
            return false;  // ����ʧ��
        }
    }
    else if (10 == header.datatypecode || 11 == header.datatypecode) {
        if (!load_rle_data(in)) {  // �����RLEѹ����ʽ�������load_rle_data��������
            in.close();  // �ر��ļ�
            std::cerr << "an error occured while reading the data\n";  // ���������Ϣ
            return false;  // ����ʧ��
        }
    }
    else {
        in.close();  // �ر��ļ�
        std::cerr << "unknown file format " << (int)header.datatypecode << "\n";  // ���������Ϣ
        return false;  // ����ʧ��
    }
    if (!(header.imagedescriptor & 0x20)) {  // ���ͼ�����������ж��Ƿ���Ҫ��ֱ��ת
        flip_vertically();  // ��ֱ��תͼ��
    }
    if (header.imagedescriptor & 0x10) {  // ���ͼ�����������ж��Ƿ���Ҫˮƽ��ת
        flip_horizontally();  // ˮƽ��תͼ��
    }
    std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";  // ���ͼ����Ϣ
    in.close();  // �ر��ļ�
    return true;  // ���سɹ�
}

// ��ȡRLE��Run-Length Encoding�������ͼ������
bool TGAImage::load_rle_data(std::ifstream& in) {
    unsigned long pixelcount = width * height;  // ����ͼ�����������
    unsigned long currentpixel = 0;  // ��ǰ�����������
    unsigned long currentbyte = 0;  // ��ǰ������ֽ���
    TGAColor colorbuffer;  // ����TGA��ɫ�������ڴ洢��ȡ����ɫ����
    do {
        unsigned char chunkheader = 0;  // ������ͷ����
        chunkheader = in.get();  // ���ļ��ж�ȡһ���ֽ���Ϊ��ͷ
        if (!in.good()) {  // ����ȡ�����Ƿ�ɹ�
            std::cerr << "an error occured while reading the data\n";  // ���������Ϣ
            return false;  // ����ʧ��
        }
        if (chunkheader < 128) {  // �����ͷС��128����ʾ����һ��ԭʼ���ݿ�
            chunkheader++;  // ԭʼ���ݿ��ʵ�ʳ���Ϊ��ͷ��1
            for (int i = 0; i < chunkheader; i++) {
                in.read((char*)colorbuffer.raw, bytespp);  // ���ļ��ж�ȡ��ɫ����
                if (!in.good()) {  // ����ȡ�����Ƿ�ɹ�
                    std::cerr << "an error occured while reading the header\n";  // ���������Ϣ
                    return false;  // ����ʧ��
                }
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.raw[t];  // ����ɫ���ݴ洢��ͼ�����ݻ�������
                currentpixel++;
                if (currentpixel > pixelcount) {  // ����Ƿ��ȡ�˹��������
                    std::cerr << "Too many pixels read\n";  // ���������Ϣ
                    return false;  // ����ʧ��
                }
            }
        }
        else {  // �����ͷ���ڵ���128����ʾ����һ��RLEѹ����
            chunkheader -= 127;  // RLEѹ�����ʵ�ʳ���Ϊ��ͷ��ȥ127
            in.read((char*)colorbuffer.raw, bytespp);  // ���ļ��ж�ȡ��ɫ����
            if (!in.good()) {  // ����ȡ�����Ƿ�ɹ�
                std::cerr << "an error occured while reading the header\n";  // ���������Ϣ
                return false;  // ����ʧ��
            }
            for (int i = 0; i < chunkheader; i++) {
                for (int t = 0; t < bytespp; t++)
                    data[currentbyte++] = colorbuffer.raw[t];  // ����ɫ�����ظ��洢��ͼ�����ݻ�������
                currentpixel++;
                if (currentpixel > pixelcount) {  // ����Ƿ��ȡ�˹��������
                    std::cerr << "Too many pixels read\n";  // ���������Ϣ
                    return false;  // ����ʧ��
                }
            }
        }
    } while (currentpixel < pixelcount);  // ѭ��ֱ����������������
    return true;  // ���سɹ�
}

// ��ͼ��д��ָ���ļ�������ѡ���Ƿ�ʹ��RLEѹ��
bool TGAImage::write_tga_file(const char* filename, bool rle) {
    unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };  // �������������ã�ͨ��Ϊ0
    unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };  // ��չ�������ã�ͨ��Ϊ0
    unsigned char footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };  // �ļ�β���̶��ַ���
    std::ofstream out;  // �����ļ����������
    out.open(filename, std::ios::binary);  // ���ļ����Զ�����ģʽд��
    if (!out.is_open()) {  // ����ļ��Ƿ�ɹ���
        std::cerr << "can't open file " << filename << "\n";  // ���������Ϣ
        out.close();  // �ر��ļ�
        return false;  // ����ʧ��
    }
    TGA_Header header;  // ����TGAͷ���ṹ�����
    memset((void*)&header, 0, sizeof(header));  // ��ʼ��ͷ����ϢΪ0
    header.bitsperpixel = bytespp << 3;  // ����ÿ�����ص�λ����bytespp����3λ�൱�ڳ���8��
    header.width = width;  // ����ͼ����
    header.height = height;  // ����ͼ��߶�
    header.datatypecode = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));  // �����������ʹ��룬�����Ƿ�ʹ��RLEѹ����ͼ���ʽ
    header.imagedescriptor = 0x20;  // ����ͼ������������ʾͼ��ԭ�������Ͻ�
    out.write((char*)&header, sizeof(header));  // ��ͷ����Ϣд���ļ�
    if (!out.good()) {  // ���д������Ƿ�ɹ�
        out.close();  // �ر��ļ�
        std::cerr << "can't dump the tga file\n";  // ���������Ϣ
        return false;  // ����ʧ��
    }
    if (!rle) {
        out.write((char*)data, width * height * bytespp);  // �����ʹ��RLEѹ����ֱ��д��ԭʼͼ������
        if (!out.good()) {  // ���д������Ƿ�ɹ�
            std::cerr << "can't unload raw data\n";  // ���������Ϣ
            out.close();  // �ر��ļ�
            return false;  // ����ʧ��
        }
    }
    else {
        if (!unload_rle_data(out)) {  // ���ʹ��RLEѹ��������unload_rle_data��������ѹ����д��
            out.close();  // �ر��ļ�
            std::cerr << "can't unload rle data\n";  // ���������Ϣ
            return false;  // ����ʧ��
        }
    }
    out.write((char*)developer_area_ref, sizeof(developer_area_ref));  // д�뿪������������
    if (!out.good()) {  // ���д������Ƿ�ɹ�
        std::cerr << "can't dump the tga file\n";  // ���������Ϣ
        out.close();  // �ر��ļ�
        return false;  // ����ʧ��
    }
    out.write((char*)extension_area_ref, sizeof(extension_area_ref));  // д����չ��������
    if (!out.good()) {  // ���д������Ƿ�ɹ�
        std::cerr << "can't dump the tga file\n";  // ���������Ϣ
        out.close();  // �ر��ļ�
        return false;  // ����ʧ��
    }
    out.write((char*)footer, sizeof(footer));  // д���ļ�β
    if (!out.good()) {  // ���д������Ƿ�ɹ�
        std::cerr << "can't dump the tga file\n";  // ���������Ϣ
        out.close();  // �ر��ļ�
        return false;  // ����ʧ��
    }
    out.close();  // �ر��ļ�
    return true;  // ���سɹ�
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TGAImage::unload_rle_data(std::ofstream& out) {
    const unsigned char max_chunk_length = 128;  // �������鳤��Ϊ128
    unsigned long npixels = width * height;  // ����ͼ�����������
    unsigned long curpix = 0;  // ��ǰ�����������
    while (curpix < npixels) {  // ѭ��ֱ����������������
        unsigned long chunkstart = curpix * bytespp;  // ��ǰ�����ʼ�ֽ�λ��
        unsigned long curbyte = curpix * bytespp;  // ��ǰ������ֽ�λ��
        unsigned char run_length = 1;  // ��ǰ��ĳ���
        bool raw = true;  // ��ǵ�ǰ���Ƿ�Ϊԭʼ���ݿ�
        while (curpix + run_length < npixels && run_length < max_chunk_length) {
            bool succ_eq = true;  // ��ǵ�ǰ�����Ƿ�����һ��������ͬ
            for (int t = 0; succ_eq && t < bytespp; t++) {
                succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);  // �Ƚϵ�ǰ���غ���һ�����ص�ÿ��ͨ��
            }
            curbyte += bytespp;  // �ƶ�����һ������
            if (1 == run_length) {
                raw = !succ_eq;  // ����ǵ�һ������������һ�����ز�ͬ������Ϊԭʼ���ݿ�
            }
            if (raw && succ_eq) {
                run_length--;  // �����ԭʼ���ݿ��ҵ�ǰ��������һ��������ͬ������ٳ��Ȳ��˳�
                break;
            }
            if (!raw && !succ_eq) {
                break;  // �����RLEѹ�����ҵ�ǰ��������һ�����ز�ͬ�����˳�
            }
            run_length++;  // ���ӿ鳤��
        }
        curpix += run_length;  // �ƶ�����һ�������ʼλ��
        out.put(raw ? run_length - 1 : run_length + 127);  // д���ͷ
        if (!out.good()) {  // ���д������Ƿ�ɹ�
            std::cerr << "can't dump the tga file\n";  // ���������Ϣ
            return false;  // ����ʧ��
        }
        out.write((char*)(data + chunkstart), (raw ? run_length * bytespp : bytespp));  // д��ͼ������
        if (!out.good()) {  // ���д������Ƿ�ɹ�
            std::cerr << "can't dump the tga file\n";  // ���������Ϣ
            return false;  // ����ʧ��
        }
    }
    return true;  // ���سɹ�
}

// ��ȡָ��λ�õ�������ɫ
TGAColor TGAImage::get(int x, int y) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {  // ���ͼ�����ݻ������Ƿ�����Լ������Ƿ���Ч
        return TGAColor();  // ����һ��Ĭ����ɫ����
    }
    return TGAColor(data + (x + y * width) * bytespp, bytespp);  // ����ָ��λ�õ�������ɫ
}

// ����ָ��λ�õ�������ɫ
bool TGAImage::set(int x, int y, TGAColor c) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) {  // ���ͼ�����ݻ������Ƿ�����Լ������Ƿ���Ч
        return false;  // ����ʧ��
    }
    memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);  // ����ɫ���ݸ��Ƶ�ָ��λ��
    return true;  // ���سɹ�
}

// ��ȡÿ�����ص��ֽ���
int TGAImage::get_bytespp() {
    return bytespp;  // ����ÿ�����ص��ֽ���
}

// ��ȡͼ��Ŀ��
int TGAImage::get_width() {
    return width;  // ����ͼ��Ŀ��
}

// ��ȡͼ��ĸ߶�
int TGAImage::get_height() {
    return height;  // ����ͼ��ĸ߶�
}

// ˮƽ��תͼ��
bool TGAImage::flip_horizontally() {
    if (!data) return false;  // ���ͼ�����ݻ������Ƿ����
    int half = width >> 1;  // ����ͼ���ȵ�һ��
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < height; j++) {
            TGAColor c1 = get(i, j);  // ��ȡ���������ɫ
            TGAColor c2 = get(width - 1 - i, j);  // ��ȡ�Ҳ�������ɫ
            set(i, j, c2);  // ���Ҳ�������ɫ���õ����
            set(width - 1 - i, j, c1);  // �����������ɫ���õ��Ҳ�
        }
    }
    return true;  // ���سɹ�
}

// ��ֱ��תͼ��
bool TGAImage::flip_vertically() {
    if (!data) return false;  // ���ͼ�����ݻ������Ƿ����
    unsigned long bytes_per_line = width * bytespp;  // ����ÿ�е��ֽ���
    unsigned char* line = new unsigned char[bytes_per_line];  // ������ʱ�������洢һ��ͼ������
    int half = height >> 1;  // ����ͼ��߶ȵ�һ��
    for (int j = 0; j < half; j++) {
        unsigned long l1 = j * bytes_per_line;  // ��ǰ�е���ʼ�ֽ�λ��
        unsigned long l2 = (height - 1 - j) * bytes_per_line;  // ��Ӧ�ĵ�����j�е���ʼ�ֽ�λ��
        memmove((void*)line, (void*)(data + l1), bytes_per_line);  // ����ǰ�����ݸ��Ƶ���ʱ������
        memmove((void*)(data + l1), (void*)(data + l2), bytes_per_line);  // ��������j�����ݸ��Ƶ���ǰ��
        memmove((void*)(data + l2), (void*)line, bytes_per_line);  // ����ʱ�����������ݸ��Ƶ�������j��
    }
    delete[] line;  // �ͷ���ʱ������
    return true;  // ���سɹ�
}

// ��ȡͼ�����ݻ�����
unsigned char* TGAImage::buffer() {
    return data;  // ����ͼ�����ݻ�������ָ��
}

// ���ͼ�����ݻ�����
void TGAImage::clear() {
    memset((void*)data, 0, width * height * bytespp);  // ��ͼ�����ݻ������������ֽ�����Ϊ0
}

// ����ͼ��ָ���Ŀ�Ⱥ͸߶�
bool TGAImage::scale(int w, int h) {
    if (w <= 0 || h <= 0 || !data) return false;  // ���Ŀ���ȡ��߶��Ƿ���Ч�Լ�ͼ�����ݻ������Ƿ����
    unsigned char* tdata = new unsigned char[w * h * bytespp];  // �����ڴ�洢���ź��ͼ������
    int nscanline = 0;  // ��ǰĿ��ɨ���ߵ���ʼλ��
    int oscanline = 0;  // ��ǰԭʼɨ���ߵ���ʼλ��
    int erry = 0;  // ��ֱ��������
    unsigned long nlinebytes = w * bytespp;  // Ŀ��ͼ��ÿ�е��ֽ���
    unsigned long olinebytes = width * bytespp;  // ԭʼͼ��ÿ�е��ֽ���
    for (int j = 0; j < height; j++) {
        int errx = width - w;  // ˮƽ��������
        int nx = -bytespp;  // ��ǰĿ�����ص���ʼλ��
        int ox = -bytespp;  // ��ǰԭʼ���ص���ʼλ��
        for (int i = 0; i < width; i++) {
            ox += bytespp;  // �ƶ�����һ��ԭʼ����
            errx += w;  // ����ˮƽ��������
            while (errx >= (int)width) {  // ���ˮƽ����������ڵ���ͼ����
                errx -= width;  // ��ȥͼ����
                nx += bytespp;  // �ƶ�����һ��Ŀ������
                memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);  // ����ԭʼ���ص�Ŀ������
            }
        }
        erry += h;  // ���Ӵ�ֱ��������
        oscanline += olinebytes;  // �ƶ�����һ��ԭʼ����
        while (erry >= (int)height) {  // �����ֱ����������ڵ���ͼ��߶�
            if (erry >= (int)height << 1)  // ��������ڵ��������߶ȣ���ʾ��Ҫ����һ��
                memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);  // ���Ƶ�ǰɨ���ߵ���һ��
            erry -= height;  // ��ȥͼ��߶�
            nscanline += nlinebytes;  // �ƶ�����һ��Ŀ������
        }
    }
    delete[] data;  // �ͷ�ԭʼͼ�����ݻ�����
    data = tdata;  // ��Ŀ��ͼ�����ݻ�������ֵ����ǰ����
    width = w;  // ���¿��
    height = h;  // ���¸߶�
    return true;  // ���سɹ�
}
