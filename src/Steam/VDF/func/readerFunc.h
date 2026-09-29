#pragma once

#include <string>
#include <cstdint>
#include <fstream>

using namespace std;

class ReaderFunc {
public:
    bool Open(const string& path);

    uint8_t Type();
    string String();
    uint32_t Int();

    bool Good();
private:
    std::ifstream file;
};