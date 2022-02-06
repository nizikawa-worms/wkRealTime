
#include <windows.h>
#include "Config.h"
#include "Utils.h"
#include "WaLibc.h"
#include "Debugf.h"
#include "RealTime.h"
#include <filesystem>
#include "git.h"

namespace fs = std::filesystem;

void Config::readConfig() {
	char wabuff[MAX_PATH];
	GetModuleFileNameA(0, (LPSTR)&wabuff, sizeof(wabuff));
	waDir = fs::path(wabuff).parent_path();
	auto inipath = (waDir / iniFile).string();
	moduleEnabled = GetPrivateProfileIntA("general", "EnableModule", 1, inipath.c_str());
	useOffsetCache = GetPrivateProfileIntA("general", "UseOffsetCache", 1, inipath.c_str());
	useMutex = GetPrivateProfileIntA("general", "UseMutex", 0, inipath.c_str());
	ignoreVersionCheck = GetPrivateProfileIntA("general", "IgnoreVersionCheck", 0, inipath.c_str());

	colorMod = GetPrivateProfileIntA("general", "ColorMod", 1, inipath.c_str());

	devConsoleEnabled = GetPrivateProfileIntA("debug", "EnableDevConsole", 0, inipath.c_str());
	hexDumpPackets = GetPrivateProfileIntA("debug", "HexDumpPackets", 0, inipath.c_str());
	RealTime::setDebugFifo(GetPrivateProfileIntA("debug", "ShowFifo", 0, inipath.c_str()));

	logToFile = GetPrivateProfileIntA("debug", "LogToFile", 0, inipath.c_str());
	if(logToFile) {
		logfile = fopen(PROJECT_NAME ".log", "w");
	}

}

bool Config::isDevConsoleEnabled() {
	return devConsoleEnabled;
}

bool Config::isModuleEnabled() {
	return moduleEnabled;
}



//StepS tools
typedef unsigned long long QWORD;
#define MAKELONGLONG(lo,hi) ((LONGLONG(DWORD(lo) & 0xffffffff)) | LONGLONG(DWORD(hi) & 0xffffffff) << 32 )
#define QV(V1, V2, V3, V4) MAKEQWORD(V4, V3, V2, V1)
#define MAKEQWORD(LO2, HI2, LO1, HI1) MAKELONGLONG(MAKELONG(LO2,HI2),MAKELONG(LO1,HI1))
QWORD GetModuleVersion(HMODULE hModule)
{
	char WApath[MAX_PATH]; DWORD h;
	GetModuleFileNameA(hModule,WApath,MAX_PATH);
	DWORD Size = GetFileVersionInfoSizeA(WApath,&h);
	if(Size)
	{
		void* Buf = malloc(Size);
		GetFileVersionInfoA(WApath,h,Size,Buf);
		VS_FIXEDFILEINFO *Info; DWORD Is;
		if(VerQueryValueA(Buf, "\\", (LPVOID*)&Info, (PUINT)&Is))
		{
			if(Info->dwSignature==0xFEEF04BD)
			{
				return MAKELONGLONG(Info->dwFileVersionLS, Info->dwFileVersionMS);
			}
		}
		free(Buf);
	}
	return 0;
}

int Config::waVersionCheck() {
	if(ignoreVersionCheck)
		return 1;

	auto version = GetModuleVersion((HMODULE)0);
	char versionstr[64];
	_snprintf_s(versionstr, _TRUNCATE, "Detected game version: %u.%u.%u.%u", PWORD(&version)[3], PWORD(&version)[2], PWORD(&version)[1], PWORD(&version)[0]);
	debugf("%s\n", versionstr);

	std::string tversion = getFullStr();
	char buff[512];
	if(version == QV(3,8,1,0)) {
		return 1;
	}

	_snprintf_s(buff, _TRUNCATE, PROJECT_NAME " is not designed to work with your WA version and may malfunction.\n\nTo disable this warning set IgnoreVersionCheck=1 in {} file.\n\n%s", versionstr, iniFile.c_str());
	return MessageBoxA(0, buff, tversion.c_str(), MB_OKCANCEL | MB_ICONWARNING) == IDOK;
}

void Config::addVersionInfoToJson(nlohmann::json & json) {
	json["module"] = getModuleStr();
	json["version"] = getVersionStr();
	json["versionInt"] = getVersionInt();
	json["build"] = getBuildStr();
	json["protocol"] = getProtocolVersion();
}

std::string Config::getModuleStr() {
	return PROJECT_NAME;
}

std::string Config::getVersionStr() {
	return std::format("v{}.{}.{}.{}", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH, PROJECT_VERSION_TWEAK);
}

int Config::getVersionInt() {
	return 1000000 * PROJECT_VERSION_MAJOR + 10000 * PROJECT_VERSION_MINOR + 100 * PROJECT_VERSION_PATCH + PROJECT_VERSION_TWEAK;
}

std::string Config::getBuildStr() {
	return __DATE__ " " __TIME__;
}

std::string Config::getFullStr() {
	return std::format("{} {} (build: {} {})", getModuleStr(), getVersionStr(), getBuildStr(), getGitStr());
}

bool Config::isHexDumpPacketsEnabled() {
	return hexDumpPackets;
}


const std::filesystem::path &Config::getWaDir() {
	return waDir;
}

int Config::getProtocolVersion() {
	return 0;
}


bool Config::isUseOffsetCache() {
	return useOffsetCache;
}

std::string Config::getWaVersionAsString() {
	char buff[32];
	auto version = GetModuleVersion(0);
	sprintf_s(buff, "%u.%u.%u.%u", PWORD(&version)[3], PWORD(&version)[2], PWORD(&version)[1], PWORD(&version)[0]);
	return buff;
}

bool Config::isLogToFile() {
	return logToFile;
}

bool Config::isMutexEnabled() {
	return useMutex;
}

int Config::getColorMod() {
	return colorMod;
}

std::string Config::getGitStr() {
	return std::format("[{}@{}{}]",  GitMetadata::Branch(), GitMetadata::Describe(), GitMetadata::AnyUncommittedChanges() ? " !!" : "");
}

