#ifndef WKREALTIME_CTASK_H
#define WKREALTIME_CTASK_H



#include "../Constants.h"
#include "CList.h"
#include <functional>
#include <array>
typedef unsigned long       DWORD;
class CTask;
typedef CList<CTask*> CTaskList;

class CTask {
public:
	virtual int vtable0(int a2) = 0;
	virtual int vtable4_Free(int heap) = 0;
	virtual int vtable8_HandleMessage(CTask * sender, Constants::TaskMessage mtype, size_t size, void * data) = 0;
	virtual int vtableC(int a2, int a3, int a4) = 0;
	virtual int vtable10(int a2, int a3, int a4) = 0;
	virtual int vtable14(int a2) = 0;
	virtual int vtable18() = 0;

	//int vtable;
	CTask * parent; // 0x4

//	int unknown8; // 0x8
//	int unknownC; // 0xC
//	int children_num; // 0x10
//	CTask ** children_ptrs; // 0x14
//	int unknown18; // 0x18
	CTaskList children;

	int unknown1C; // 0x1C
	Constants::ClassType classtype; // 0x20
	int unknown24; // 0x24
	int unknown28; // 0x28
	int gameglobal2c; // 0x2C

	void traverse(const std::function<void(CTask*, const int level)>& cb, const int level=0) {
		cb(this, level);
		for(auto child : this->children) {
			if(child) {
				child->traverse(cb, level+1);
			}
		}
	}
};


#endif //WKREALTIME_CTASK_H
