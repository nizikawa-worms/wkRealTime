#ifndef WKWORMORDER_CGAMETASK_H
#define WKWORMORDER_CGAMETASK_H


#include "CTask.h"

class CGameTask : public CTask {
public:
	virtual int vtable1C(int a2, int a3) = 0;
	virtual int vtable20(int a2, int a3) = 0;
	virtual int vtable24(int a2) = 0;
	virtual int vtable28(int a2) = 0;
	virtual int vtable2C_OnSink(int a2, int a3) = 0;
	virtual int vtable30() = 0;
	virtual int vtable34() = 0;
	virtual int vtable38_SetState(int a2) = 0;
	virtual int vtable3C() = 0;
	virtual int vtable40() = 0;
	virtual int vtable44(int a2, int a3, int a4) = 0;
	virtual int vtable48() = 0;



	int unknown30; // 0x30
	int unknown34; // 0x34
	int unknown38; // 0x38
	int unknown3C; // 0x3C
	int unknown40; // 0x40
	int state_dword44; // 0x44
	int unknown48; // 0x48
	int unknown4C; // 0x4C
	int unknown50; // 0x50
	int unknown54; // 0x54
	int unknown58; // 0x58
	int unknown5C; // 0x5C
	int unknown60; // 0x60
	int unknown64; // 0x64
	int unknown68; // 0x68
	int unknown6C; // 0x6C
	int unknown70; // 0x70
	int unknown74; // 0x74
	int unknown78; // 0x78
	int unknown7C; // 0x7C
	int unknown80; // 0x80
	int posX; // 0x84
	int posY; // 0x88
	int unknown8C; // 0x8C
	int speedX; // 0x90
	int speedY; // 0x94
	int unknown98; // 0x98
	int unknown9C; // 0x9C
	int unknownA0; // 0xA0
	int unknownA4; // 0xA4
	int unknownA8; // 0xA8
	int unknownAC; // 0xAC
	int unknownB0; // 0xB0
	int unknownB4; // 0xB4
	int unknownB8; // 0xB8
	int attached_to_rope_unknownBC; // 0xBC
	int rope_anchorX_dwordC0; // 0xC0
	int rope_anchorY_dwordC4; // 0xC4
	int rope_length1_unknownC8; // 0xC8
	int unknownCC; // 0xCC
	int rope_length2_unknownD0; // 0xD0
	int unknownD4; // 0xD4
	int unknownD8; // 0xD8
	int unknownDC; // 0xDC
	int unknownE0; // 0xE0
	int unknownE4; // 0xE4
	int unknownE8; // 0xE8
	int unknownEC; // 0xEC
};


#endif //WKWORMORDER_CGAMETASK_H
