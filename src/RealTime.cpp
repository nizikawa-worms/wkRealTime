

#include <format>
#include "RealTime.h"
#include "Chat.h"


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
		if(parts[0] == "/realtime") {
			if(parts[1] == "on") setActive(true);
			else if(parts[1] == "off") setActive(false);
			Chat::callShowChatMessage(std::format("Realtime: {}", isActive()), 6);
			return 1;
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
