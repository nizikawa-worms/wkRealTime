#include <windows.h>
#include <string>

#include <stdexcept>
#include <ctime>

#include "WaLibc.h"
#include "DevConsole.h"
#include "Config.h"
#include "LobbyPackets.h"
#include "LobbyChat.h"
#include "LobbyProtocol.h"
#include "Frontend.h"
#include "W2App.h"
#include "Debugf.h"
#include "Hooks.h"
#include "entities/CTaskTeam.h"
#include "entities/CTaskTurnGame.h"
#include "RealTime.h"
#include "Chat.h"
#include "TaskMessageFifo.h"
#include <chrono>
#include "entities/CTaskWorm.h"
#include "GameNet.h"
#include "Camera.h"
#include "Drawing.h"

void install() {
	srand(time(0) * GetCurrentProcessId());

	WaLibc::install();
	LobbyPackets::install();
	LobbyChat::install();
	LobbyProtocol::install();
	Frontend::install();
	W2App::install();

	CTaskTeam::install();
	CTaskTurnGame::install();
	CTaskWorm::install();
	TaskMessageFifo::install();
	GameNet::install();
	RealTime::install();
	Chat::install();
	Camera::install();
	Drawing::install();
}

// Thanks StepS
bool LockGlobalInstance(LPCTSTR MutexName)
{
	if(!Config::isMutexEnabled()) return true;
	if (!CreateMutex(NULL, 0, MutexName)) return 0;
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
	return 1;
}

char LocalMutexName[MAX_PATH];
bool LockCurrentInstance(LPCTSTR MutexName)
{
	_snprintf_s(LocalMutexName, _TRUNCATE,"P%u/%s", GetCurrentProcessId(), MutexName);
	return LockGlobalInstance(LocalMutexName);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			auto start = std::chrono::high_resolution_clock::now();
			decltype(start) finish;
			try {
				Config::readConfig();
				if (Config::isModuleEnabled() && Config::waVersionCheck() && LockCurrentInstance(PROJECT_NAME)) {
					Hooks::loadOffsets();
					if (Config::isDevConsoleEnabled()) DevConsole::install();
					install();
					Hooks::saveOffsets();
				}
				finish = std::chrono::high_resolution_clock::now();
			} catch (std::exception &e) {
				finish = std::chrono::high_resolution_clock::now();
				MessageBoxA(0, e.what(), Config::getFullStr().c_str(), MB_ICONERROR);
				Hooks::cleanup();
			}
			std::chrono::duration<double> elapsed = finish - start;
			printf(PROJECT_NAME " startup took %lf seconds\n", elapsed.count());
		}
		break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			Hooks::cleanup();
		default:
			break;
	}
	return TRUE;
}
