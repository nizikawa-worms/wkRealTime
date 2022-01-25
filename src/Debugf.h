
#ifndef WKREALTIME_DEBUGF_H
#define WKREALTIME_DEBUGF_H

#include "Config.h"
#define debugf(fmt, ...) if(Config::isDevConsoleEnabled()) {printf("%s:%d: " fmt, __FUNCTION__ , __LINE__, __VA_ARGS__); if(Config::isLogToFile()) {fprintf(Config::logfile, "%s:%d: " fmt, __FUNCTION__ , __LINE__, __VA_ARGS__); fflush(Config::logfile);}}

#endif //WKREALTIME_DEBUGF_H
