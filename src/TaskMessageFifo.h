

#ifndef WKTOOLASSIST_TASKMESSAGEFIFO_H
#define WKTOOLASSIST_TASKMESSAGEFIFO_H

#include "Constants.h"

typedef unsigned long       DWORD;

struct TaskMessageEntry {
	DWORD totalsize_dword0;
	DWORD dword4;
	DWORD type_dword8;
	unsigned char datac[];
};

class TaskMessageFifo {
public:
	DWORD dword0; // maybe backing storage
	DWORD dword4; // maybe max size / num elements
	DWORD size_dword8; // current size
	DWORD dwordC; //unk
	DWORD dword10; //maybe pointer to current data end?
	DWORD data_start_dword14; //maybe pointer to data start
	DWORD num_elements_dword18; //number of elements

	static int callCopyFiFo(TaskMessageFifo * src, TaskMessageFifo * dst);
	static int callFifoMakeSpace(TaskMessageFifo * fifo, size_t tsize);
	static DWORD callTaskMessageSend(TaskMessageFifo * fifo, DWORD msize, Constants::TaskMessage mtype, void * data);

	void printInfo();
	void printContents();
	static void install();
};

#endif //WKTOOLASSIST_TASKMESSAGEFIFO_H
