

#ifndef WKREALTIME_DRAWING_H
#define WKREALTIME_DRAWING_H

#include <memory>

typedef unsigned long DWORD;
class BitmapImage {
public:
	virtual int _fastcall vt0(BitmapImage * This, int EDX, int a2, int a3, int a4, int a5, int a6) = 0;
	virtual int _fastcall vt4(BitmapImage * This, int EDX, int a2, int a3, int a4, int a5) = 0;
	virtual int _fastcall vt8(BitmapImage * This, int EDX, int a2, int a3, int a4, int a5) = 0;
	virtual int _fastcall vtC(BitmapImage * This, int EDX, int a2) = 0;
	virtual int _fastcall vt10(BitmapImage * This, int EDX, int a2, int a3) = 0;
	virtual int _fastcall vt14(BitmapImage * This, int EDX, int a2, int a3, int a4) = 0; //20
	virtual int _fastcall vt18(BitmapImage * This, int EDX, int a2, int a3) = 0; //24
	virtual int _fastcall vt1C(BitmapImage * This, int EDX, int a2, int a3, int a4) = 0; //28
	virtual int _fastcall vt20(BitmapImage * This, int EDX) = 0; //32
	virtual int _fastcall vt24(BitmapImage * This, int EDX, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9) = 0; //36
	virtual int _fastcall vt28(BitmapImage * This, int EDX, int a2, int a3, int a4, int a5, int a6) = 0; //40

//		int vtable_dword0; // 0x0
	int unknown4; // 0x4
	int data_dword8; // 0x8
	int bitdepth_dwordC; // 0xC
	int rowsize_dword10; // 0x10
	int max_width_dword14; // 0x14
	int max_height_dword18; // 0x18
	int unknown1C; // 0x1C
	int unknown20; // 0x20
	int current_width_dword24; // 0x24
	int current_height_dword28; // 0x28
	// values below are probably unused / wrong size of structure
	int unknown2C; // 0x2C
	int unknown30; // 0x30
	int unknown34; // 0x34
	int unknown38; // 0x38
	int unknown3C; // 0x3C
	int unknown40; // 0x40
	int unknown44; // 0x44
	int unknown48; // 0x48
	static constexpr int vtNum = 11;

	void drawScreen(int posY, int a3, int posX, int a6, int a7, int a8, int a9);
};

class BitmapImage8 : public BitmapImage {};
class BitmapImage1 : public BitmapImage {};



class BitmapTextbox {
public:
	static const int defaultTextColor = 6;
	static const int defaultTextBackground = 21;
	static const int defaultTextFrame = 54;
	static const int defaultTextFont = 1;
	static const int defaultTextOpacity = 0x10000;
public:
	DWORD width_dword0;
	DWORD height_dword4;
	DWORD fontid_dword8;
	DWORD bitmap_dwordC;
	DWORD dword10;
	DWORD dword14;
	DWORD dword18;
	DWORD message_dword1C;
	DWORD dword20;
	DWORD dword24;
	DWORD dword28;
	DWORD dword2C;
	DWORD dword30;
	DWORD dword34;
	DWORD dword38;
	DWORD dword3C;
	DWORD dword40;
	DWORD dword44;
	DWORD dword48;
	DWORD dword4C;
	DWORD dword50;
	DWORD dword54;
	DWORD dword58;
	DWORD dword5C;
	DWORD dword60;
	DWORD dword64;
	DWORD dword68;
	DWORD dword6C;
	DWORD dword70;
	DWORD dword74;
	DWORD dword78;
	DWORD dword7C;
	DWORD dword80;
	DWORD dword84;
	DWORD dword88;
	DWORD dword8C;
	DWORD dword90;
	DWORD dword94;
	DWORD dword98;
	DWORD dword9C;
	DWORD dwordA0;
	DWORD dwordA4;
	DWORD dwordA8;
	DWORD dwordAC;
	DWORD dwordB0;
	DWORD dwordB4;
	DWORD dwordB8;
	DWORD dwordBC;
	DWORD dwordC0;
	DWORD dwordC4;
	DWORD dwordC8;
	DWORD dwordCC;
	DWORD dwordD0;
	DWORD dwordD4;
	DWORD dwordD8;
	DWORD dwordDC;
	DWORD dwordE0;
	DWORD dwordE4;
	DWORD dwordE8;
	DWORD dwordEC;
	DWORD dwordF0;
	DWORD dwordF4;
	DWORD dwordF8;
	DWORD dwordFC;
	DWORD dword100;
	DWORD dword104;
	DWORD dword108;
	DWORD dword10C;
	DWORD dword110;
	DWORD dword114;
	DWORD dword118;
	DWORD textcolor_dword11C;
	DWORD width_dword120;
	DWORD height_dword124;
	DWORD color1_dword128;
	DWORD color2_dword12C;
	DWORD opacity_dword130;
	DWORD dword134;
	DWORD dword138;
	DWORD dword13C;
	DWORD dword140;
	DWORD dword144;
	DWORD dword148;
	DWORD dword14C;
	DWORD dword150;
	DWORD dword154;

	BitmapTextbox(int maxlength=256, int font=defaultTextFont);
	void * operator new(size_t size);
	void operator delete(void * p);

	BitmapImage * setText(char *text, int *width, int *height, int text_color = defaultTextColor, int color1 = defaultTextBackground, int color2 = defaultTextFrame, int opacity = defaultTextOpacity);

};


class Drawing {
private:
	static void hookTurnGameRenderScene_c();
	static int __stdcall hookTurnGameRenderScene();

	static DWORD __stdcall hookDrawSpriteLocal(DWORD layer, DWORD posx, DWORD sprite, DWORD frame);
	static inline DWORD spriteMask = 0;
public:
	static BitmapImage * __stdcall hookTextboxSetText(BitmapTextbox * box, char *text, int text_color, int color1, int color2, int * width, int *height, int opacity);
	static void onConstructGameGlobal();
	static void onDestructGameGlobal();
	static void install();

	static void setSpriteMask(DWORD spriteMask);
};


#endif //WKREALTIME_DRAWING_H
