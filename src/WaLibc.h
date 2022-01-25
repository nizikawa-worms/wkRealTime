
#ifndef WKREALTIME_WALIBC_H
#define WKREALTIME_WALIBC_H

typedef unsigned long       DWORD;
#include <string>

class WaLibc {
private:
	static inline char * addrDriveLetter = nullptr;
	static inline char * addrSteamFlag = nullptr;

	static FILE *__cdecl hookWaFopen(char *Filename, char *Mode);
public:
	static void * waMalloc(size_t size);
	static void waFree(void * ptr);
	static inline void (__cdecl *waSrand)(unsigned int Seed);
	static inline int (__cdecl *waRand)();
	static inline int (__fastcall *CStringFromString)(void * This, int EDX, void *Source, size_t DestinationSize);
	static inline int (__fastcall *CStringBufferFromString)(char ** buffer, int EDX, char * source, size_t sourcelen);
	static inline FILE *(__cdecl *origWaFopen)(char *Filename, char *Mode);
	static inline int (__cdecl *origWaFwrite)(const void *Buffer, size_t ElementSize, size_t ElementCount, FILE *Stream);
	static inline int (__cdecl *origWaFclose)(FILE *Stream);
	static std::string getWaDataPath(bool addWaPath=false);
	static int install();
};


#endif //WKREALTIME_WALIBC_H
