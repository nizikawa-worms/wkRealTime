
#ifndef WKREALTIME_LOBBYPROTOCOL_H
#define WKREALTIME_LOBBYPROTOCOL_H


#include <string>
#include <json.hpp>
#include <set>
#include <filesystem>

typedef unsigned long       DWORD;
class LobbyProtocol {
public:
	struct ProtocolHeader {
		unsigned short int pktId;
	};
	static const unsigned short int magicPacketID = 0x6666;

	static std::string encodeMsg(std::string data);
	static std::string decodeMsg(std::string data);

	static void parseMsgHost(DWORD hostThis, std::string data, int slot);
	static void parseMsgClient(std::string data, DWORD connection);

	static void install();

	static void showVersionInfoConnection(nlohmann::json & parsed, DWORD connection);
	static void sendVersionInfoConnection(nlohmann::json & parsed, DWORD connection);
	static void showVersionInfoSlot(nlohmann::json & parsed, int slot);
	static void sendVersionInfoSlot(nlohmann::json & parsed, int slot);
	static void sendVersionQueryToAllPlayers();
	static void sendVersionQueryToHost();

	static void sendRealTimeStatusToAllPlayers();
};


#endif //WKREALTIME_LOBBYPROTOCOL_H
