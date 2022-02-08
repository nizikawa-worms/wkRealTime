

#include <format>
#include "RealTime.h"
#include "Chat.h"
#include "Config.h"


void RealTime::install() {

}

bool RealTime::isActive() {
	return active;
}

void RealTime::setActive(bool active) {
	RealTime::active = active;
}

int RealTime::onChatInput(std::vector<std::string> &parts) {
	if(parts.size() >= 2) {
		if(parts[0] == "/realtime" || parts[0] == "/rt") {
			if(parts[1] == "on") setActive(true);
			else if(parts[1] == "off") setActive(false);
			Chat::callShowChatMessage(std::format("Realtime: {}", isActive()), 6);
			return 1;
		}
		if(parts[0] == "/fifo") {
			if(parts[1] == "on") setDebugFifo(true);
			else if(parts[1] == "off") setDebugFifo(false);
			Chat::callShowChatMessage(std::format("DebugFifo: {}", isDebugFifo()), 6);
			return 1;
		}
		if(parts[0] == "/ghosts") {
			try {
				unsigned int choice = std::stoi(parts[1]);
				if(choice <= 4) {
					Config::setGhosts(choice);
					Chat::callShowChatMessage(std::format("GhostWorms: {}", Config::getGhosts()), 6);
					return 1;
				} else throw std::runtime_error(std::format("Unknown choice: {}", choice));
			} catch(std::exception & e) {
				Chat::callShowChatMessage(std::format("GhostWorms: {}", e.what()), 6);
			}
		}
	}
	return 0;
}

void RealTime::onFrontendExit() {
	setActive(false);
	setTurn(false);
}

void RealTime::onDestructGameGlobal() {
	setActive(false);
	setTurn(false);
}

bool RealTime::isTurn() {
	return turn;
}

void RealTime::setTurn(bool turn) {
	RealTime::turn = turn;
}

bool RealTime::isDebugFifo() {
	return debugFifo;
}

void RealTime::setDebugFifo(bool debugFifo) {
	RealTime::debugFifo = debugFifo;
}
