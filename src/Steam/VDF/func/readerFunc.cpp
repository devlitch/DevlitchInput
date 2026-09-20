#include "readerFunc.h"
#include <fstream>
    
ifstream file;

bool ReaderFunc::Open(string path) {
    file.open(path, ios::binary);
    return file.is_open();
}

uint8_t ReaderFunc::Type() {
    return (uint8_t)file.get();
}

string ReaderFunc::String() {
    string s;
    char c;
    while (file.get(c)) {
        if (c == 0) break;
        s += c;
    }
    return s;
}

uint32_t ReaderFunc::Int() {
    uint32_t x;
    file.read((char*)&x, sizeof(x));
    return x;
}

bool ReaderFunc::Good(){
    return file.good();
}