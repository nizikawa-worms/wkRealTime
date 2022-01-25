
#include "GameNet.h"
#include "Hooks.h"
#include "Constants.h"
#include "entities/CTaskWorm.h"
#include "entities/CTaskTurnGame.h"
#include "W2App.h"
#include "RealTime.h"
#include "TaskMessageFifo.h"
#include "entities/CTaskTeam.h"
#include "Debugf.h"
#include "Utils.h"

// sending realtime state and inputs from teams not owning the turn
void GameNet::hookGameNetUnknown_c(DWORD ddgame) {
	if(RealTime::isActive() && RealTime::isTurn()) {
		DWORD gameglobal = W2App::getAddrGameGlobal();
		DWORD * controlFlag = (DWORD*)(gameglobal + 0x7264);
		if(*controlFlag != 0) {
			CTaskTurnGame * turngame = *(CTaskTurnGame**)(ddgame + 0x8);
			TaskMessageFifo * inputFifo = *(TaskMessageFifo**)(ddgame + 0x40);
			TaskMessageFifo * chatFifo = *(TaskMessageFifo**)(ddgame + 0x28);
			TaskMessageFifo * networkFifo = *(TaskMessageFifo**)(ddgame + 0x3C);
			TaskMessageFifo * controlFifo = *(TaskMessageFifo**)(ddgame + 0x50);

			sendWormStates(inputFifo, turngame);

			turngame->vtable8_HandleMessage(turngame, Constants::TaskMessage::TaskMessage_ProcessInput, 1032, &inputFifo);
			if(inputFifo->num_elements_dword18 > 0) {
				TaskMessageFifo::callCopyFiFo(inputFifo, networkFifo);
				TaskMessageFifo::callTaskMessageSend(networkFifo, 0, Constants::TaskMessage::TaskMessage_FrameFinish, 0);
				DWORD gamenet = W2App::getAddrWsGameNet();
				(*(void (__thiscall **)(DWORD, TaskMessageFifo *))(*(DWORD*)gamenet + 8))(gamenet, networkFifo); // serialize events to network
				callSendOutOfOrder(gamenet);
			}
		}
	}
}

void __stdcall GameNet::gamenetmain_patch2_c() {
//	DWORD gamenet = W2App::getAddrWsGameNet();
//	callSendOutOfOrder(gamenet);
}

//sending state from team owning the turn
void __stdcall GameNet::gamenetmain_patch1_c(DWORD ddgame) {
	if (RealTime::isActive() && RealTime::isTurn()) {
		DWORD gamenet = W2App::getAddrWsGameNet();
		TaskMessageFifo * inputFifo = *(TaskMessageFifo**)(ddgame + 0x40);
		TaskMessageFifo * chatFifo = *(TaskMessageFifo**)(ddgame + 0x28);
		TaskMessageFifo * networkFifo = *(TaskMessageFifo**)(ddgame + 0x3C);
		TaskMessageFifo * controlFifo = *(TaskMessageFifo**)(ddgame + 0x50);

//		GameState gamestate;
//		TaskMessageFifo::callTaskMessageSend(networkFifo, 0, Constants::TaskMessage::TaskMessage_GameState, &gamestate);
		CTaskTurnGame * turngame = *(CTaskTurnGame**)(ddgame + 0x8);
		sendWormStates(inputFifo, turngame);
		TaskMessageFifo::callCopyFiFo(inputFifo, networkFifo);
		(*(void (__thiscall **)(DWORD, TaskMessageFifo *))(*(DWORD*)gamenet + 8))(gamenet, networkFifo); // serialize events to network
		callSendOutOfOrder(gamenet);
	}
}

void GameNet::sendWormStates(TaskMessageFifo *fifo, CTaskTurnGame *turngame) {
	turngame->traverse([&](CTask * obj, const int level) {
		if(obj->classtype == Constants::ClassType_Task_Team) {
			CTaskTeam * team = (CTaskTeam*)obj;
			if(team->isOwnedByMe()) {
				for(auto child : team->children) {
					CTaskWorm * worm = (CTaskWorm*)child;
					if(worm->active_dword104) {
						WormState state(worm);
						TaskMessageFifo::callTaskMessageSend(fifo, sizeof(WormState), Constants::TaskMessage_WormState, &state);
					}
				}
			}
		}
	});
}

int (__stdcall *origGameNetUnknown)();
int __stdcall GameNet::hookGameNetUnknown() {
	DWORD ddgame, retv;
	_asm mov ddgame, eax

	_asm mov eax, ddgame
	_asm call origGameNetUnknown
	_asm mov retv, eax

	hookGameNetUnknown_c(ddgame);
	injectRealtimeFifoEvents();

	return retv;
}


