#include "LobbyPackets.h"
#include <utility>
#include "Hooks.h"
#include "LobbyProtocol.h"
#include "Frontend.h"
#include "Config.h"
#include "Utils.h"
#include "LobbyChat.h"
#include "Debugf.h"

int (__fastcall *origHostLobbyPacketHandler)(DWORD This, DWORD EDX, int slot, unsigned char * packet, size_t size);
int __fastcall LobbyPackets::hookHostLobbyPacketHandler(DWORD This, DWORD EDX, int slot, unsigned char * packet, size_t size) {
	if(Config::isHexDumpPacketsEnabled()) {
		Utils::hexDump("HostLobbyPacketHandler", packet, size);
	}
	auto ptype = *(unsigned short int*) packet;
	if(ptype == LobbyProtocol::magicPacketID) {
		LobbyProtocol::parseMsgHost(This, std::string((char*)packet, size), slot);
	}
	return origHostLobbyPacketHandler(This, 0, slot, packet, size);
}

void (__fastcall *origHostEndscreenPacketHandler)(DWORD This, DWORD EDX, int slot, unsigned char * packet, size_t size);
void __fastcall LobbyPackets::hookHostEndscreenPacketHandler(DWORD This, DWORD EDX, int slot, unsigned char * packet, size_t size) {
	if(Config::isHexDumpPacketsEnabled()) {
		Utils::hexDump("HostEndscreenPacketHandler", packet, size);
	}
	auto ptype = *(unsigned short int*) packet;
	if(ptype == LobbyProtocol::magicPacketID) {
		LobbyProtocol::parseMsgHost(This, std::string((char*)packet, size), slot);
	}
	origHostEndscreenPacketHandler(This, 0, slot, packet, size);
}

int (__fastcall *origClientLobbyPacketHandler)(DWORD This, DWORD EDX, unsigned char * packet, size_t size) ;
int __fastcall LobbyPackets::hookClientLobbyPacketHandler(DWORD This, DWORD EDX, unsigned char * packet, size_t size) {
	if(Config::isHexDumpPacketsEnabled()) {
		Utils::hexDump("ClientLobbyPacketHandler", packet, size);
	}
	auto ptype = *(unsigned short int*) packet;
	if(ptype == LobbyProtocol::magicPacketID) {
		LobbyProtocol::parseMsgClient(std::string((char*)packet, size), This);
	}
	return origClientLobbyPacketHandler(This, 0, packet, size);
}

int (__fastcall *origClientEndscreenPacketHandler)(DWORD This, DWORD EDX, unsigned char * packet, size_t size);
int __fastcall LobbyPackets::hookClientEndscreenPacketHandler(DWORD This, DWORD EDX, unsigned char * packet, size_t size) {
	if(Config::isHexDumpPacketsEnabled()) {
		Utils::hexDump("ClientEndscreenPacketHandler", packet, size);
	}
	auto ptype = *(unsigned short int*) packet;
	if(ptype == LobbyProtocol::magicPacketID) {
		LobbyProtocol::parseMsgClient(std::string((char*)packet, size), This);
	}
	return origClientEndscreenPacketHandler(This, 0, packet, size);
}

int (__fastcall *origInternalSendPacket)(DWORD This, DWORD EDX, unsigned char * packet, size_t size);
int __fastcall LobbyPackets::hookInternalSendPacket(DWORD This, DWORD EDX, unsigned char * packet, size_t size) {
	if(Config::isHexDumpPacketsEnabled()) {
		Utils::hexDump("InternalSendPacket", packet, size);
	}
	if(internalFlag) {
		auto ptype = *(unsigned short int*) packet;
		if(ptype == gameStartPacketID){
			LobbyProtocol::sendRealTimeStatusToAllPlayers();
		}
	}
	auto ret = origInternalSendPacket(This, 0, packet, size);
	return ret;
}


void LobbyPackets::sendDataToClient_connection(DWORD connection, std::string msg) {
	auto data = LobbyProtocol::encodeMsg(std::move(msg));
	origInternalSendPacket(connection, 0, (unsigned char*)data.c_str(), data.size());
}


void LobbyPackets::sendMessage(DWORD connection, const std::string &message) {
	std::string data = {0x00, 0x00};
	data += message;
	data += {0x00};
	origInternalSendPacket(connection, 0, (unsigned char *) data.c_str(), data.size());
}

