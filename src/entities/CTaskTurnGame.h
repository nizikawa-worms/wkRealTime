
#ifndef WKJELLYWORM_CTASKTURNGAME_H
#define WKJELLYWORM_CTASKTURNGAME_H

#include "CTask.h"
#include "../TaskMessageFifo.h"
#include "CTaskTeam.h"

class CTaskTurnGame : public CTask {
public:
	int unknown30; // 0x30
	int unknown34; // 0x34
	int unknown38; // 0x38
	int unknown3C; // 0x3C
	int unknown40; // 0x40
	int unknown44; // 0x44
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
	int unknown84; // 0x84
	int unknown88; // 0x88
	int unknown8C; // 0x8C
	int unknown90; // 0x90
	int unknown94; // 0x94
	int unknown98; // 0x98
	int unknown9C; // 0x9C
	int unknownA0; // 0xA0
	int unknownA4; // 0xA4
	int unknownA8; // 0xA8
	int unknownAC; // 0xAC
	int unknownB0; // 0xB0
	int unknownB4; // 0xB4
	int unknownB8; // 0xB8
	int unknownBC; // 0xBC
	int unknownC0; // 0xC0
	int unknownC4; // 0xC4
	int unknownC8; // 0xC8
	int unknownCC; // 0xCC
	int unknownD0; // 0xD0
	int unknownD4; // 0xD4
	int unknownD8; // 0xD8
	int unknownDC; // 0xDC
	int unknownE0; // 0xE0
	int unknownE4; // 0xE4
	int unknownE8; // 0xE8
	int unknownEC; // 0xEC
	int unknownF0; // 0xF0
	int unknownF4; // 0xF4
	int unknownF8; // 0xF8
	int unknownFC; // 0xFC
	int unknown100; // 0x100
	int unknown104; // 0x104
	int unknown108; // 0x108
	int unknown10C; // 0x10C
	int unknown110; // 0x110
	int unknown114; // 0x114
	int unknown118; // 0x118
	int unknown11C; // 0x11C
	int unknown120; // 0x120
	int unknown124; // 0x124
	int unknown128; // 0x128
	int current_team_1_unknown12C; // 0x12C
	int unknown130; // 0x130
	int current_team_2_unknown134; // 0x134
	int unknown138; // 0x138
	int unknown13C; // 0x13C
	int its_before_round_start_dword140; // 0x140
	int unknown144; // 0x144
	int unknown148; // 0x148
	int unknown14C; // 0x14C
	int unknown150; // 0x150
	int unknown154; // 0x154
	int unknown158; // 0x158
	int unknown15C; // 0x15C
	int unknown160; // 0x160
	int unknown164; // 0x164
	int unknown168; // 0x168
	int unknown16C; // 0x16C
	int unknown170; // 0x170
	int unknown174; // 0x174
	int unknown178; // 0x178
	int unknown17C; // 0x17C
	int unknown180; // 0x180
	int round_timer_dword184; // 0x184
	int turn_timer1_unknown188; // 0x188
	int turn_timer2_unknown18C; // 0x18C
	int unknown190; // 0x190
	int unknown194; // 0x194
	int unknown198; // 0x198
	int unknown19C; // 0x19C
	int unknown1A0; // 0x1A0
	int unknown1A4; // 0x1A4
	int unknown1A8; // 0x1A8
	int unknown1AC; // 0x1AC
	int unknown1B0; // 0x1B0
	int unknown1B4; // 0x1B4
	int unknown1B8; // 0x1B8
	int unknown1BC; // 0x1BC
	int unknown1C0; // 0x1C0
	int unknown1C4; // 0x1C4
	int unknown1C8; // 0x1C8
	int unknown1CC; // 0x1CC
	int unknown1D0; // 0x1D0
	int unknown1D4; // 0x1D4
	int unknown1D8; // 0x1D8
	int unknown1DC; // 0x1DC
	int unknown1E0; // 0x1E0
	int unknown1E4; // 0x1E4
	int unknown1E8; // 0x1E8
	int unknown1EC; // 0x1EC
	int unknown1F0; // 0x1F0
	int unknown1F4; // 0x1F4
	int unknown1F8; // 0x1F8
	int unknown1FC; // 0x1FC
	int unknown200; // 0x200
	int unknown204; // 0x204
	int unknown208; // 0x208
	int unknown20C; // 0x20C
	int unknown210; // 0x210
	int unknown214; // 0x214
	int unknown218; // 0x218
	int unknown21C; // 0x21C
	int unknown220; // 0x220
	int unknown224; // 0x224
	int unknown228; // 0x228
	int unknown22C; // 0x22C
	int unknown230; // 0x230
	int unknown234; // 0x234
	int unknown238; // 0x238
	int unknown23C; // 0x23C
	int unknown240; // 0x240
	int unknown244; // 0x244
	int unknown248; // 0x248
	int unknown24C; // 0x24C
	int unknown250; // 0x250
	int unknown254; // 0x254
	int unknown258; // 0x258
	int unknown25C; // 0x25C
	int unknown260; // 0x260
	int unknown264; // 0x264
	int unknown268; // 0x268
	int unknown26C; // 0x26C
	int unknown270; // 0x270
	int unknown274; // 0x274
	int unknown278; // 0x278
	int unknown27C; // 0x27C
	int unknown280; // 0x280
	int unknown284; // 0x284
	int unknown288; // 0x288
	int unknown28C; // 0x28C
	int unknown290; // 0x290
	int unknown294; // 0x294
	int unknown298; // 0x298
	int unknown29C; // 0x29C
	int unknown2A0; // 0x2A0
	int unknown2A4; // 0x2A4
	int unknown2A8; // 0x2A8
	int unknown2AC; // 0x2AC
	int unknown2B0; // 0x2B0
	int unknown2B4; // 0x2B4
	int unknown2B8; // 0x2B8
	int unknown2BC; // 0x2BC
	int unknown2C0; // 0x2C0
	int unknown2C4; // 0x2C4
	int unknown2C8; // 0x2C8
	int unknown2CC; // 0x2CC
	int unknown2D0; // 0x2D0
	int unknown2D4; // 0x2D4
	int unknown2D8; // 0x2D8
	int unknown2DC; // 0x2DC


private:
	static int __fastcall hookTurnHandleMessage(CTaskTurnGame * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data);
	static DWORD __stdcall hookSetActiveWorm();
	static inline bool flagActivatingTeam = false;
	static void spoofTeamMessage(CTaskTurnGame *This, const void *data, Constants::TaskMessage mtype);
public:
	static void install();
};

#pragma pack(push, 1)
struct GameState {
//	DWORD frame;
//	DWORD roundTimer;
	DWORD wind;

	GameState() = delete;
	GameState(CTaskTurnGame * turngame);
	bool apply(CTaskTurnGame * turngame);
};

struct WeaponFinishedData {
	DWORD team;
	DWORD worm;
	Constants::Weapon weapon;
};

#pragma pack(pop)

#endif //WKJELLYWORM_CTASKTURNGAME_H
