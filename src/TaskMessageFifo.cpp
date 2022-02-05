
#include <cstdio>
#include "TaskMessageFifo.h"
#include "Utils.h"
#include "Debugf.h"
#include "Hooks.h"
#include "Constants.h"
#include "Drawing.h"
#include "W2App.h"
#include "RealTime.h"

std::map<TaskMessageFifo*, std::unique_ptr<FifoDebugDisplay>> TaskMessageFifo::debugFifoTextboxes;

void TaskMessageFifo::printInfo() {
	debugf("TaskMessageFifo: this: 0x%X dword0: 0x%X capacity_dword4: 0x%X dword8: 0x%X dwordC: 0x%X dword10: 0x%X dword14: 0x%X dword18: 0x%X\n", (DWORD)this, this->dword0, this->capacity_dword4, this->end_dword8, this->start_dwordC, this->dword10, this->data_start_dword14, this->num_elements_dword18);
}

void TaskMessageFifo::printContents() {
	TaskMessageEntry * entry = (TaskMessageEntry*) this->data_start_dword14;
	int i = 0;
	while(entry) {
		DWORD size = entry->totalsize_dword0 - 4;
		printf("\t%d: entry: 0x%08X tsize: %d size: %d, capacity_dword4: 0x%08X type: %d\n", i, (DWORD)entry, entry->totalsize_dword0, size, entry->nextentry_dword4, entry->type_dword8);
		if(size) {
			Utils::hexDump("Payload", entry->datac, size);
		}
		entry = entry->nextentry_dword4;
		i++;
	}
}

DWORD addrCopyFiFo;
int TaskMessageFifo::callCopyFiFo(TaskMessageFifo * src, TaskMessageFifo * dst) {
	int retv;
	_asm mov eax, src
	_asm mov esi, dst
	_asm call addrCopyFiFo
	_asm mov retv, eax

	return retv;
}


DWORD addrFifoMakeSpace;
int TaskMessageFifo::callFifoMakeSpace(TaskMessageFifo * fifo, size_t tsize) {
	int retv;

	_asm mov eax, fifo
	_asm mov ecx, tsize
	_asm call addrFifoMakeSpace
	_asm mov retv, eax

	return retv;
}

DWORD addrTaskMessageSend;
DWORD TaskMessageFifo::callTaskMessageSend(TaskMessageFifo * fifo, DWORD msize, Constants::TaskMessage mtype, void * data) {
	DWORD retv;
	_asm mov eax, fifo
	_asm mov ecx, msize
	_asm push data
	_asm push mtype
	_asm call addrTaskMessageSend
	_asm mov retv, eax

	return retv;
}

void TaskMessageFifo::install() {
	addrCopyFiFo = _ScanPattern("CopyFiFo", "\x53\x8B\x58\x14\x85\xDB\x55\x57\x74\x72\x8D\x9B\x00\x00\x00\x00\x8B\x3B\x8B\x56\x08\x8B\x46\x0C\x8D\x4F\x0B\x83\xE1\xFC\x3B\xD0", "??????xxxxxx????xxxxxxxxxxxxxxxx");
	addrFifoMakeSpace = _ScanPattern("FifoMakeSpace", "\x53\x56\x8B\x70\x08\x57\x8B\xF9\x8B\x48\x0C\x8D\x57\x0B\x83\xE2\xFC\x3B\xF1\x8D\x1C\x16\x7D\x0A\x3B\xD9\x7C\x17\x5F\x5E\x33\xC0\x5B\xC3", "??????xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	addrTaskMessageSend = Hooks::scanPattern("TaskMessageSend", "\x56\x8B\xF1\x83\xC6\x03\x83\xE6\xFC\x8D\x4E\x04\xE8\x00\x00\x00\x00\x85\xC0\x75\x04\x5E\xC2\x08\x00\x85\xF6\x8B\x4C\x24\x08\x89\x08\x74\x12\x8B\x54\x24\x0C\x56", "??????xxxxxxx????xxxxxxxxxxxxxxxxxxxxxxx");
}

std::pair<int, int> TaskMessageFifo::enqueueDebugDisplay(std::string name, int x, int y) {
	if(debugFifoTextboxes.find(this) == debugFifoTextboxes.end()) {
		debugFifoTextboxes[this] = std::make_unique<FifoDebugDisplay>(FifoDebugDisplay{BitmapTextbox(), 0, 0, 0, 0, false});
	}

	auto & entry = debugFifoTextboxes[this];
	int width, height;

	std::string contents;
	if(this->data_start_dword14) {
		TaskMessageEntry *fifoentry = (TaskMessageEntry *) this->data_start_dword14;
		int i = 0;
		while (fifoentry) {
//			DWORD size = fifoentry->totalsize_dword0 - 4;
//			contents += std::format("{}({}) ", fifoentry->type_dword8, size);
			contents += std::format("{} ", (DWORD)fifoentry->type_dword8);
			fifoentry = fifoentry->nextentry_dword4;
			i++;
			if(i > 20) {
				contents += "...";
				break;
			}
		}
	}

	entry->bmp = entry->textbox.setText((char*)std::format("{} num: {} size: {}, {}", name, this->num_elements_dword18, getSize(), contents).c_str(), &width, &height);
	entry->x = x;
	entry->y = y;
	entry->width = width;
	entry->height = height;
	entry->visible = true;

	return {width, height};
}

void TaskMessageFifo::onTurnGameRenderScene() {
	if(!RealTime::isDebugFifo()) return;
	DWORD gameglobal = W2App::getAddrGameGlobal();
	DWORD screenw = *(DWORD *) (gameglobal + 0x77ac);
	DWORD screenh = *(DWORD *) (gameglobal + 0x77b4);

	for(auto & it : debugFifoTextboxes) {
		auto & entry = it.second;
//		if(entry->visible) {
			entry->bmp->drawScreen((-screenh / 2 + 20 + entry->y) << 16, 10, (-screenw / 2 + entry->width/2 + entry->x) << 16, 0x0, entry->width, entry->height, 0x0);
//			entry->visible = false;
//		}
	}
}

void TaskMessageFifo::onDestructGameGlobal() {
	debugFifoTextboxes.clear();
}

DWORD TaskMessageFifo::getSize() {
	if(end_dword8 < start_dwordC)
		return end_dword8 + capacity_dword4 - start_dwordC;
	else
		return end_dword8 - start_dwordC;
}