int LobbyPackets::sendDataToClient_slot(DWORD slot, std::string msg) {
	auto data = LobbyProtocol::encodeMsg(std::move(msg));

	size_t sized = data.size();
	auto dataptr = data.c_str();
	int reta;

	_asm push addrHostSlot
	_asm mov esi, dataptr
	_asm mov ecx, slot
	_asm mov edx, sized
	_asm call addrHostUnicast
	_asm mov reta, eax

	return 0;
}

void LobbyPackets::sendDataToHost(std::string msg) {
	auto data = LobbyProtocol::encodeMsg(std::move(msg));
	(*(int (__thiscall **)(int *, unsigned char *, size_t)) (*(int *) addrClientSlot + 52))((int*)addrClientSlot,(unsigned char*) data.c_str(), data.size());
}


int (__stdcall * origHostBroadcastData)(unsigned char *data_a2, int size_a3); // eax = host slot
int LobbyPackets::callHostBroadcastData(unsigned char * packet, size_t psize) {
	if(!addrHostSlot || !isHost()) {
		return 0;
	}
	int retv;
	_asm mov eax, addrHostSlot
	_asm push psize
	_asm push packet
	_asm call origHostBroadcastData
	_asm mov retv, eax

	return retv;
}

DWORD (__stdcall *origSendGameStart)(CWnd *a1);
DWORD __stdcall hookSendGameStart(CWnd *a1) {
	LobbyProtocol::sendRealTimeStatusToAllPlayers();
	return origSendGameStart(a1);
}


