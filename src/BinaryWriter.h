#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
class BinaryWriter {
public:
    BinaryWriter(int size = 1500) : pointer(0) {
        buffer = (char*)malloc(size);
    }

    ~BinaryWriter() {
        free(buffer);
    }

    inline void write1(unsigned char byte) {
        buffer[pointer++] = byte;
    }

    inline void write2(short value) {
        memcpy(&buffer[pointer], &value, 2);
        pointer += 2;
    }

    inline void write4(int value) {
        memcpy(&buffer[pointer], &value, 4);
        pointer += 4;
    }

    inline void write4float(float value) {
        memcpy(&buffer[pointer], &value, 4);
        pointer += 4;
    }

    inline void write8(long long value) {
        memcpy(&buffer[pointer], &value, 8);
        pointer += 8;
    }

    inline void writeSZ(const char* str) 
    {
        int sz = strlen(str);
        for (int i = 0; i < sz; i++) {
            write1(str[i]);
        }
    }

    inline void writeS(const char* str) {
        int sz = strlen(str);
        for(int i = 0; i < sz; i++) {
            write1(str[i]);
        }
        write1('\0');
    }

    inline void save(const std::string& name) {
        std::ofstream fstream(name, std::ios::binary);
        if (fstream.is_open()) {
            fstream.write(buffer, pointer);
            fstream.close();
            std::cout << "Данные успешно записаны в файл: " << name << std::endl;
        }
        else {
            std::cerr << "Ошибка открытия файла для записи: " << name << std::endl;
        }
    }

    char* buffer;
    int pointer;
};