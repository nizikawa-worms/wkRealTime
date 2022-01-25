

#include "CTaskTeam.h"
#include "../W2App.h"
#include "../Hooks.h"
#include "../Debugf.h"
#include "../RealTime.h"

int (__fastcall *origTeamHandleMessage)(CTaskTeam * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data);
int __fastcall CTaskTeam::hookTeamHandleMessage(CTaskTeam * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data) {
	switch(mtype) {
		case Constants::TaskMessage_CursorMoved: {
			////005573F1 - writing cursor XY to gameglobal
			// bad fix for recursive teleport cursor movement
			if(RealTime::isActive()) {
				DWORD senderTeam  = *(DWORD*)data;
//				DWORD posX  = *(DWORD*)((DWORD)data+4);
//				DWORD posY  = *(DWORD*)((DWORD)data+8);
//				debugf("Cursor moved, senderTeam: %d posX: %d posY: %d\n", senderTeam, posX, posY);
				if(senderTeam == This->team_number_dword38) {
					if(!This->isOwnedByMe()) return 0;
				}
			}
		}
		default: break;
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