void LobbyPackets::install() {
	DWORD addrHostBroadcast = _ScanPattern("HostBroadcast", "\x83\x78\x0C\x00\x55\x8B\x6C\x24\x0C\x74\x50\x53\x56\xBB\x00\x00\x00\x00\x57\x8D\xB0\x00\x00\x00\x00\x8D\x7B\x05\x8D\x64\x24\x00", "??????xxxxxxxx????x?????????xxxx"); //0x58F8E0
	addrHostUnicast = _ScanPattern("HostUnicast", "\x8B\x44\x24\x04\x83\x78\x0C\x00\x74\x28\x69\xC9\x00\x00\x00\x00\x03\xC1\x83\xB8\x00\x00\x00\x00\x00\x75\x2F\x8D\x88\x00\x00\x00\x00\x8B\x01\x52", "??????xxxx??????xxxx?????xxxx????xxx"); //0x58F9F0
	DWORD addrHostLobbyPacketHandler = _ScanPattern("HostLobbyPacketHandler", "\x83\xEC\x10\x8B\x44\x24\x1C\x83\xF8\x02\x53\x8B\xD9\x89\x5C\x24\x04\x0F\x82\x00\x00\x00\x00\x55\x8B\x6C\x24\x20\x0F\xB7\x4D\x00\x85\xC9\x56\x57\x0F\x84\x00\x00\x00\x00\x83\xF9\x10\x0F\x84\x00\x00\x00\x00\x83\xF9\x1A\x74\x18", "???????xxxxxxxxxxxx????xxxxxxxxxxxxxxx????xxxxx????xxxxx"); //0x4B6290
	DWORD addrHostEndscreenPacketHandler = _ScanPattern("HostEndscreenPacketHandler", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x20\x53\x55\x56\x8B\x74\x24\x40\x0F\xB7\x06\x57\x33\xDB\x3B\xC3\x8B\xF9\x89\x7C\x24\x2C\x0F\x84\x00\x00\x00\x00\x83\xF8\x0F\x0F\x85\x00\x00\x00\x00", "???????xx????xxxx????xxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxx????"); //0x4AC0D0
	DWORD addrClientLobbyPacketHandler = _ScanPattern("ClientLobbyPacketHandler", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x24\x53\x8B\x5D\x0C\x83\xFB\x02\x56\x8B\xF1\x57\x89\x74\x24\x10\x0F\x82\x00\x00\x00\x00\x8B\x7D\x08\x0F\xB7\x0F\x0F\xB7\xC1\x83\xF8\x32\x0F\x87\x00\x00\x00\x00\x0F\xB6\x90\x00\x00\x00\x00\xFF\x24\x95\x00\x00\x00\x00\x53", "??????xxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxx????xxx????xxx????x"); //0x4C0790
	DWORD addrClientEndscreenPacketHandler = _ScanPattern("ClientEndscreenPacketHandler", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x08\x56\x8B\x75\x08\x0F\xB7\x06\x83\xC0\xE7\x83\xF8\x10\x57\x8B\xF9\x0F\x87\x00\x00\x00\x00\x0F\xB6\x80\x00\x00\x00\x00\xFF\x24\x85\x00\x00\x00\x00\x83\x7D\x0C\x08\x0F\x82\x00\x00\x00\x00\x8B\x4E\x04\x83\xE9\x01\x83\xF9\x0C\x0F\x83\x00\x00\x00\x00", "??????xxxxxxxxxxxxxxxxxxxxx????xxx????xxx????xxxxxx????xxxxxxxxxxx????"); //0x4BD400
	DWORD addrInternalSendPacket = _ScanPattern("InternalSendPacket", "\x53\x8B\x5C\x24\x0C\x55\x56\x57\x8D\x6B\x04\x55\x8B\xF9\xE8\x00\x00\x00\x00\x8B\x54\x24\x18\x8B\xF0\x8D\x43\x04\x66\x89\x46\x02\x53\xC6\x46\x01\x00\x8A\x8F\x00\x00\x00\x00\x52\x8D\x46\x04", "??????xxxxxxxxx????xxxxxxxxxxxxxxxxxxxx????xxxx"); //58FCA0
//	DWORD addrSendColorMapData = _ScanPattern("SendColorMapData", "\x55\x8B\x6C\x24\x0C\x69\xED\x00\x00\x00\x00\x57\x8B\xBD\x00\x00\x00\x00\x85\xFF\x0F\x8C\x00\x00\x00\x00\x8B\x44\x24\x0C\x53\x8B\x98\x00\x00\x00\x00\x2B\xDF\x81\xFB\x00\x00\x00\x00\x56\x7E\x05", "??????x????xxx????xxxx????xxxxxxx????xxxx????xxx");
	origHostBroadcastData = (int (__stdcall *)(unsigned char *,int)) addrHostBroadcast;
	DWORD addrSendGameStart = _ScanPattern("SendGameStart", "\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x8B\x50\x0C", "??????xxx????xx????xxxx????xx????x????x????xxx");
	addrClientSlot = *(DWORD*)(addrClientEndscreenPacketHandler + 0x103);
	addrHostSlot = *(DWORD*)(addrHostEndscreenPacketHandler + 0xC6);
	debugf("addrClientSlot:0x%X addrHostSlot:0x%X\n", addrClientSlot, addrHostSlot);

	DWORD addrHostSlotListConstructor = _ScanPattern("HostSlotListConstructor", "\x64\xA1\x00\x00\x00\x00\x6A\xFF\x68\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x56\x8B\x74\x24\x14\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x6A\x06\x89\x46\x04\x89\x46\x08\x68\x00\x00\x00\x00\x8D\x86\x00\x00\x00\x00", "??????xxx????xxxx????xxxxxx????x????x????xxxxxxxxx????xx????");
	DWORD * hostVtable = *(DWORD**)(addrHostSlotListConstructor + 0x5D);
	addrHostIncomingConnection = hostVtable[9];

	_HookDefault(HostLobbyPacketHandler);
	_HookDefault(HostEndscreenPacketHandler);
	_HookDefault(ClientLobbyPacketHandler);
	_HookDefault(ClientEndscreenPacketHandler);
	_HookDefault(InternalSendPacket);
	_HookDefault(SendGameStart);
}

bool LobbyPackets::isHost() {
	return *(int *)(addrHostSlot + 12) != 0;
}

bool LobbyPackets::isClient() {
	return *(int *)(addrClientSlot + 0xB8) == 3;
}

std::string LobbyPackets::getNicknameBySlot(int slot) {
	int numSlots = getNumSlots();
	if(!isHost() || slot < 0 || slot > numSlots) return "???";
	DWORD hostSlots = (numSlots == 6) ? addrHostSlot : *(DWORD*)(addrHostIncomingConnection + 0xB);
	DWORD slotAddr = hostSlots + 0x1A4 + slot * 0x19118;
	return (const char*)(slotAddr + 0xD0);
}

int LobbyPackets::getNumSlots() {
	if(addrHostIncomingConnection) {
		return *(char*)(addrHostIncomingConnection + 0x28);
	}
	return 6;
}