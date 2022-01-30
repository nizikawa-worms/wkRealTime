#ifndef WKREALTIME_HOOKS_H
#define WKREALTIME_HOOKS_H


#include <map>
#include <string>
#include <vector>
#include <polyhook2/Detour/x86Detour.hpp>
#include <polyhook2/PE/IatHook.hpp>
#include "version.h"

#ifndef __CALLPOSITION__
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define __CALLPOSITION__  __FUNCTION__ ":" STRINGIZE(__LINE__)
#endif

class Hooks {
private:
	struct PatchInfo {
		DWORD addr;
		std::string original;
		~PatchInfo() {PatchMemData((PVOID)addr, original.size(), (PVOID)original.data(), original.size());}
	};

	static inline const std::string cacheFile = PROJECT_NAME ".cache";
	static inline std::map<std::string, DWORD> hookNameToAddr;
	static inline std::map<DWORD, std::string> hookAddrToName;

	static inline std::map<std::string, DWORD> scanNameToAddr;
	static inline std::vector<std::unique_ptr<PLH::x86Detour>> detours;
	static inline std::vector<std::unique_ptr<PLH::IatHook>> iathooks;
	static inline std::vector<std::unique_ptr<PatchInfo>> patches;

	static inline bool foundNewOffsets = false;

	//Worms development tools by StepS
	template<typename VT>
	static BOOL __stdcall PatchMemVal(PVOID pAddr, VT newValue) { return PatchMemData(pAddr, sizeof(VT), &newValue, sizeof(VT)); }
	template<typename VT>
	static BOOL __stdcall PatchMemVal(ULONG_PTR pAddr, VT newValue) { return PatchMemData((PVOID) pAddr, sizeof(VT), &newValue, sizeof(VT)); }

	static BOOL PatchMemData(PVOID pAddr, size_t buf_len, PVOID pNewData, size_t data_len);
	static BOOL InsertJump(PVOID pDest, size_t dwPatchSize, PVOID pCallee, DWORD dwJumpType);
public:
	static void hook(std::string name, DWORD pTarget, DWORD *pDetour, DWORD *ppOriginal, const char * line = nullptr);
#define _Hook(name, pTarget, pDetour, ppOriginal) Hooks::hook(name, pTarget, pDetour, ppOriginal, __CALLPOSITION__ )
#define _HookDefault(name) Hooks::hook(#name, addr##name, (DWORD *) &hook##name, (DWORD *) &orig##name, __CALLPOSITION__)

	static void hookIat(std::string dllName, std::string apiName, DWORD *pDetour, DWORD *ppOriginal);

	static void hookAsm(DWORD startAddr, DWORD hookAddr, const char * line = nullptr);
#define _HookAsm(startAddr, hookAddr) Hooks::hookAsm(startAddr, hookAddr, __CALLPOSITION__ )

	static void hookVtable(const char * classname, int offset, DWORD addr, DWORD hookAddr, DWORD * original, const char * line = nullptr);
#define _HookVtable(classname, offset, addr, hookAddr, original) Hooks::hookVtable(classname, offset, addr, hookAddr, original, __CALLPOSITION__ )

	static void patchAsm(DWORD addr, unsigned char *op, size_t opsize, const char * line = nullptr);
#define _PatchAsm(addr, op, opsize) Hooks::patchAsm(addr, op, opsize, __CALLPOSITION__ )

	static DWORD scanPattern(const char* name, const char* pattern, const char* mask, DWORD expected = 0, const char * line = nullptr);
#define _ScanPattern(name, pattern, mask) Hooks::scanPattern(name, pattern, mask, 0, __CALLPOSITION__ )

	static const std::map<std::string, DWORD> &getScanNameToAddr();
	static void loadOffsets();
	static void saveOffsets();
	static void cleanup();
};


#endif //WKREALTIME_HOOKS_H
