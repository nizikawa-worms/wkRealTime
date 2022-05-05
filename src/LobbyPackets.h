
#ifndef WKREALTIME_LOBBYPACKETS_H
#define WKREALTIME_LOBBYPACKETS_H

#include <string>
#include <vector>
#include <set>

typedef unsigned long       DWORD;
class LobbyPackets {
public:
	static const unsigned short int gameStartPacketID = 0x1c;
private:
	static inline DWORD addrClientSlot;
	static inline DWORD addrHostSlot;
	static inline DWORD addrHostUnicast;
	static inline DWORD addrSendMapData;
	static inline bool internalFlag = false;

	static int __fastcall hookHostLobbyPacketHandler(DWORD This, DWORD EDX, int slot, unsigned char * packet, size_t size);
	static void __fastcall hookHostEndscreenPacketHandler(DWORD This, DWORD EDX, int slot, unsigned char * packet, size_t size);
	static int __fastcall hookClientLobbyPacketHandler(DWORD This, DWORD EDX, unsigned char * packet, size_t size);
	static int __fastcall hookClientEndscreenPacketHandler(DWORD This, DWORD EDX, unsigned char * packet, size_t size);
	static int __fastcall hookInternalSendPacket(DWORD This, DWORD EDX, unsigned char * packet, size_t size);

	static inline DWORD addrHostIncomingConnection;
public:
	static void sendDataToClient_connection(DWORD connection, std::string msg);
	static int sendDataToClient_slot(DWORD slot, std::string msg);
	static void sendDataToHost(std::string msg);
	static int callHostBroadcastData(unsigned char * packet, size_t size);

	static void install();

	static bool isHost();
	static bool isClient();
	static std::string getNicknameBySlot(int slot);

	static void sendMessage(DWORD connection, const std::string & message);
	static int getNumSlots();
};


#endif //WKREALTIME_LOBBYPACKETS_H
