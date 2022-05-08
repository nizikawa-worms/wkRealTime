
#ifndef WKREALTIME_WEAPONPANEL_H
#define WKREALTIME_WEAPONPANEL_H


class WeaponPanel {
private:
	static int __stdcall hookShadowWeaponTile(int weapon_id);
public:
	static void install();
};


#endif //WKREALTIME_WEAPONPANEL_H
