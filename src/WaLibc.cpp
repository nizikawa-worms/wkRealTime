#include "WaLibc.h"
#include "Hooks.h"
#include "Config.h"
#include "Debugf.h"
#include <filesystem>

void *(__cdecl * origWaNew)(size_t size);
void *WaLibc::waMalloc(size_t size) {
	return origWaNew(size);
}

void (__cdecl *origWaFree)(void * ptr);
void WaLibc::waFree(void *ptr) {
	origWaFree(ptr);
}

FILE *__cdecl WaLibc::hookWaFopen(char *Filename, char *Mode) {
	std::string file = Filename;
	return origWaFopen((char*)file.c_str(), Mode);
}

int WaLibc::install() {
	DWORD addrWaMallocMemset = _ScanPattern("WaMallocMemset", "\x8D\x47\x03\x83\xE0\xFC\x83\xC0\x20\x56\x50\xE8\x00\x00\x00\x00\x57\x8B\xF0\x6A\x00\x56\xE8\x00\x00\x00\x00\x83\xC4\x10\x8B\xC6\x5E\xC3", "??????xxxxxx????xxxxxxx????xxxxxxx");
	origWaNew = (void *(__cdecl *)(size_t)) (addrWaMallocMemset + 0x10 +  *(DWORD*)(addrWaMallocMemset +  0xC)); //5C0377
	origWaFree = (void (__cdecl *)(void *))_ScanPattern("WaFree", "\x6A\x0C\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x75\x08\x85\xF6\x74\x75\x83\x3D\x00\x00\x00\x00\x00\x75\x43\x6A\x04\xE8\x00\x00\x00\x00\x59\x83\x65\xFC\x00", "???????x????xxxxxxxxx?????xxxxx????xxxxx");
	DWORD addrGetDataPath = _ScanPattern("GetDataPath", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x08\x53\x56\x8B\x74\x24\x20\x33\xDB\x89\x5C\x24\x18\x89\x5C\x24\x0C\xE8\x00\x00\x00\x00\x33\xC9", "???????xx????xxxx????xxxxxxxxxxxxxxxxxxxx????xx");
	addrDriveLetter = *(char**)(addrGetDataPath + 0x69);
	addrSteamFlag = *(char**)(addrGetDataPath + 0x56);
	debugf("addrDriveLetter: 0x%X addrSteamFlag: 0x%X\n", addrDriveLetter, addrSteamFlag);

	DWORD addrWaFopenRef = _ScanPattern("WaFopenRef", "\x81\xEC\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\x85\xC0\x89\x44\x24\x14\x0F\x84\x00\x00\x00\x00\x0F\xB7\x06", "??????x????xx????xxxxxxxxxxx????xxx");
	DWORD addrWaFopen = *(DWORD*)(addrWaFopenRef + 0xD) + addrWaFopenRef + 0x11;
	waSrand =
		(void (__cdecl *)(unsigned int))
		_ScanPattern("WaSrand", "\xE8\x00\x00\x00\x00\x8B\x4C\x24\x04\x89\x48\x14\xC3", "x????xxxxxxxx");
	waRand =
		(int (__cdecl *)())
		_ScanPattern("WaRand", "\xE8\x00\x00\x00\x00\x8B\x48\x14\x69\xC9\x00\x00\x00\x00\x81\xC1\x00\x00\x00\x00\x89\x48\x14\x8B\xC1\xC1\xE8\x10\x25\x00\x00\x00\x00\xC3", "x????xxxxx????xx????xxxxxxxxx????x");
	CStringFromString =
		(int (__fastcall *)(void *,int,void *,size_t))
		_ScanPattern("CStringFromString", "\x53\x56\x8B\x74\x24\x10\x85\xF6\x8B\xD9\x75\x0A\xE8\x00\x00\x00\x00\x5E\x5B\xC2\x08\x00\x8B\x4C\x24\x0C\x85\xC9", "??????xxxxxxx????xxxxxxxxxxx");
	CStringBufferFromString =
		(int (__fastcall *)(char **,int,char *,size_t))
		_ScanPattern("CStringBufferFromString", "\x53\x56\x8B\x74\x24\x10\x85\xF6\x8B\xD9\x75\x0A\xE8\x00\x00\x00\x00\x5E\x5B\xC2\x08\x00\x8B\x4C\x24\x0C\x85\xC9\x75\x0A", "??????xxxxxxx????xxxxxxxxxxxxx");

	origWaFwrite =
		(int (__cdecl *)(const void *,size_t,size_t,FILE *))
		_ScanPattern("WaFwrite", "\x6A\x0C\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x33\xF6\x39\x75\x0C\x74\x29\x39\x75\x10\x74\x24\x33\xC0\x39\x75\x14\x0F\x95\xC0", "???????x????xxxxxxxxxxxxxxxxxxxx");
	origWaFclose =
		(int (__cdecl *)(FILE *))
		_ScanPattern("WaFclose", "\x6A\x0C\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x4D\xE4\xFF\x33\xC0\x8B\x75\x08\x33\xFF\x3B\xF7\x0F\x95\xC0\x3B\xC7\x75\x1D\xE8\x00\x00\x00\x00\xC7\x00\x00\x00\x00\x00", "???????x????xxxxxxxxxxxxxxxxxxxxx????xx????");

	_HookDefault(WaFopen);
	return 0;
}

std::string WaLibc::getWaDataPath(bool addWaPath) {
	if(!addrDriveLetter || !addrSteamFlag) return "";
	if(*addrSteamFlag) {
		if(!addWaPath) return "Data";
		else return (Config::getWaDir() / "Data").string();
	}
	if(*addrDriveLetter) {
		char buff[16];
		_snprintf_s(buff, _TRUNCATE, "%c:\\Data", *addrDriveLetter);

		std::filesystem::path path(buff);
		if(!std::filesystem::exists(path)) return "";

		return buff;
	} else {
		return "";
	}
}
