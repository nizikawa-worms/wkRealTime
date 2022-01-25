#ifndef WKREALTIME_W2APP_H
#define WKREALTIME_W2APP_H

#include <vector>

typedef unsigned long DWORD;
class W2App {
private:
	static inline DWORD addrDDGame = 0;
	static inline DWORD addrDDDisplay = 0;
	static inline DWORD addrDSSound = 0;
	static inline DWORD addrDDKeyboard = 0;
	static inline DWORD addrDDMouse = 0;
	static inline DWORD addrWavCDRom = 0;
	static inline DWORD addrWSGameNet = 0;
	static inline DWORD addrW2Wrapper = 0;
	static inline DWORD addrGameinfoObject = 0;
	static inline DWORD addrGameGlobal = 0;
	static inline DWORD addrDDMain = 0;

	static DWORD __stdcall hookInitializeW2App(unsigned long DD_Game_a2, unsigned long DD_Display_a3, unsigned long DS_Sound_a4, unsigned long DD_Keyboard_a5, unsigned long DD_Mouse_a6, unsigned long WAV_CDrom_a7, unsigned long WS_GameNet_a8);
	static DWORD __stdcall hookConstructGameGlobal(DWORD ddgame);
	static DWORD __fastcall hookDestroyGameGlobal(int This, int EDX);
public:
	static void install();

	static DWORD getAddrDdGame();
	static DWORD getAddrDdDisplay();
	static DWORD getAddrDsSound();
	static DWORD getAddrDdKeyboard();
	static DWORD getAddrDdMouse();
	static DWORD getAddrWavCdRom();
	static DWORD getAddrWsGameNet();
	static DWORD getAddrGameinfoObject();
	static DWORD getAddrW2Wrapper();
	static DWORD getAddrGameGlobal();

	static DWORD getAddrDdMain();

	static void setAddrDdMain(DWORD addrDdMain);
};


#endif //WKREALTIME_W2APP_H