void __stdcall GameNet::injectRealtimeFifoEvents() {
	if(!RealTime::isActive() || !RealTime::isTurn()) return;

	DWORD ddgame = W2App::getAddrDdGame();
	DWORD gameglobal = W2App::getAddrGameGlobal();
	DWORD gamenet = *(DWORD*)(gameglobal + 0x1c);
	DWORD ddmain = W2App::getAddrDdMain();

	DWORD numMachines = *(DWORD*)(gamenet + 0x8);
	DWORD currentMachine = *(DWORD*)(gameglobal + 0x726C);
	char myMachine = *(char *) (ddmain + 0xD9DC + 0x40);
	TaskMessageFifo * inputFifo = *(TaskMessageFifo**)(ddgame + 0x40);

	for(int i=0; i < numMachines; i++) {
		if(i == currentMachine) continue;
		if(i == myMachine) continue;

		TaskMessageFifo * fifo = *(TaskMessageFifo **)(gamenet+0x24 + 0x68 * i);
		if(fifo->data_start_dword14) {
			for(int n=0; n < fifo->num_elements_dword18; n++) {
				size_t msize;
				int mtype;
				unsigned char data[2048];
				callFifoGamenetGetEvent(i, &msize, gamenet, &mtype, (unsigned char*)&data);

				if(mtype == Constants::TaskMessage::TaskMessage_FrameFinish) {
					break;
				}
				CTaskTurnGame * turngame = *(CTaskTurnGame**)(ddgame + 0x8);
				turngame->vtable8_HandleMessage(turngame, (Constants::TaskMessage)mtype, sizeof(data), data);
			}
		}
	}
}

int (__fastcall *origTaskMessageSerializer)(unsigned char *out_a1, unsigned char *in_a2, int a3, Constants::TaskMessage mtype_a4, int game_version_a5,int a6);
int __fastcall hookTaskMessageSerializer(unsigned char *out_a1, unsigned char *in_a2, int available_a3, Constants::TaskMessage mtype_a4, int game_version_a5,int a6) {
//	debugf("Serializing event: %d - a3: %d a6: %d\n", mtype_a4, a3, a6);

	if(mtype_a4 == Constants::TaskMessage_WormState) {
		if(available_a3 < 1 + sizeof(WormState)) return -1;
		out_a1[0] = mtype_a4;
		memcpy(&out_a1[1], in_a2, sizeof(WormState));
		return 1 + sizeof(WormState);
	} else if(mtype_a4 == Constants::TaskMessage_GameState) {
		if(available_a3 < 1 + sizeof(GameState)) return -1;
		out_a1[0] = mtype_a4;
		memcpy(&out_a1[1], in_a2, sizeof(GameState));
		return 1 + sizeof(GameState);
	} else {
		auto ret = origTaskMessageSerializer(out_a1, in_a2, available_a3, mtype_a4, game_version_a5, a6);
		if(ret < 0) {
			debugf("Failed to serialize event: %d - a3: %d a6: %d\n", mtype_a4, available_a3, a6);
		}
		return ret;
	}
}

DWORD origTaskMessageDeserializer;
int __stdcall hookTaskMessageDeserializer(unsigned int available_a4, DWORD *out_size_a5, int a6, int a7, int a8) {
	DWORD * out_mtype;
	unsigned char * m_in, *m_out;
	int retv;

	_asm mov out_mtype, eax
	_asm mov m_in, ecx
	_asm mov m_out, esi

//	debugf("Deserializing event: %d a4: %d a6: %d a7: %d a8: %d\n", *m_in, a4, a6, a7, a8);
	if(*m_in == Constants::TaskMessage_WormState) {
		if(available_a4 < 1 + sizeof(WormState)) return -1;
		*out_mtype = Constants::TaskMessage::TaskMessage_WormState;
		memcpy(m_out, &m_in[1], sizeof(WormState));
		*out_size_a5 = sizeof(WormState);
		return 1 + sizeof(WormState);
	} else if(*m_in == Constants::TaskMessage_GameState) {
		if(available_a4 < 1 + sizeof(GameState)) return -1;
		*out_mtype = Constants::TaskMessage::TaskMessage_GameState;
		memcpy(m_out, &m_in[1], sizeof(GameState));
		*out_size_a5 = sizeof(GameState);
		return 1 + sizeof(GameState);
	} else {
		_asm mov eax, out_mtype
		_asm mov ecx, m_in
		_asm mov esi, m_out
		_asm push a8
		_asm push a7
		_asm push a6
		_asm push out_size_a5
		_asm push available_a4
		_asm call origTaskMessageDeserializer
		_asm mov retv, eax

		if(retv < 0) {
			debugf("Failed to deserialize event: %d - available: %d\n", *m_in, available_a4);
		}

		return retv;
	}
}

