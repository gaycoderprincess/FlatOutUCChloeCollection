#include <windows.h>
#include <fstream>
#include <format>
#include <thread>
#include "toml++/toml.hpp"
#include "nya_commonhooklib.h"

#include "game.h"

#include "config.h"
#include "mallochook.h"
#include "customsave.h"
#include "savegamemover.h"
#include "soundtrackswapper.h"
#include "arcadescoring.h"
#include "playermodels.h"
#include "luafunctions.h"
#include "soundtweaks.h"
#include "aifudge.h"
#include "aiextender.h"
#include "aiupgrades.h"
#include "ailuahacks.h"
#include "handlingmode.h"
#include "skindbextender.h"
#include "instantaction.h"
#include "customhud.h"
#include "customsettings.h"

uintptr_t ArcadeCareerCarSkinASM_jmp = 0x467D63;
void __attribute__((naked)) __fastcall ArcadeCareerCarSkinASM() {
	__asm__ (
		"mov edx, %1\n\t"
		"mov [ebp+0x4DC], edx\n\t"
		"jmp %0\n\t"
			:
			: "m" (ArcadeCareerCarSkinASM_jmp), "m" (nArcadeCareerCarSkin)
	);
}

uint32_t bSetupDialogRan = 0;
uintptr_t SkipSetupASM_jmpSetup = 0x457252;
uintptr_t SkipSetupASM_jmpNoSetup = 0x4572B7;
void __attribute__((naked)) __fastcall SkipSetupASM() {
	__asm__ (
		// show setup with the -setup parameter
		"mov edx, 0x8465EC\n\t"
		"cmp dword ptr [edx], 0\n\t"
		"je noSetup\n\t"

		"mov edx, 1\n\t"
		"mov %2, edx\n\t"
		"xor edx, edx\n\t"
		"push ebx\n\t"
		"push ebx\n\t"
		"push ebx\n\t"
		"push ebx\n\t"
		"push 0x82\n\t"
		"push edi\n\t"
		"jmp %0\n\t"

		"noSetup:\n\t"
		"xor edx, edx\n\t"
		"jmp %1\n\t"
			:
			: "m" (SkipSetupASM_jmpSetup), "m" (SkipSetupASM_jmpNoSetup), "m" (bSetupDialogRan)
	);
}

void SetArrowColor() {
	static int nLastArrowColor = -1;
	if (nLastArrowColor != nArrowColor) {
		int colorId = nArrowColor;
		if (colorId <= 0) colorId = -1; // player color is stored 1 space behind ai colors for some reason
		NyaHookLib::Patch(0x469835, colorId + 65);
		NyaHookLib::Patch(0x46983B, 0x9298C74 + (colorId * 4));
		NyaHookLib::Patch(0x469875, colorId + 65);
		NyaHookLib::Patch(0x46987B, 0x9298C74 + (colorId * 4));
		nLastArrowColor = nArrowColor;
	}
}

RECT GetMonitorRect(HWND hwnd) {
	RECT rect;
	auto monitor = MonitorFromWindow(hwnd, 0);
	MONITORINFO mi;
	memset(&mi, 0, sizeof(mi));
	mi.cbSize = sizeof(mi);
	if (monitor && GetMonitorInfoA(monitor, &mi)) rect = mi.rcWork;
	else SystemParametersInfoA(0x30u, 0, &rect, 0);
	return rect;
}

