

#include "CTaskTurnGame.h"
#include "../Debugf.h"
#include "../Hooks.h"
#include "../RealTime.h"
#include "CTaskTeam.h"
#include "../TaskMessageFifo.h"
#include "../W2App.h"
#include "CTaskWorm.h"

int (__fastcall *origTurnHandleMessage)(CTaskTurnGame * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data);
int __fastcall CTaskTurnGame::hookTurnHandleMessage(CTaskTurnGame * This, int EDX, CTask * sender, Constants::TaskMessage mtype, size_t size, void * data) {
	auto ret = origTurnHandleMessage(This, 0, sender, mtype, size, data);

	switch(mtype) {
		case Constants::TaskMessage::TaskMessage_StartTurn: {
			debugf("Start turn\n");
			RealTime::setTurn(true);
			if(RealTime::isActive()) {

				This->traverse([&](CTask *obj, const int level) {
					if (obj->classtype == Constants::ClassType_Task_Team) {
						CTaskTeam *team = (CTaskTeam *) obj;
						flagActivatingTeam = true;
						char buff[1024];
						memset(buff, 0, sizeof(buff));
						*(DWORD*)buff = team->team_number_dword38;

						int first_worm_number = 0;
						bool found = false;
						for(auto child : team->children) {
							CTaskWorm * worm = (CTaskWorm*)child;
							if(!first_worm_number) first_worm_number = worm->wormnumber_dword100;
							if(team->current_worm_number_dword48 == worm->wormnumber_dword100) {
								found = true;
								break;
							}
						}
						if(!found) {
							if(first_worm_number) {
								debugf("*bugfix* Setting current worm number of team %d to %d\n", team->team_number_dword38, first_worm_number);
								team->current_worm_number_dword48 = first_worm_number;
								found = true;
							} else {
								debugf("team %d has no matching current worm\n", team->team_number_dword38);
							}
						}
						if(found) {
							team->vtable8_HandleMessage(This, Constants::TaskMessage::TaskMessage_StartTurn, sizeof(buff), buff);
						}
						flagActivatingTeam = false;
					}
				});

			}
			break;
		}
		case Constants::TaskMessage::TaskMessage_FinishTurn: {
			debugf("Finish turn\n");
			RealTime::setTurn(false);
			break;
		}
		case Constants::TaskMessage::TaskMessage_GameState: {
			GameState * state = (GameState*)data;
			state->apply(This);
			break;
		}
		default: break;
	}

	return ret;
}

DWORD (__stdcall *origSetActiveWorm)();
DWORD __stdcall CTaskTurnGame::hookSetActiveWorm() {
	DWORD a1, teamnumber, wormnumber, retv;
	_asm mov a1, eax
	_asm mov teamnumber, edx
	_asm mov wormnumber, esi

	if(!flagActivatingTeam) {
		_asm mov eax, a1
		_asm mov edx, teamnumber
		_asm mov esi, wormnumber
		_asm call origSetActiveWorm
		_asm mov retv, eax
	}

	return retv;
}


void CTaskTurnGame::install() {
	DWORD addrTurnHandleMessage = _ScanPattern("CTurnGameHandleMessage", "\x55\x8B\xEC\x83\xE4\xF8\x81\xEC\x00\x00\x00\x00\x53\x8B\x5D\x0C\x56\x8D\x43\xFE\x83\xF8\x7B\x57\x8B\xF1\x0F\x87\x00\x00\x00\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\x8B\x86\x00\x00\x00\x00", "??????xx????xxxxxxxxxxxxxxxx????xxx????xxx????xx????");
	DWORD addrSetActiveWorm = Hooks::scanPattern("SetActiveWorm", "\x85\xF6\x57\x75\x48\x69\xD2\x00\x00\x00\x00\x39\x74\x02\xF8\x8D\x0C\x02\x0F\x84\x00\x00\x00\x00\x83\xCA\xFF\x01\x90\x00\x00\x00\x00\x39\x71\xF4\x7C\x74\x01\x90\x00\x00\x00\x00", "??????x????xxxxxxxxx????xxxxx????xxxxxxx????");

	_HookDefault(TurnHandleMessage);
	_HookDefault(SetActiveWorm);


	//:00531F81 - game end condition check!
	//005578CE writes 6 to game end
}

GameState::GameState() {
	DWORD gameglobal = W2App::getAddrGameGlobal();
	DWORD collisionmanager = *(DWORD*)(gameglobal + 0x528);
	this->frame = *(DWORD*)(gameglobal + 0x5CC);
	this->wind = *(DWORD*)(collisionmanager + 0x230);
}

bool GameState::apply(CTaskTurnGame * turngame) {
	DWORD gameglobal = W2App::getAddrGameGlobal();
	DWORD collisionmanager = *(DWORD*)(gameglobal + 0x528);
//	*(DWORD*)(gameglobal + 0x5CC) = this->frame;
//	*(DWORD*)(collisionmanager + 0x230) = this->wind;
	return true;
}
