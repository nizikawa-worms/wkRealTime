#include "Drawing.h"
#include "Hooks.h"
#include "WaLibc.h"
#include "W2App.h"
#include "Config.h"
#include "GameNet.h"

BitmapImage * (__stdcall *origTextboxSetText)(BitmapTextbox * box, char *text, int text_color, int color1, int color2, int * width, int * height, int opacity);
BitmapImage * __stdcall Drawing::hookTextboxSetText(BitmapTextbox * box, char *text, int text_color, int color1, int color2, int * width, int * height, int opacity) {

	return origTextboxSetText(box, text, text_color, color1, color2, width, height, opacity);
}

BitmapTextbox *(__fastcall *addrConstructTextbox)(DWORD DdDisplay, int EDX, BitmapTextbox *box, int maxlength, int font);
BitmapTextbox::BitmapTextbox(int maxlength, int font) {
	DWORD addrDdDisplay = W2App::getAddrDdDisplay();
	if(!addrDdDisplay) throw std::runtime_error("DD Display is null");
	memset((void*)this, 0, sizeof(BitmapTextbox));
	addrConstructTextbox(addrDdDisplay, 0, this, maxlength, font);
}

BitmapImage * BitmapTextbox::setText(char *text, int *width, int *height, int text_color, int color1, int color2, int opacity) {
	return Drawing::hookTextboxSetText(this, text, text_color, color1, color2, width, height, opacity);
}

void *BitmapTextbox::operator new(size_t size) {
	return WaLibc::waMalloc(size);
}

void BitmapTextbox::operator delete(void *p) {
	WaLibc::waFree(p);
}



int (__fastcall *origDrawBitmapImageScreen)(DWORD gamescene, int posY_a2, int a3, int posX_a4, BitmapImage *bmp, int a6, int a7, int a8, int a9);
int __fastcall hookDrawBitmapImageScreen(DWORD gamescene, int posY_a2, int a3, int posX_a4, BitmapImage *bmp, int a6, int a7, int a8, int a9) {
	return origDrawBitmapImageScreen(gamescene, posY_a2, a3, posX_a4, bmp, a6, a7, a8, a9);
}

void BitmapImage::drawScreen(int posY, int a3, int posX, int a6, int a7, int a8, int a9) {
	DWORD gameglobal = W2App::getAddrGameGlobal();
	if(!gameglobal) return;
	DWORD gamescene = *(DWORD*)(gameglobal + 0x524);
	if(!gamescene) return;
	hookDrawBitmapImageScreen(gamescene, posY, a3, posX, this, a6, a7, a8, a9);
}

int (__stdcall *origTurnGameRenderScene)();
void Drawing::hookTurnGameRenderScene_c() {
//	GameNet::enqueueDebugFifo();
	TaskMessageFifo::onTurnGameRenderScene();
}
int __stdcall Drawing::hookTurnGameRenderScene() {
	int turngame, retv;
	_asm mov turngame, edi

	hookTurnGameRenderScene_c();

	_asm mov edi, turngame
	_asm call origTurnGameRenderScene
	_asm mov retv, eax

	return retv;
}



//_DWORD *__userpurge draw_sprite_local_sub_542060@<eax>(
//        _DWORD *posy_result@<eax>,
//        _DWORD *gamescene_a2@<ecx>,
//        int layer_a3,
//        int posx_a4,
//        int sprite_id_a5,
//        int frame_a6)
//{

DWORD (__stdcall *origDrawSpriteLocal)(DWORD layer, DWORD posx, DWORD sprite, DWORD frame);
DWORD __stdcall Drawing::hookDrawSpriteLocal(DWORD layer, DWORD posx, DWORD sprite, DWORD frame) {
	DWORD posy, gamescene, retv;
	_asm mov posy, eax
	_asm mov gamescene, ecx

	sprite |= spriteMask;

	_asm push frame
	_asm push sprite
	_asm push posx
	_asm push layer
	_asm mov eax, posy
	_asm mov ecx, gamescene
	_asm call origDrawSpriteLocal
	_asm mov retv, eax

	return retv;
}

