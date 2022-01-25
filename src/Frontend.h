

#ifndef WKREALTIME_FRONTEND_H
#define WKREALTIME_FRONTEND_H

#include <utility>
#include <vector>
typedef void CWnd;
typedef void CDataExchange;
typedef unsigned long DWORD;

class Frontend {
private:
	static void __stdcall hookFrontendChangeScreen(int screen);
	static inline int currentScreen = 0;
public:
	static int callMessageBox(const char * message, int a2, int a3);
	static void install();
	static int getCurrentScreen();
};


#endif //WKREALTIME_FRONTEND_H
