

#ifndef WKREALTIME_CAMERA_H
#define WKREALTIME_CAMERA_H

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
public:
	static void install();
};


#endif //WKREALTIME_CAMERA_H
