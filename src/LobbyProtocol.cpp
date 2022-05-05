
#include "LobbyProtocol.h"
#include <json.hpp>
#include "Config.h"
#include "LobbyPackets.h"
#include "Frontend.h"
#include "LobbyChat.h"
#include <iostream>
#include <filesystem>
#include <optional>
#include <fstream>
#include <sstream>
#include "Utils.h"
#include "Debugf.h"
#include "Hooks.h"
#include "RealTime.h"

namespace fs = std::filesystem;
typedef unsigned char byte;

void LobbyProtocol::install() {

}


std::string LobbyProtocol::encodeMsg(std::string data) {
	ProtocolHeader header{magicPacketID};
	return std::string((const char*)&header, sizeof(header)) + data;
}

std::string LobbyProtocol::decodeMsg(std::string data) {
	if(data.size() >= sizeof(ProtocolHeader)) {
		return data.substr(sizeof(ProtocolHeader));
	}
	throw std::runtime_error("Invalid packet size");
}

void LobbyProtocol::parseMsgHost(DWORD hostThis, std::string data, int slot) {
	try {
		data = decodeMsg(data);
		nlohmann::json parsed = nlohmann::json::parse(data);
		std::string mType = parsed["type"];
		std::string module = parsed["module"];
		if(module == Config::getModuleStr()) {
			if (mType == "Placeholder") {}
			else if(mType == "VersionQuery") { sendVersionInfoSlot(parsed, slot);}
			else if(mType == "VersionInfo") { showVersionInfoSlot(parsed, slot);}
			else throw std::runtime_error("Unknown protocol message type: " + mType + " - if you see this error you probably have an outdated version of " PROJECT_NAME);
		}
	} catch(std::exception & e) {
		char buff[1024];
		_snprintf_s(buff, _TRUNCATE, PROJECT_NAME " host exception in data from player %s:\n%s", LobbyPackets::getNicknameBySlot(slot).c_str(), e.what());
		debugf("parseMsgHost exception: %s\n", e.what());
		debugf("Received message: |%s|\n", data.c_str());
		Frontend::callMessageBox(buff, 0, 0);
	}
}

void LobbyProtocol::parseMsgClient(std::string data, DWORD connection) {
	try {
		data = decodeMsg(data);
		debugf("Message: %s\n", data.c_str());
		nlohmann::json parsed = nlohmann::json::parse(data);
		std::string mType = parsed["type"];
		std::string module = parsed["module"];
		if(module == Config::getModuleStr()) {
			if(mType == "RealtimeStatus") {
				RealTime::setActive(parsed["enabled"]);
				debugf("RealtimeStatus: %d\n", RealTime::isActive());
			}
			else if(mType == "VersionQuery") { sendVersionInfoConnection(parsed, connection);}
			else if(mType == "VersionInfo") { showVersionInfoConnection(parsed, connection);}
			else throw std::runtime_error("Unknown protocol message type: " + mType + " - if you see this error you probably have an outdated version of " PROJECT_NAME);
		}
	} catch(std::exception & e) {
		char buff[1024];
		_snprintf_s(buff, _TRUNCATE, PROJECT_NAME " client exception:\n%s", e.what());
		debugf("parseMsgClient exception: %s\n", e.what());
		debugf("Received message: |%s|\n", data.c_str());
		Frontend::callMessageBox(buff, 0, 0);
	}
}



void LobbyProtocol::sendVersionInfoConnection(nlohmann::json &parsed, DWORD connection) {
	nlohmann::json json;
	json["type"] = "VersionInfo";
	Config::addVersionInfoToJson(json);
	LobbyPackets::sendDataToHost(json.dump());
}

void LobbyProtocol::showVersionInfoConnection(nlohmann::json &parsed, DWORD connection) {
	std::stringstream ss;
	ss << "The host is using " << parsed["module"] << " " << parsed["version"] << " (" << parsed["build"] << " )";
	LobbyChat::lobbyPrint((char*)ss.str().c_str());
}

void LobbyProtocol::showVersionInfoSlot(nlohmann::json &parsed, int slot) {
	std::stringstream ss;
	std::string nickname = LobbyPackets::getNicknameBySlot(slot);
	ss << nickname << " is using " << parsed["module"] << " " << parsed["version"] << " (" << parsed["build"] << " )";
	LobbyChat::lobbyPrint((char*)ss.str().c_str());
}

void LobbyProtocol::sendVersionInfoSlot(nlohmann::json &parsed, int slot) {
	nlohmann::json json;
	json["type"] = "VersionInfo";
	Config::addVersionInfoToJson(json);
	LobbyPackets::sendDataToClient_slot(slot, json.dump());
}

void LobbyProtocol::sendVersionQueryToAllPlayers() {
	DWORD hostThis = LobbyChat::getLobbyHostScreen();
	if(!hostThis) {
		debugf("hostThis is null\n");
		return;
	}
	nlohmann::json json;
	json["type"] = "VersionQuery";
	Config::addVersionInfoToJson(json);
	std::string dump = json.dump();
	for(int i=0; i < LobbyPackets::getNumSlots(); i++) {
		LobbyPackets::sendDataToClient_slot(i, dump);
	}
}

void LobbyProtocol::sendVersionQueryToHost() {
	nlohmann::json json;
	json["type"] = "VersionQuery";
	Config::addVersionInfoToJson(json);
	LobbyPackets::sendDataToHost(json.dump());
}

void LobbyProtocol::sendRealTimeStatusToAllPlayers() {
	debugf("...");
	DWORD hostThis = LobbyChat::getLobbyHostScreen();
	if(!hostThis) {
		debugf("hostThis is null\n");
		return;
	}
	nlohmann::json json;
	json["type"] = "RealtimeStatus";
	json["enabled"] = RealTime::isActive();
	Config::addVersionInfoToJson(json);
	std::string dump = json.dump();
	for(int i=0; i < LobbyPackets::getNumSlots(); i++) {
		LobbyPackets::sendDataToClient_slot(i, dump);
	}
}
