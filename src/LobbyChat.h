
#ifndef WKREALTIME_LOBBYCHAT_H
#define WKREALTIME_LOBBYCHAT_H

#include <string>
#include <vector>

typedef unsigned long       DWORD;
class LobbyChat {
private:
	static inline DWORD lobbyHostScreen = 0;
	static inline DWORD lobbyClientScreen = 0;

	static int __stdcall hookConstructLobbyHostScreen(int a1, int a2);
	static int __fastcall hookDestructLobbyHostScreen(void *This, int EDX, char a2);
	static int __stdcall hookConstructLobbyHostEndScreen(DWORD a1, unsigned int a2, char a3, int a4);
	static int __stdcall hookDestructLobbyHostEndScreen(int a1);

	static int __stdcall hookConstructLobbyClientScreen(int a1, int a2);
	static int __fastcall hookDestructLobbyClientScreen(void *This, int EDX, char a2);
	static int __stdcall hookConstructLobbyClientEndScreen(DWORD a1);
	static void __fastcall hookDestructCWnd(int This);

	static int __fastcall hookLobbyClientCommands(void *This, void *EDX, char **commandstrptr, char **argstrptr);
	static int __fastcall hookLobbyHostCommands(void *This, void *EDX, char **commandstrptr, char **argstrptr);
	static int __stdcall hookLobbyDisplayMessage(int a1, char *msg);

	static inline DWORD addrConstructLobbyHostScreen = 0;
	static inline DWORD addrConstructLobbyHostEndScreen = 0;
	static inline DWORD addrConstructLobbyHostEndScreenWrapper = 0;

public:
	static void install();
	static void lobbyPrint(char * msg);

	static DWORD getLobbyHostScreen();
	static DWORD getLobbyClientScreen();

	static void sendGreentextMessage(std::string msg);
};


#endif //WKREALTIME_LOBBYCHAT_H