void Drawing::install() {
	DWORD addrTextboxSetText = Hooks::scanPattern("TextboxSetText", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x8B\x44\x24\x18\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x10\x53\x8B\x5C\x24\x24\x3B\x83\x00\x00\x00\x00\x55", "???????xx????xxxxxxxx????xxxxxxxxxx????x");
	addrConstructTextbox = (BitmapTextbox *(__fastcall *)(DWORD,int,BitmapTextbox *,int,int))
			Hooks::scanPattern("ConstructTextbox", "\x6A\xFF\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51\x8B\x44\x24\x1C\x53\x8B\x5C\x24\x1C\x55\x8B\x6C\x24\x1C\x56\x57\x8B\xF1\x89\x45\x08", "???????xx????xxxx????xxxxxxxxxxxxxxxxxxxxxx");
	DWORD addrDrawBitmapImageScreen = Hooks::scanPattern("DrawBitmapImageScreen", "\x8B\x81\x00\x00\x00\x00\x3D\x00\x00\x00\x00\x56\x8B\x74\x24\x0C\x7D\x79\x57\x8B\x39\x83\xC7\xD8\x78\x70\x89\x39\x8D\x7C\x0F\x04\x89\xBC\x81\x00\x00\x00\x00\x8B\xB9\x00\x00\x00\x00\x8B\x84\xB9\x00\x00\x00\x00\x83\xC7\x01\x85\xC0\x89\xB9\x00\x00\x00\x00\x74\x49\x8B\x4C\x24\x0C\x81\xE6\x00\x00\x00\x00\x81\xE2\x00\x00\x00\x00\x89\x50\x0C\x8B\x54\x24\x14\x89\x48\x04\x8B\x4C\x24\x18\x89\x50\x10\x8B\x54\x24\x1C\x89\x48\x18\x8B\x4C\x24\x20",
														 "??????x????xxxxxxxxxxxxxxxxxxxxxxxx????xx????xxx????xxxxxxx????xxxxxxxx????xx????xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	DWORD addrTurnGameRenderScene = Hooks::scanPattern("TurnGameRenderScene", "\x83\xEC\x28\x8B\x47\x2C\x8B\x48\x24\x8B\x91\x00\x00\x00\x00\x85\xD2\x53\x8B\x98\x00\x00\x00\x00\x55\x8B\xA8\x00\x00\x00\x00\x56\x89\x5C\x24\x1C\x89\x6C\x24\x20\x7E\x4E", "??????xxxxx????xxxxx????xxx????xxxxxxxxxxx");
	DWORD origDrawSpriteGlobal = Hooks::scanPattern("DrawSpriteGlobal", "\x8B\x91\x00\x00\x00\x00\x81\xFA\x00\x00\x00\x00\x56\x57\x8B\x7C\x24\x10\x8B\xF0\x7D\x5B\x8B\x01\x83\xC0\xE8\x78\x54\x89\x01\x8D\x44\x08\x04\x89\x84\x91\x00\x00\x00\x00\x8B\x91\x00\x00\x00\x00", "??????xx????xxxxxxxxxxxxxxxxxxxxxxxxxx????xx????", 0x541FE0);
	DWORD addrDrawSpriteLocal = origDrawSpriteGlobal + (0x541BA0 - 0x541B20);

	_HookDefault(TextboxSetText);
	_HookDefault(TurnGameRenderScene);
	_HookDefault(DrawBitmapImageScreen);
	_HookDefault(DrawSpriteLocal);
}

void Drawing::onConstructGameGlobal() {
	spriteMask = 0;
}

void Drawing::onDestructGameGlobal() {
}

void Drawing::setSpriteMask(DWORD spriteMask) {
	Drawing::spriteMask = spriteMask;
}

