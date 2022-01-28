

#include "CTaskTeam.h"
#include "../W2App.h"
#include "../Hooks.h"
#include "../Debugf.h"
#include "../RealTime.h"
#include "CTaskWorm.h"

int (__fastcall *origTeamHandleMessage)(CTaskTeam * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data);
int __fastcall CTaskTeam::hookTeamHandleMessage(CTaskTeam * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data) {
	if(RealTime::isActive()) {
		switch (mtype) {
			case Constants::TaskMessage_CursorMoved: {
				DWORD senderTeam = *(DWORD *) data;
				if (senderTeam == This->team_number_dword38) {
					if (!This->isOwnedByMe()) return 0;
				}
			}
			break;
			case Constants::TaskMessage_FrameFinish:
			case Constants::TaskMessage_StartTurn:
			case Constants::TaskMessage_FinishTurn:
				This->fixCurrentWorm();
			default:
				break;
		}
	}
	return origTeamHandleMessage(This, EDX, sender, mtype, size, data);
}

DWORD TeamHandle0x83Message_patch1_ret1;
DWORD TeamHandle0x83Message_patch1_ret2;
void __declspec(naked) TeamHandle0x83Message_patch1() {
	_asm test eax, eax
	_asm jnz continuefunc
	_asm jmp TeamHandle0x83Message_patch1_ret2
_asm continuefunc:
	_asm mov     edx, [eax]
	_asm mov     edx, [edx+0x0C]
	_asm lea     ecx, [esp+0x1C]
	_asm jmp TeamHandle0x83Message_patch1_ret1
}

void CTaskTeam::install() {
	DWORD addrTeamHandleMessage = _ScanPattern("TeamHandleMessage", "\x55\x8B\xEC\x83\xE4\xF8\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x81\xEC\x00\x00\x00\x00\x53\x56\x57\x8B\x7D\x0C\x8D\x47\xFE\x3D\x00\x00\x00\x00\x8B\xD9\x0F\x87\x00\x00\x00\x00", "??????xx????xxx????xxxx????xx????xxxxxxxxxx????xxxx????");
	DWORD addrTeamHandle0x83Message = _ScanPattern("TeamHandle0x83Message", "\x81\xEC\x00\x00\x00\x00\x53\x8B\x9C\x24\x00\x00\x00\x00\x8B\x43\x2C\x8B\x48\x24\x0F\xBE\x91\x00\x00\x00\x00\x3B\x53\x40\x55\x56\x57\x0F\x85\x00\x00\x00\x00\x83\xB8\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00", "??????xxxx????xxxxxxxxx????xxxxxxxx????xx?????xx????");
	_HookDefault(TeamHandleMessage);

	TeamHandle0x83Message_patch1_ret1 = addrTeamHandle0x83Message + 0xE2;
	TeamHandle0x83Message_patch1_ret2 = addrTeamHandle0x83Message + 0x413;
	Hooks::hookAsm(addrTeamHandle0x83Message + 0xD9, (DWORD)&TeamHandle0x83Message_patch1);
}

bool CTaskTeam::isOwnedByMe() {
	char mymachine = *(char *) (W2App::getAddrDdMain() + 0xD9DC + 0x40);
	return this->owner_byte40 == mymachine;
}

bool CTaskTeam::fixCurrentWorm() {
	if(this == nullptr) return false;
	int first_worm_number = 0;
	for (auto child : this->children) {
		if(child == nullptr) continue;
		CTaskWorm *worm = (CTaskWorm *) child;
		if (!first_worm_number) first_worm_number = worm->wormnumber_dword100;
		if (this->current_worm_number_dword48 == worm->wormnumber_dword100) {
			return true;
		}
	}
	if (first_worm_number) {
		debugf("*bugfix* Setting current worm number of team %d to %d\n", this->team_number_dword38, first_worm_number);
		this->current_worm_number_dword48 = first_worm_number;
		return true;
	} else {
		debugf("team %d has no matching current worm\n", this->team_number_dword38);
		return false;
	}
}

