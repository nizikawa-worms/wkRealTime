
#ifndef WKREALTIME_GAMENET_H
#define WKREALTIME_GAMENET_H

#include "TaskMessageFifo.h"
#include "entities/CTaskTurnGame.h"

typedef unsigned long       DWORD;
struct ChatMessage {
	DWORD machineid;
	DWORD unk1;
	DWORD unk2;
	char text[284];
};

class GameNet {
private:
	static const int inactiveWormsInterval = 25;
	static const int newFifoSize = 0xA00000;

	static int callSendOutOfOrder(DWORD gamenet);
	static int __stdcall hookGameNetUnknown();
	static void hookGameNetUnknown_c(DWORD ddgame);
//	static int __stdcall hookHandleRealtimeControlMessages(DWORD ddgame);
	static int callFifoGamenetGetEvent(int machine, size_t * msize, DWORD gamenet, int * mtype, unsigned char *data);
	static void __stdcall injectRealtimeFifoEvents();
	static void __stdcall gamenetmain_patch1_c(DWORD ddgame);
	static void sendWormStates(TaskMessageFifo *fifo, CTaskTurnGame *turngame, bool sendInactiveWorms=false);
	static void __stdcall gamenetmain_patch2_c();
public:
	static void install();
	static void enqueueDebugFifo();
};


#endif //WKREALTIME_GAMENET_H
