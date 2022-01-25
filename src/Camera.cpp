
#include "Camera.h"
#include "Hooks.h"
#include "entities/CGameTask.h"
#include "RealTime.h"
#include "entities/CTaskWorm.h"
#include "entities/CTaskTeam.h"
#include "Utils.h"
#include "Debugf.h"
#include "W2App.h"

DWORD (__fastcall *origFixedCameraCenterAtObject)(DWORD posx, DWORD posy, CGameTask *object);
DWORD __fastcall hookFixedCameraCenterAtObject(DWORD posx, DWORD posy, CGameTask *object) {
	if(RealTime::isActive() && object->classtype == Constants::ClassType_Task_Team) {
		CameraFixedLookAt * cameraFixedLookAt = (CameraFixedLookAt*)(object->gameglobal2c + 0x739C);
		cameraFixedLookAt->enabled = 1;
		CTaskTeam * team = (CTaskTeam*)object;
		if(!team->isOwnedByMe()) {
			return 0;
		}
	}
	return origFixedCameraCenterAtObject(posx, posy, object);
}

DWORD (__fastcall *origCameraLookAtMe)(DWORD posx, DWORD posy, CGameTask *object, int priority);
DWORD __fastcall hookCameraLookAtMe(DWORD posx, DWORD posy, CGameTask *object, int priority) {
//	debugf("X: %lf Y: %lf Obj: 0x%X priority: %d\n", Utils::fixedToDouble(posx), Utils::fixedToDouble(posy), object, priority);
	if(RealTime::isActive() && object->classtype == Constants::ClassType_Task_Team) {
		CTaskTeam * team = (CTaskTeam*)object;
		if(!team->isOwnedByMe()) {
//			priority = std::clamp(priority+3, 0, 14);
			return 0;
		}
	}

	return origCameraLookAtMe(posx, posy, object, priority);
}

void Camera::install() {
	DWORD addrFixedCameraCenterAtObject = _ScanPattern("FixedCameraCenterAtObject", "\x8B\x44\x24\x04\x56\x8B\x70\x2C\xC7\x86\x00\x00\x00\x00\x00\x00\x00\x00\x8B\x40\x2C\x05\x00\x00\x00\x00\x83\x38\x00\x5E\x75\x15\x89\x48\x0C\x89\x48\x04\x89\x50\x08\xC7\x00\x00\x00\x00\x00\x89\x50\x10\xC2\x04\x00", "??????xxxx????????xxxx????xxxxxxxxxxxxxxxxx????xxxxxx");
	DWORD addrCameraLookAtMe = _ScanPattern("CameraLookAtMe", "\x8B\x44\x24\x04\x8B\x40\x2C\x83\xB8\x00\x00\x00\x00\x00\x56\x8B\x74\x24\x0C\x75\x4A\x8D\x34\xB6\x83\xBC\xB0\x00\x00\x00\x00\x00\x8D\x84\xB0\x00\x00\x00\x00\x75\x16\x89\x48\x0C\x89\x48\x04\x89\x50\x08", "??????xxx?????xxxxxxxxxxxxx?????xxx????xxxxxxxxxxx");
	_HookDefault(FixedCameraCenterAtObject);
	_HookDefault(CameraLookAtMe);
}

void CameraFixedLookAt::print() {
	debugf("enabled: %d x1: %lf x2: %lf y1: %lf y2: %lf\n", enabled, Utils::fixedToDouble(posx1), Utils::fixedToDouble(posx2), Utils::fixedToDouble(posy1), Utils::fixedToDouble(posy2));
}
