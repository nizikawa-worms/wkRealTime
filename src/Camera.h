

#ifndef WKREALTIME_CAMERA_H
#define WKREALTIME_CAMERA_H

class CGameTask;
typedef unsigned long       DWORD;
struct CameraFixedLookAt {
	DWORD enabled;
	DWORD posx1;
	DWORD posx2;
	DWORD posy1;
	DWORD posy2;
	// probably it's a list of (posx, posy, ...) pairs for each priority level
	void print();
};

class Camera {
private:
	static inline int followingTeam = 0;
	static DWORD __fastcall hookFixedCameraCenterAtObject(DWORD posx, DWORD posy, CGameTask *object);
public:
	static void install();
	static void onConstructGameGlobal();
	static int getFollowingTeam();
	static void setFollowingTeam(int followingTeam);
};


#endif //WKREALTIME_CAMERA_H
