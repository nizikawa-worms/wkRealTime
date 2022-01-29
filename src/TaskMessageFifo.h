

#ifndef WKREALTIME_TASKMESSAGEFIFO_H
#define WKREALTIME_TASKMESSAGEFIFO_H

#include <memory>
#include "Constants.h"
#include "Drawing.h"

typedef unsigned long       DWORD;

struct TaskMessageEntry {
	DWORD totalsize_dword0;
	TaskMessageEntry * nextentry_dword4;
	DWORD type_dword8;
	unsigned char datac[];
};


struct FifoDebugDisplay {
	BitmapTextbox textbox;
	BitmapImage * bmp;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	bool visible = false;
};

class TaskMessageFifo {
public:
	DWORD dword0; // maybe backing storage
	DWORD capacity_dword4; // maybe max size
	DWORD end_dword8; // current size
	DWORD start_dwordC; //unk
	DWORD dword10; //maybe pointer to current data end?
	DWORD data_start_dword14; //maybe pointer to data start
	DWORD num_elements_dword18; //number of elements

	static int callCopyFiFo(TaskMessageFifo * src, TaskMessageFifo * dst);
	static int callFifoMakeSpace(TaskMessageFifo * fifo, size_t tsize);
	static DWORD callTaskMessageSend(TaskMessageFifo * fifo, DWORD msize, Constants::TaskMessage mtype, void * data);
	static std::map<TaskMessageFifo*, std::unique_ptr<FifoDebugDisplay>> debugFifoTextboxes;

	void printInfo();
	void printContents();
	DWORD getSize();

	std::pair<int, int> enqueueDebugDisplay(std::string name, int x, int y);
	static void onTurnGameRenderScene();
	static void onDestructGameGlobal();

	static void install();
};

#endif //WKREALTIME_TASKMESSAGEFIFO_H