DWORD addrFifoGamenetGetEvent;
int GameNet::callFifoGamenetGetEvent(int machine, size_t * msize, DWORD gamenet, int * mtype, unsigned char *data) {
	int retv;
	_asm mov eax, machine
	_asm mov edx, msize
	_asm push data
	_asm push mtype
	_asm push gamenet
	_asm call addrFifoGamenetGetEvent
	_asm mov retv, eax
	return retv;
}

DWORD addrSendOutOfOrder;
int GameNet::callSendOutOfOrder(DWORD gamenet) {
	int retv;
	_asm mov eax, gamenet
	_asm call addrSendOutOfOrder
	_asm mov retv, eax

	return retv;
}


int (__stdcall *origWriteStateChecksum)(DWORD *a3);
int __stdcall hookWriteStateChecksum(DWORD *a3) {
	int a1, a2, retv;
	_asm mov a1, edx
	_asm mov a2, edi

	if(RealTime::isActive()) return 1;
	_asm mov edx, a1
	_asm mov edi, a2
	_asm push a3
	_asm call origWriteStateChecksum
	_asm mov retv, eax

	return retv;
}

int (__stdcall *origTaskMessageIgnoredChecksum)();
int __stdcall hookTaskMessageIgnoredChecksum() {
	int tm, retv;
	_asm mov tm, eax

	if(RealTime::isActive()) return 1;

	_asm mov eax, tm
	_asm call origTaskMessageIgnoredChecksum
	_asm mov retv, eax

	return retv;
}



DWORD gamenetmain_patch1_ret1;
DWORD gamenetmain_patch1_ret2;
void __declspec(naked) gamenetmain_patch1() {
	_asm cmp     [ebx+0x7264], esi
	_asm jnz notmyturn 	 // do nothing - not owning the turn
_asm myturn:
	_asm pushad
	_asm push edi
	_asm call GameNet::gamenetmain_patch1_c
	_asm popad
	_asm jmp gamenetmain_patch1_ret1
_asm notmyturn:
	_asm jmp gamenetmain_patch1_ret2
}

DWORD gamenetmain_patch2_ret;
void __declspec(naked) gamenetmain_patch2() {
	_asm call edx

	_asm pushad
	_asm call GameNet::gamenetmain_patch2_c
	_asm popad
	_asm jmp gamenetmain_patch2_ret
}