void SetWindowedMode() {
	static int nLastWindowed = -1;
	if (nLastWindowed != nWindowedMode) {
		auto hwnd = *(HWND*)(0x7242B0 + 0x7C);
		auto resX = *(int*)0x764A84;
		auto resY = *(int*)0x764A88;
		auto rect = GetMonitorRect(hwnd);

		auto style = GetWindowLongA(hwnd, GWL_STYLE);
		uint32_t targetStyle = (WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		if (nWindowedMode) style |= targetStyle;
		else style &= ~targetStyle;
		SetWindowLongA(hwnd, GWL_STYLE, style);
		SetWindowPos(hwnd, nullptr, rect.left, rect.top, resX, resY, SWP_NOZORDER | SWP_FRAMECHANGED);
		SetFocus(hwnd);
		nLastWindowed = nWindowedMode;
	}
}

void CustomSetterThread() {
	// set windowed mode to the desired option if the setup dialog is used
	if (bSetupDialogRan) {
		nWindowedMode = *(int*)0x764AAC;
	}

	SetSoundtrack();
	SetPlayerModel();
	SetHUDType();
	SetAIFudgeFactor();
	SetArrowColor();
	SetSlideControl();
	SetWindowedMode();
}

auto EndSceneOrig = (HRESULT(__thiscall*)(void*))nullptr;
HRESULT __fastcall EndSceneHook(void* a1) {
	CustomSetterThread();
	return EndSceneOrig(a1);
}

auto LoadMapIconsTGA_call = (void*(__stdcall*)(void*, const char*, int, int))0x5A6F00;
void* __stdcall LoadMapIconsTGA(void* a1, const char* a2, int a3, int a4) {
	SetTextureFolder("data/global/overlay/");
	auto ret = LoadMapIconsTGA_call(a1, a2, a3, a4);
	SetTextureFolder("data/global/map/");
	return ret;
}

void __stdcall TextureErrorHooked(const char* message, const char* path) {
	std::string err;
	if (sTextureFolder[0] && sSharedTextureFolder[0]) {
		err = std::format("Cannot find texture {} in either {} or {}", path, sTextureFolder, sSharedTextureFolder);
	}
	else if (sTextureFolder[0] || sSharedTextureFolder[0]) {
		err = std::format("Cannot find texture {} in {}{}", path, sTextureFolder, sSharedTextureFolder);
	}
	else err = std::format("Cannot find texture {}", path);
	MessageBoxA(nullptr, err.c_str(), "Fatal error", 0x10);
	exit(0);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x24CEF7) {
				MessageBoxA(nullptr, aFOUCVersionFail, "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

			// don't show cursor
			NyaHookLib::Patch(0x60F23B, 0x9090D231);

			// always run in windowed - this'll be borderless
			static uint32_t bWindowedTmp = 1;
			NyaHookLib::Patch(0x4572DA, &bWindowedTmp);

			// skip setup window if already configured
			if (std::filesystem::exists("Savegame/device.cfg")) {
				NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x457248, &SkipSetupASM);
			}
			else bSetupDialogRan = true;

			// there's a check against car count at 46B440
			// seems to check if the current car was a bonus car?

			// after race finish, autopilot starts being read:
			// 00481EAB
			// 00478EBB
			// 00478F96
			// 00478F0B
			// 00479FD7
			// set by game->0x2820->0x34
			// which is set at 00481F96 when race ends
			NyaHookLib::Fill(0x464CC2, 0x90, 0x464CCA - 0x464CC2); // disable autopilot

			// Event is read from game+0x4C4
			// CarSkin is read from game+0x4DC
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x467D5A, &ArcadeCareerCarSkinASM);

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4F0F0A, &LoadMapIconsTGA);

			// highscoremanager read at:
			// 00492ACD when finishing a lap
			// 00465305 and 004703A0 when quitting
			// 00468FE8 when loaded into menu
			// uploading race results is lang entry 246 (0xF6), called at 4AF733 for event 3015, from event 7025
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x465270, 0x46543C); // no highscore updating
			NyaHookLib::Patch<uint8_t>(0x4675E6, 0xEB); // no uploading race results

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x465F46, 0x467312); // disable video recording

			EndSceneOrig = (HRESULT(__thiscall*)(void*))(*(uintptr_t*)0x677448);
			NyaHookLib::Patch(0x677448, &EndSceneHook);
			HookMalloc();
			ApplyCustomSettingsPatches();
			ApplySoundtrackPatches();
			ApplySavegameMoverPatches();
			ApplySkinDBExtenderPatches();
			ApplyAIExtenderPatches();
			ApplyAIUpgradesPatches();
			ApplyAILUAHacks();
			ApplyLUAPatches();
			ApplyArcadeScoringPatches();
			ApplyInstantActionPatches();
			ApplyCustomHUDPatches();
			ApplyPlayerModelPatches();
			ApplySoundTweaks();
			*(uint32_t*)0x8494D4 = 1; // set ShowBonus to always true

			// carnage total score is set +0x3CC off player profile
			// cleared at 487934, 48831B, also with a memset somewhere
			// added together with the normal car count:
			// 0048836E
			// 004883A3
			// 004883DB
			// 00488416
			// 00488451
			// events are unlocked at 0046504A
			InitCustomSave();

			// crash exiting an fo1 level at 60d294
			// d3ddevice+0x7F8 seems to be the end of a fixed size array
			// maybe this'll help? increased texture & vertex/index buffer max count
			NyaHookLib::Patch(0x60D3D2 + 1, 8192);
			NyaHookLib::Patch(0x70E224, 8192);

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x5A71FD, &TextureErrorHooked);
		} break;
		default:
			break;
	}
	return TRUE;
}