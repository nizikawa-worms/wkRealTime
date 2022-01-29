
#include "CTaskWorm.h"
#include "../Hooks.h"
#include "../Debugf.h"
#include "CTaskTurnGame.h"
#include "CTaskTeam.h"
#include "../W2App.h"
#include "../RealTime.h"

int (__fastcall *origWormHandleMessage)(CTaskWorm * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data);
int __fastcall CTaskWorm::hookWormHandleMessage(CTaskWorm * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data) {
	switch(mtype){
		case Constants::TaskMessage_FrameFinish:
			if(This->selected_weapon_entry_ptr36C == 0) {
				DWORD gameglobal = W2App::getAddrGameGlobal();
				DWORD weaponTable = *(DWORD*)(gameglobal + 0x510);
				This->selected_weapon_entry_ptr36C = weaponTable + 464 * This->selected_weapon_unknown170;
			}
			break;
		case Constants::TaskMessage_RenderScene:
			if(Config::getColorMod() == 2 || (Config::getColorMod() == 1 && RealTime::isActive())){
				// This enables ColorMod (originally from RubberWorm)
				DWORD gameglobal = W2App::getAddrGameGlobal();
				DWORD *colors = (DWORD*)(gameglobal + 0x72D8 + 0x30); // 0x7248 in 3.7.2
				DWORD oldColor = colors[7];
				colors[7] = colors[This->color_dword10C + 1];
				int retVal = origWormHandleMessage(This, EDX, sender, mtype, size, data);
				colors[7] = oldColor;
				return retVal;
			}
			break;
		case Constants::TaskMessage_WormState: {
			WormState *state = (WormState *) data;
			state->apply(This);
			return 0;
		}
		default: break;
	}
	return origWormHandleMessage(This, EDX, sender, mtype, size, data);
}

WormState::WormState(CTaskWorm * worm) {
	this->team = worm->teamnumber_dwordFC;
	this->worm = worm->wormnumber_dword100;
	this->posY = worm->posY;
	this->posX = worm->posX;
	this->speedY = worm->speedY;
	this->speedX = worm->speedX;
	this->facingDirection = worm->facing_direction_dword1A8;

	this->state = worm->state_dword44;
	this->stateCounter = worm->state_counter_dword164;

//	this->attachedToRope = worm->attached_to_rope_unknownBC;
	this->shootingRope = worm->is_shooting_rope_dword258;
	this->nextRopeAngle = worm->rope_next_angle_unknown264;
	this->ropeLength1 = worm->rope_length1_unknownC8;
	this->ropeLength2 = worm->rope_length2_unknownD0;
	this->ropeX = worm->rope_anchorX_dwordC0;
	this->ropeY = worm->rope_anchorY_dwordC4;

}

bool WormState::apply(CTaskWorm *worm) {
	if(worm->wormnumber_dword100 == this->worm && worm->teamnumber_dwordFC == this->team) {
		CTaskTeam * team = (CTaskTeam*)worm->parent;
		DWORD ddmain = W2App::getAddrDdMain();
		char mymachine = *(char *) (ddmain + 0xD9DC + 0x40);
		if(team->owner_byte40 != mymachine) {
			worm->posY = this->posY;
			worm->posX = this->posX;
			worm->speedY = this->speedY;
			worm->speedX = this->speedX;
//			worm->shooting_angle_dword270 = this->shootingAngle;
			worm->facing_direction_dword1A8 = this->facingDirection;

			worm->state_dword44 = this->state;
			worm->state_counter_dword164 = this->stateCounter;

			worm->rope_next_angle_unknown264 = this->nextRopeAngle;
			if(!worm->attached_to_rope_unknownBC && !worm->is_shooting_rope_dword258 && this->shootingRope) {
				worm->callStartShootingNinjaRope(this->nextRopeAngle);
			}
			if(worm->attached_to_rope_unknownBC) {
				worm->rope_length1_unknownC8 = this->ropeLength1;
				worm->rope_length2_unknownD0 = this->ropeLength2;
				worm->rope_anchorX_dwordC0 = this->ropeX;
				worm->rope_anchorY_dwordC4 = this->ropeY;
			}

			return true;
		}
	}
	return false;
}

DWORD addrStartShootingNinjaRope;
int CTaskWorm::callStartShootingNinjaRope(DWORD a2) {
	int retv;
	_asm mov eax, this
	_asm push a2
	_asm call addrStartShootingNinjaRope
	_asm mov retv, eax
	return retv;
}

void CTaskWorm::install() {
	DWORD addrWormHandleMessage = _ScanPattern("CTaskWormHandleMessage", "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x00\x00\x00\x00\x53\x8B\xD9\x8B\x83\x00\x00\x00\x00\x8B\x8B\x00\x00\x00\x00\x8B\x93\x00\x00\x00\x00\x89\x44\x24\x14\x8B\x83\x00\x00\x00\x00\x56\x8B\x75\x0C\x89\x44\x24\x08\x89\x4C\x24\x14\x8B\x8B\x00\x00\x00\x00\x8D\x46\xE2\x83\xF8\x15\x57", "??????xx????xxxxx????xx????xx????xxxxxx????xxxxxxxxxxxxxx????xxxxxxx");
	addrStartShootingNinjaRope = _ScanPattern("StartShootingNinjaRope", "\x57\x8B\xF8\x8B\x4F\x2C\x8B\x81\x00\x00\x00\x00\x05\x00\x00\x00\x00\x83\xB9\x00\x00\x00\x00\x00\x75\x13\x83\xB9\x00\x00\x00\x00\x00\x75\x0A\xC7\x81\x00\x00\x00\x00\x00\x00\x00\x00\x83\xBF\x00\x00\x00\x00\x00\x75\x17", "??????xx????x????xx?????xxxx?????xxxx????????xx?????xx");
	_HookDefault(WormHandleMessage);
}

bool CTaskWorm::isOwnedByMe() {
	CTaskTeam * team = (CTaskTeam*)this->parent;
	return team->isOwnedByMe();
}