#include "Writer.h"

static uint32_t crcTable[256];
bool crcSetup = false;
void Writer::InitCRC32() {
	for (uint32_t i = 0; i < 256; i++) {
		uint32_t c = i;
		for (int j = 0; j < 8; j++) {
			if (c & 1) c = 0xEDB88320 ^ (c >> 1);
			else c >>= 1;
		}
		crcTable[i] = c;
	}
}
uint32_t Writer::CRC32(const string& str) {
	uint32_t crc = 0xFFFFFFFF;
	for (unsigned char c : str) crc = crcTable[(crc ^ c) & 0xFF] ^ (crc >> 8);
	return crc ^ 0xFFFFFFFF;
}
uint32_t Writer::GenerateAppID(const string& exe, const string& appName) {
	if (!crcSetup) InitCRC32();
	return CRC32(exe + appName) | 0x80000000;
}
void Writer::WriteShortcut(WriterFunc& w, const Shortcut& sc) {

	w.WriteInt("appid", sc.appid);

	w.WriteString("AppName", sc.AppName);
	w.WriteString("Exe", sc.Exe);
	w.WriteString("StartDir", sc.StartDir);
	w.WriteString("icon", sc.Icon);
	w.WriteString("ShortcutPath", sc.ShortcutPath);
	w.WriteString("LaunchOptions", sc.LaunchOptions);

	w.WriteBool("IsHidden", sc.IsHidden);
	w.WriteBool("AllowDesktopConfig", sc.AllowDesktopConfig);
	w.WriteBool("AllowOverlay", sc.AllowOverlay);
	w.WriteBool("OpenVR", sc.OpenVR);
	w.WriteBool("Devkit", sc.Devkit);

	w.WriteString("DevkitGameID", sc.DevkitGameID);
	w.WriteInt("DevkitOverrideAppID", sc.DevkitOverrideAppID);
	w.WriteInt("LastPlayTime", sc.LastPlayTime);

	w.WriteString("FlatpakAppID", sc.FlatpakAppID);
	w.WriteString("sortas", sc.SortAs);

	w.BeginDict("tags");

	for (size_t i = 0; i < sc.Tags.size(); ++i) w.WriteString(to_string(i), sc.Tags[i]);
	w.EndDict();
}
void Writer::WriteShortcuts(string path, const vector<Shortcut>& shortcuts) {
	WriterFunc w;
	w.Open(path);
	w.BeginDict("shortcuts");
	for (size_t i = 0; i < shortcuts.size(); ++i) {
		w.BeginDict(to_string(i));
		WriteShortcut(w, shortcuts[i]);
		w.EndDict();
	}
	w.EndDict();
	w.EndDict();
	w.Close();
}
bool Writer::CanWriteFile(const string& path) {
	ofstream test(path, ios::app | ios::binary);
	if (!test.is_open()) return false;
	test.close();
	return true;
}
bool Writer::BackupFile(const string& original, const string& backup) {
	ifstream src(original, ios::binary);
	if (!src.is_open()) return false;
	ofstream dst(backup, ios::binary);
	if (!dst.is_open()) return false;
	dst << src.rdbuf();
	return true;
}