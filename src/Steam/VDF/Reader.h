#pragma once

#include <vector>
#include <string>
#include "func/readerFunc.h"
#include "Shortcut.h"

enum VdfType {
    VDF_DICT = 0x00,
    VDF_STRING = 0x01,
    VDF_INT = 0x02,
    VDF_END = 0x08
};
class Reader {
public:
    void ReadShortcut(ReaderFunc& r, Shortcut& sc);

    std::vector<Shortcut> ReadShortcuts(std::string path);
};