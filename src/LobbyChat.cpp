#include "LobbyChat.h"
#include "Hooks.h"
#include "Config.h"
#include "LobbyPackets.h"
#include <sstream>
#include "Utils.h"
#include "LobbyProtocol.h"
#include "W2App.h"
#include "RealTime.h"

void (__fastcall *addrLobbySendGreentext)(const char * msg, void * EDX, void* This, int a3, int a4);
char * addrMyNickname;
int (__fastcall *origLobbyClientCommands)(void* This, void* EDX, char **commandstrptr, char **argstrptr);
int __fastcall LobbyChat::hookLobbyClientCommands(void *This, void *EDX, char **commandstrptr, char **argstrptr) {
	std::string command = std::string(commandstrptr[0]);
	std::string args = std::string(argstrptr[0]);

	if(command == "realtime" || command == "rt") {
//		if(args.empty() || args == "list") {
			std::stringstream ss;
			ss << addrMyNickname << " is using " << Config::getFullStr();
			addrLobbySendGreentext(ss.str().c_str(), 0, This, 0, 0);
//		}
		return 1;
	}
	return origLobbyClientCommands(This, EDX, commandstrptr, argstrptr);
}

int (__fastcall *origLobbyHostCommands)(void *This, void *EDX, char **commandstrptr, char **argstrptr);
int __fastcall LobbyChat::hookLobbyHostCommands(void *This, void *EDX, char **commandstrptr, char **argstrptr) {
	std::string command = std::string(commandstrptr[0]);
	std::string args = std::string(argstrptr[0]);

	if(command == "realtime" || command == "rt") {
		if(args == "on") {
			RealTime::setActive(true);
			lobbyPrint((char*)"Enabled RealTime mode");
			LobbyProtocol::sendRealTimeStatusToAllPlayers();
			return 1;
		} else if(args == "off") {
			RealTime::setActive(false);
			lobbyPrint((char*)"Disabled RealTime mode");
			LobbyProtocol::sendRealTimeStatusToAllPlayers();
			return 1;
		} else if(args == "query") {
			lobbyPrint((char*)"Sending wkRealTime version query to players");
			LobbyProtocol::sendVersionQueryToAllPlayers();
		}
	}

	return origLobbyHostCommands(This, EDX, commandstrptr, argstrptr);
}


int (__stdcall *origConstructLobbyHostScreen)(int a1, int a2);
int __stdcall LobbyChat::hookConstructLobbyHostScreen(int a1, int a2) {
	auto ret = origConstructLobbyHostScreen(a1, a2);
	lobbyHostScreen = a1;
	return ret;
}

int (__stdcall *origConstructLobbyHostEndScreen)(DWORD a1, unsigned int a2, char a3, int a4);
int __stdcall LobbyChat::hookConstructLobbyHostEndScreen(DWORD a1, unsigned int a2, char a3, int a4) {
	auto ret = origConstructLobbyHostEndScreen(a1, a2, a3, a4);
	lobbyHostScreen = a1;
	return ret;
}

int (__fastcall *origDestructLobbyHostScreen)(void *This, int EDX, char a2);
int __fastcall LobbyChat::hookDestructLobbyHostScreen(void *This, int EDX, char a2) {
	lobbyHostScreen = 0;
	return origDestructLobbyHostScreen(This, EDX, a2);
}

int (__stdcall *origDestructLobbyHostEndScreen)(int a1);
int __stdcall LobbyChat::hookDestructLobbyHostEndScreen(int a1) {
	lobbyHostScreen = 0;
	return origDestructLobbyHostEndScreen(a1);
}


int (__stdcall *origConstructLobbyClientScreen)(int a1, int a2);
int __stdcall LobbyChat::hookConstructLobbyClientScreen(int a1, int a2) {
	auto ret = origConstructLobbyClientScreen(a1, a2);
	lobbyClientScreen = a1;
	return ret;
}

int (__fastcall *origDestructLobbyClientScreen)(void *This, int EDX, char a2);
int __fastcall LobbyChat::hookDestructLobbyClientScreen(void *This, int EDX, char a2) {
	lobbyClientScreen = 0;
	return origDestructLobbyClientScreen(This, EDX, a2);
}

