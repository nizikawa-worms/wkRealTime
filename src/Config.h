#ifndef WKREALTIME_CONFIG_H
#define WKREALTIME_CONFIG_H

#include <string>
#include <json.hpp>
#include <set>
#include <filesystem>
#include "version.h"

class Config {
public:
	static inline const std::string iniFile = PROJECT_NAME ".ini";
	static inline const std::string moduleName = PROJECT_NAME;

private:
	static inline bool moduleEnabled = true;
	static inline bool devConsoleEnabled = true;

	static inline bool ignoreVersionCheck = false;
	static inline bool hexDumpPackets = false;
	static inline bool useOffsetCache = true;
	static inline bool useMutex = false;

	static inline std::filesystem::path waDir;

	static inline bool logToFile = false;
	static inline int colorMod = 0;
public:

	static void readConfig();
	static bool isModuleEnabled();
	static bool isDevConsoleEnabled();
	static int waVersionCheck();

	static std::string getVersionStr();
	static int getVersionInt();
	static std::string getBuildStr();
	static std::string getModuleStr();
	static std::string getFullStr();
	static int getProtocolVersion();

	static void addVersionInfoToJson(nlohmann::json &);
	static bool isHexDumpPacketsEnabled();

	static const std::filesystem::path &getWaDir();

	static bool isUseOffsetCache();
	static bool isMutexEnabled();
	static bool isLogToFile();
	static inline FILE * logfile;

	static std::string getWaVersionAsString();

	static int getColorMod();
};


#endif //WKREALTIME_CONFIG_H
