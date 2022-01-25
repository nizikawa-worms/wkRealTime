#include "DevConsole.h"
#include "Config.h"

#include <windows.h>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include "Config.h"

void DevConsole::install() {
	if(!AllocConsole()) return;
	SetConsoleTitle(Config::getFullStr().c_str());

	//resize console
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.X = 600;
	coninfo.dwSize.Y = 3000;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
	FILE *COutputHandle = _fdopen(SystemOutput, "w");

	HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
	int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
	FILE *CErrorHandle = _fdopen(SystemError, "w");

	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
	FILE *CInputHandle = _fdopen(SystemInput, "r");

	SetConsoleMode(ConsoleInput,  ENABLE_QUICK_EDIT_MODE);

	std::ios::sync_with_stdio(true);

	freopen_s(&CInputHandle, "CONIN$", "r", stdin);
	freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
	freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}
