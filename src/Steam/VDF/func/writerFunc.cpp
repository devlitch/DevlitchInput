#include "WriterFunc.h"

bool WriterFunc::Open(string path) {
    file.open(path, ios::binary | ios::trunc);
    return file.is_open();
}

void WriterFunc::Type(uint8_t type) {
    file.put((char)type);
}

void WriterFunc::String(const string& str) {
    file.write(str.c_str(), str.size());
    file.put('\0');
}

void WriterFunc::Int(uint32_t value) {
    file.write((char*)&value, sizeof(value));
}

void WriterFunc::BeginDict(const string& key) {
    Type(0x00);
    String(key);
}

void WriterFunc::EndDict() {
    Type(0x08);
}

void WriterFunc::WriteString(const string& key, const string& value) {
    Type(0x01);
    String(key);
    String(value);
}

void WriterFunc::WriteInt(const string& key, uint32_t value) {
    Type(0x02);
    String(key);
    Int(value);
}

void WriterFunc::WriteBool(const string& key, bool value) {
    WriteInt(key, value ? 1 : 0);
}

void WriterFunc::Close() {
    file.close();
}

bool WriterFunc::Good() {
    return file.good();
}