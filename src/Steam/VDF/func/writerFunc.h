#pragma once

#include <string>
#include <cstdint>
#include <fstream>

using namespace std;

class WriterFunc {
public:
    bool Open(string path);

    void Type(uint8_t type);
    void String(const string& str);
    void Int(uint32_t value);

    void BeginDict(const string& key);
    void EndDict();

    void WriteString(const string& key, const string& value);
    void WriteInt(const string& key, uint32_t value);
    void WriteBool(const string& key, bool value);

    void Close();
    bool Good();

private:
    ofstream file;
};