#include "Hooks.h"
#include "WeaponPanel.h"
#include "RealTime.h"

int (__stdcall *origShadowWeaponTile)(int weapon_id);
int __stdcall WeaponPanel::hookShadowWeaponTile(int weapon_id) {
	WeaponPanel *panel;
	int retv;
	_asm mov panel, eax

	if(RealTime::isActive()) {
		return 0;
	}

	_asm mov eax, panel
	_asm push weapon_id
	_asm call origShadowWeaponTile
	_asm mov retv, eax

	return retv;
}

void WeaponPanel::install() {
	DWORD addrShadowWeaponTile = _ScanPattern("ShadowWeaponTile", "\x8B\x48\x04\x8B\x54\x88\x20\x69\xD2\x00\x00\x00\x00\x53\x8B\x5C\x24\x08\x56\x8B\x30\x03\xD3\x83\xBC\x96\x00\x00\x00\x00\x00\x74\x0A\x5E\xB8\x00\x00\x00\x00\x5B\xC2\x04\x00", "??????xxx????xxxxxxxxxxxxx?????xxxx????xxxx");
	_HookDefault(ShadowWeaponTile);
}
