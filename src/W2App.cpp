#include "W2App.h"
#include "Hooks.h"
#include "Config.h"
#include "LobbyChat.h"
#include "RealTime.h"

DWORD origInitializeW2App;
DWORD __stdcall W2App::hookInitializeW2App(DWORD DD_Game_a2, DWORD DD_Display_a3, DWORD DS_Sound_a4, DWORD DD_Keyboard_a5, DWORD DD_Mouse_a6, DWORD WAV_CDrom_a7, DWORD WS_GameNet_a8) {
	DWORD DD_W2Wrapper, retv;
	_asm mov DD_W2Wrapper, edi

	addrDDGame = DD_Game_a2;
	addrDDDisplay = DD_Display_a3;
	addrDSSound = DS_Sound_a4;
	addrDDKeyboard = DD_Keyboard_a5;
	addrDDMouse = DD_Mouse_a6;
	addrWavCDRom = WAV_CDrom_a7;
	addrWSGameNet = WS_GameNet_a8;
	addrW2Wrapper = DD_W2Wrapper;


	_asm push WS_GameNet_a8
	_asm push WAV_CDrom_a7
	_asm push DD_Mouse_a6
	_asm push DD_Keyboard_a5
	_asm push DS_Sound_a4
	_asm push DD_Display_a3
	_asm push DD_Game_a2
	_asm mov edi, DD_W2Wrapper
	_asm call origInitializeW2App
	_asm mov retv, eax

	return retv;
}


DWORD (__stdcall *origConstructGameGlobal)(int ddgame);
DWORD __stdcall W2App::hookConstructGameGlobal(DWORD ddgame) {
	auto ret = origConstructGameGlobal(ddgame);
	addrGameGlobal = *(DWORD*)(ddgame + 0x488);


	return ret;
}

DWORD (__fastcall *origDestroyGameGlobal)(int This, int EDX);
DWORD __fastcall W2App::hookDestroyGameGlobal(int This, int EDX) {
	addrGameGlobal = 0;
	auto ret = origDestroyGameGlobal(This, EDX);
	addrDDDisplay = addrDSSound = addrDDKeyboard = addrDDMouse = addrWavCDRom = addrWSGameNet = addrDDGame = 0;

	RealTime::onDestructGameGlobal();

	return ret;
}


void W2App::install() {
	DWORD addrInitializeW2App = _ScanPattern("InitializeW2App", "\x6A\xFF\x64\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x53\x8B\x5C\x24\x1C\x55\x8B\x6C\x24\x1C\x56\x8B\x74\x24\x1C\x33\xC0\x89\x86\x00\x00\x00\x00\x89\x44\x24\x14\x89\x86\x00\x00\x00\x00\x8B\xC7\xC7\x06\x00\x00\x00\x00\x89\x9E\x00\x00\x00\x00\x89\xAE\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "????????x????xxxx????xxxxxxxxxxxxxxxxxxx????xxxxxx????xxxx????xx????xx????x????");
	DWORD addrConstructGameGlobal = _ScanPattern("ConstructGameGlobal", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x24\x53\x55\x8B\x6C\x24\x3C\x8B\x85\x00\x00\x00\x00\x8B\x48\x24", "???????xx????xxxx????xxxxxxxxxxx????xxx");
	DWORD addrDestroyGameGlobal = _ScanPattern("DestroyGameGlobal", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51\x56\x8B\xF1\x89\x74\x24\x04\xC7\x06\x00\x00\x00\x00\x8B\xC6\xC7\x44\x24\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x83\x78\x5C\x00\x75\x07\x8B\xC6\xE8\x00\x00\x00\x00\x8B\xC6\xE8\x00\x00\x00\x00\x8B\x86\x00\x00\x00\x00\x85\xC0\x74\x09\x50\xE8\x00\x00\x00\x00\x83\xC4\x04", "???????xx????xxxx????xxxxxxxxxx????xxxxx?????x????x????xxxxxxxxx????xxx????xx????xxxxxx????xxx");
	DWORD addrReferencesGameinfo =  _ScanPattern("ReferencesGameinfo", "\x80\x3D\x00\x00\x00\x00\x00\x75\x15\x56\x8B\x35\x00\x00\x00\x00\x6A\x01\xFF\xD6\x80\x3D\x00\x00\x00\x00\x00\x74\xF3\x5E\x8B\x44\x24\x04\x50\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xC2\x04\x00", "???????xxxxx????xxxxxx?????xxxxxxxxx????x????xxx");
	addrGameinfoObject =  *(DWORD*)(addrReferencesGameinfo + 0x24);

	_HookDefault(InitializeW2App);
	_HookDefault(ConstructGameGlobal);
	_HookDefault(DestroyGameGlobal);
}

DWORD W2App::getAddrDdGame() {
	return addrDDGame;
}

DWORD W2App::getAddrDdDisplay() {
	return addrDDDisplay;
}

DWORD W2App::getAddrDsSound() {
	return addrDSSound;
}

DWORD W2App::getAddrDdKeyboard() {
	return addrDDKeyboard;
}

DWORD W2App::getAddrDdMouse() {
	return addrDDMouse;
}

DWORD W2App::getAddrWavCdRom() {
	return addrWavCDRom;
}

DWORD W2App::getAddrWsGameNet() {
	return addrWSGameNet;
}

DWORD W2App::getAddrGameinfoObject() {
	return addrGameinfoObject;
}

DWORD W2App::getAddrW2Wrapper() {
	return addrW2Wrapper;
}

DWORD W2App::getAddrGameGlobal() {
	return addrGameGlobal;
}


DWORD W2App::getAddrDdMain() {
	return addrDDMain;
}

void W2App::setAddrDdMain(DWORD addrDdMain) {
	addrDDMain = addrDdMain;
}
