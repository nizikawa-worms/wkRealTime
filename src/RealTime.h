

#ifndef WKREALTIME_REALTIME_H
#define WKREALTIME_REALTIME_H


#include <vector>
#include <string>

class RealTime {
private:
	static inline bool active = false;
	static inline bool turn = false;
public:
	static void install();

	static bool isActive();
	static void setActive(bool active);
	static int onChatInput(std::vector<std::string> & parts);

	static void onFrontendExit();
	static void onDestructGameGlobal();

	static bool isTurn();

	static void setTurn(bool turn);
};


#endif //WKREALTIME_REALTIME_H