int (__stdcall *origConstructLobbyClientEndScreen)(DWORD a1);
int __stdcall LobbyChat::hookConstructLobbyClientEndScreen(DWORD a1) {
	auto ret = origConstructLobbyClientEndScreen(a1);
	lobbyClientScreen = a1;
	return ret;
}

void (__fastcall *origDestructCWnd)(int This);
void __fastcall LobbyChat::hookDestructCWnd(int This) {
	if(lobbyClientScreen == This) {
		lobbyClientScreen = 0;
	}
	origDestructCWnd(This);
}

int (__stdcall *origLobbyDisplayMessage)(int a1, char *msg);
int __stdcall LobbyChat::hookLobbyDisplayMessage(int a1, char *msg) {
	return origLobbyDisplayMessage(a1, msg);
}



void LobbyChat::lobbyPrint(char * msg) {
	std::string buff = "SYS:" + Config::getModuleStr() + ":ALL:" + Config::getModuleStr() + ": ";
	buff += msg;
	if(LobbyPackets::isClient() && lobbyClientScreen) {
		origLobbyDisplayMessage((int)lobbyClientScreen + 0x10318, (char*)buff.c_str());
	} else if(LobbyPackets::isHost() && lobbyHostScreen) {
		origLobbyDisplayMessage((int)lobbyHostScreen + 0x10318, (char*)buff.c_str());
	}
}

