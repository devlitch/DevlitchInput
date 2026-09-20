#pragma once

#include <string>
#include <cstdint>
#include <fstream>

#include "func/writerFunc.h"
#include "Shortcut.h"

using namespace std;

class Writer {
public:
    void WriteShortcuts(string path, const vector<Shortcut>& shortcuts);
    uint32_t GenerateAppID(const string& exe, const string& appName);
    bool BackupFile(const string& original, const string& backup);
    bool CanWriteFile(const string& path);

private:
    ofstream file;
    static void InitCRC32();
    uint32_t CRC32(const string& str);
    void WriteShortcut(WriterFunc& w, const Shortcut& sc);
};