void GameNet::install() {
	DWORD addrGamenetMain = _ScanPattern("GamenetMain", "\x8B\x87\x00\x00\x00\x00\x53\x55\x8B\x68\x1C\x8B\x5D\x08\x56\x8B\xF7\xE8\x00\x00\x00\x00\x57\xE8\x00\x00\x00\x00\x33\xF6\x3B\xDE\x7E\x3E\x8D\x87\x00\x00\x00\x00\x39\x70\xCC\x75\x2B\x8B\x8F\x00\x00\x00\x00\x8B\x51\x24\x0F\xB7\x8A\x00\x00\x00\x00\x6B\xC9\x32\x89\x08", "??????xxxxxxxxxxxx????xx????xxxxxxxx????xxxxxxx????xxxxxx????xxxxx");
	DWORD addrGameNetUnknown = _ScanPattern("GameNetUnknown", "\x51\x8B\x88\x00\x00\x00\x00\x53\x8B\x59\x1C\x33\xC9\x39\x4B\x0C\x55\x57\x89\x4C\x24\x0C\x75\x05\x8B\x6B\x08\xEB\x05\xBD\x00\x00\x00\x00", "???????xxxxxxxxxxxxxxxxxxxxxxx????");
	DWORD addrWriteStateChecksum = _ScanPattern("WriteStateChecksum", "\x81\xEC\x00\x00\x00\x00\x83\xBF\x00\x00\x00\x00\x00\x55\x8B\xAC\x24\x00\x00\x00\x00\x56\x74\x50\x80\xBF\x00\x00\x00\x00\x00\x74\x47\x8B\x87\x00\x00\x00\x00\x8B\x8F\x00\x00\x00\x00\x8B\xF2\x83\xC6\x03", "??????xx?????xxxx????xxxxx?????xxxx????xx????xxxxx");
	DWORD addrTaskMessageIgnoredChecksum = _ScanPattern("TaskMessageIgnoredChecksum", "\x83\xC0\xFA\x83\xF8\x71\x77\x14\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\xB8\x00\x00\x00\x00\xC3\x33\xC0\xC3\x90\x06", "??????xxxxx????xxx????x????xxxxxx");
	addrSendOutOfOrder = _ScanPattern("SendOutOfOrder", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x1C\x53\x56\x8B\xF0\x8B\x46\x20\x83\x78\x18\x00\x57\x0F\x8E\x00\x00\x00\x00\x8B\x5C\x24\x0C\xE8\x00\x00\x00\x00\x83\xF8\x4B\x0F\x8D\x00\x00\x00\x00\x8B\x46\x20\x33\xC9", "??????xxxxxxxxxxxxxxxxx????xxxxx????xxxxx????xxxxx");
	addrFifoGamenetGetEvent = _ScanPattern("FifoGamenetGetEvent", "\x8B\x4C\x24\x04\x83\x79\x10\x00\x74\x64\x85\xC0\x7C\x60\x3B\x41\x08\x7D\x5B\x6B\xC0\x68\x56\x57\x8B\x7C\x08\x24\x83\x7F\x14\x00\x74\x45\x8B\x47\x14\x8B\x08\x8D\x70\x08\x85\xF6\x89\x0A\x74\x37", "??????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	DWORD addrTaskMessageSerializer = _ScanPattern("TaskMessageSerializer", "\x8B\x44\x24\x08\x83\xF8\x74\x53\x8B\x5C\x24\x14\x55\x56\x8B\x74\x24\x10\x57\x8B\x7C\x24\x1C\x88\x01\x0F\x87\x00\x00\x00\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\x83\xFB\xFF\x0F\x8D\x00\x00\x00\x00\x5F\x5E", "??????xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxxxx????xx");
	DWORD addrTaskMessageDeserializer = _ScanPattern("TaskMessageDeserializer", "\x55\x8B\xEC\x83\xE4\xF8\x8B\x55\x18\x83\xEC\x08\x53\x8B\x5D\x08\x85\xDB\x57\x8B\xF8\x77\x0B\x83\xC8\xFF\x5F\x5B\x8B\xE5\x5D\xC2\x14\x00\x0F\xB6\x01", "??????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");


//	DWORD addrChatKeyHandling = _ScanPattern("ChatKeyHandling", "\x81\xEC\x00\x00\x00\x00\x53\x55\x8B\xAC\x24\x00\x00\x00\x00\x8B\x85\x00\x00\x00\x00\x8B\x08\x8B\x51\x04\x8B\x02\x8B\x11\x56\x89\x44\x24\x28\x8B\x42\x14\x33\xF6\x57\x89\x74\x24\x14\x89\x4C\x24\x20\xFF\xD0\x8A\xD8", "??????xxxxx????xx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
//	DWORD addrHandleRealtimeControlMessages = _ScanPattern("HandleRealtimeControlMessages", "\x81\xEC\x00\x00\x00\x00\x53\x55\x8B\xAC\x24\x00\x00\x00\x00\x8B\x85\x00\x00\x00\x00\x8B\x40\x1C\x89\x44\x24\x10\x8B\x40\x08\x85\xC0\x56\x57\x89\x44\x24\x1C\xC7\x44\x24\x00\x00\x00\x00\x00\x0F\x8E\x00\x00\x00\x00\x8B\x54\x24\x14", "??????xxxxx????xx????xxxxxxxxxxxxxxxxxxxxx?????xx????xxxx");


	_HookDefault(WriteStateChecksum);
	_HookDefault(TaskMessageIgnoredChecksum);
	_HookDefault(GameNetUnknown);
	_HookDefault(TaskMessageSerializer);
	_HookDefault(TaskMessageDeserializer);

	gamenetmain_patch1_ret1 = addrGamenetMain + 0x13A; //52DDFA
	gamenetmain_patch1_ret2 = addrGamenetMain + 0x171; //52DE31
	Hooks::hookAsm(addrGamenetMain + 0x132, (DWORD)&gamenetmain_patch1); //0052DDF2

	gamenetmain_patch2_ret = addrGamenetMain + 0x221; //52DEE1
	Hooks::hookAsm(addrGamenetMain + 0x16A, (DWORD)&gamenetmain_patch2);  //52DE2C
}
