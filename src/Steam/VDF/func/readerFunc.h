#pragma once

#include <string>
#include <cstdint>

using namespace std;

class ReaderFunc {
public:
    bool Open(string path);

    uint8_t Type();
    string String();
    uint32_t Int();

    bool Good();
};