void LobbyChat::install() {
	DWORD addrLobbyClientCommands = _ScanPattern("LobbyClientCommands", "\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x40\x53\x56\x8B\x75\x08\x8B\x06\x57\x8B\xD9\x68\x00\x00\x00\x00\x50\x89\x5C\x24\x1C\xE8\x00\x00\x00\x00\x83\xC4\x08\x85\xC0", "??????xxx????xx????xxxx????xxxxxxxxxxxxxx????xxxxxx????xxxxx");
	DWORD addrLobbyHostCommands = _ScanPattern("LobbyHostCommands", "\x55\x8B\xEC\x83\xE4\xF8\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00\x53\x56\x8B\x75\x08\x8B\x06\x57\x68\x00\x00\x00\x00\x50\x8B\xF9\xE8\x00\x00\x00\x00\x83\xC4\x08\x85\xC0\x0F\x84\x00\x00\x00\x00\x8B\x06\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00", "??????xx????xxx????xxxx????xx????xxxxxxxxx????xxxx????xxxxxxx????xxx????xx????");
	addrLobbySendGreentext =
		(void (__fastcall *)(const char*,void *,void *,int, int))
		_ScanPattern("LobbySendGreentext", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51\x56\x57\x8B\xF1\x68\x00\x00\x00\x00\x8D\x4C\x24\x0C\xE8\x00\x00\x00\x00\x80\x7C\x24\x00\x00", "???????xx????xxxx????xxxxxx????xxxxx????xxx??");
	addrMyNickname = *(char**)((DWORD)addrLobbySendGreentext + 0x4D);
	addrConstructLobbyHostScreen = _ScanPattern("ConstructLobbyHostScreen", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x5C\x53\x8B\x5C\x24\x74\x55\x8B\x6C\x24\x74\x57\x8D\xBD\x00\x00\x00\x00\x57\x53\x68\x00\x00\x00\x00", "???????xx????xxxx????xxxxxxxxxxxxxxxx????xxx????");
	addrConstructLobbyHostEndScreen = _ScanPattern("ConstructLobbyHostEndScreen", "\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x8B\x44\x24\x14\x64\x89\x25\x00\x00\x00\x00\x53\x8B\x5C\x24\x1C\x56\x57\x8B\x7C\x24\x1C\x53\x50\x57\xE8\x00\x00\x00\x00\x6A\x01\x8D\x8F\x00\x00\x00\x00\xC7\x44\x24\x00\x00\x00\x00\x00\x51", "??????xxx????xxxxxxxx????xxxxxxxxxxxxxxx????xxxx????xxx?????x");
	addrConstructLobbyHostEndScreenWrapper = _ScanPattern("ConstructLobbyHostEndScreenWrapper", "\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x53\x56\x57\x8B\x7C\x24\x1C\x8D\x9F\x00\x00\x00\x00\x53\x6A\x01\x68\x00\x00\x00\x00\x57\xE8\x00\x00\x00\x00\xC7\x44\x24\x00\x00\x00\x00\x00\x8D\xB7\x00\x00\x00\x00", "??????xxx????xxxx????xxxxxxxxx????xxxx????xx????xxx?????xx????");
	DWORD addrDestructLobbyHostEndScreen = _ScanPattern("DestructLobbyHostEndScreen", "\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x56\x8B\x74\x24\x14\xC7\x06\x00\x00\x00\x00\xC7\x46\x00\x00\x00\x00\x00\xC7\x44\x24\x00\x00\x00\x00\x00\x8B\x86\x00\x00\x00\x00\x85\xC0\x74\x09\x50\xE8\x00\x00\x00\x00\x83\xC4\x04", "??????xxx????xxxx????xxxxxxx????xx?????xxx?????xx????xxxxxx????xxx");
	DWORD addrConstructLobbyClientScreen = _ScanPattern("ConstructLobbyClientScreen", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x5C\x53\x8B\x5C\x24\x74\x55\x8B\x6C\x24\x74\x56\x57\x53\x55\xB9\x00\x00\x00\x00", "???????xx????xxxx????xxxxxxxxxxxxxxxxxx????");
	DWORD addrConstructLobbyClientEndScreen = _ScanPattern("ConstructLobbyClientEndScreen", "\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x53\x56\x57\x8B\x7C\x24\x1C\x6A\x01\x57\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8D\x87\x00\x00\x00\x00\x50\x33\xDB\x68\x00\x00\x00\x00\x53", "??????xxx????xxxx????xxxxxxxxxxx????x????xx????xxxx????x");
	DWORD addrLobbyDisplayMessage = _ScanPattern("LobbyDisplayMessage", "\x55\x8B\xEC\x83\xE4\xF8\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x30\x53\x56\x57\xE8\x00\x00\x00\x00\x33\xC9\x85\xC0\x0F\x95\xC1\x85\xC9\x75\x0A\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "??????xx????xxx????xxxx????xxxxxxx????xxxxxxxxxxxx????x????");
	DWORD addrDestructCWnd = _ScanPattern("DestructCWnd", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51\x53\x56\x57\x8B\xF9\x89\x7C\x24\x0C\xC7\x07\x00\x00\x00\x00\xC7\x44\x24\x00\x00\x00\x00\x00", "???????xx????xxxx????xxxxxxxxxxxx????xxx?????");
	DWORD addrConstructLobbyOfflineScreen = _ScanPattern("ConstructLobbyOfflineScreen", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x24\x55\x8B\x6C\x24\x38\x56\x57\x33\xFF\x57\x68\x00\x00\x00\x00\x55", "???????xx????xxxx????xxxxxxxxxxxxxx????x");
	DWORD * addrLobbyHostScreenVtable = *(DWORD**)(addrConstructLobbyHostScreen + 0x41);
	DWORD addrDestructLobbyHostScreen = addrLobbyHostScreenVtable[1];
	DWORD * addrLobbyClientScreenVtable = *(DWORD**)(addrConstructLobbyClientScreen + 0x3F);
	DWORD addrDestructLobbyClientScreen = addrLobbyClientScreenVtable[1];

	_HookDefault(LobbyClientCommands);
	_HookDefault(LobbyHostCommands);
	_HookDefault(ConstructLobbyHostScreen);
	_HookDefault(DestructLobbyHostScreen);
	_HookDefault(ConstructLobbyHostEndScreen);
	_HookDefault(DestructLobbyHostEndScreen);
	_HookDefault(ConstructLobbyClientScreen);
	_HookDefault(DestructLobbyClientScreen);
	_HookDefault(ConstructLobbyClientEndScreen);
	_HookDefault(DestructCWnd);
	_HookDefault(LobbyDisplayMessage);

	W2App::setAddrDdMain(*(DWORD*)(addrConstructLobbyHostScreen + 0x3DE));
}


DWORD LobbyChat::getLobbyHostScreen() {
	return lobbyHostScreen;
}

DWORD LobbyChat::getLobbyClientScreen() {
	return lobbyClientScreen;
}

void LobbyChat::sendGreentextMessage(std::string msg) {
	DWORD hostThis = LobbyChat::getLobbyHostScreen();
	if(hostThis) {
		addrLobbySendGreentext(msg.c_str(), 0, (void*)hostThis, 0, 0);
	}
}

