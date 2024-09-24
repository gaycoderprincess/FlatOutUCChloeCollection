#include <windows.h>
#include <fstream>
#include <format>
#include <thread>
#include <codecvt>
#include <filesystem>
#include "toml++/toml.hpp"
#include "nya_dx9_hookbase.h"
#include "nya_commonhooklib.h"
#include "nya_commonmath.h"

#include "game.h"

#include "config.h"
#include "mallochook.h"
#include "customsave.h"
#include "savegamemover.h"
#include "arcadescoring.h"
#include "playermodels.h"
#include "custominput.h"
#include "profiles.h"
#include "customsettings.h"
#include "soundtrackswapper.h"
#include "ultrawide.h"
#include "cartuning.h"
#include "stuntmode.h"
#include "carlimitadjuster.h"
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
#include "setupskip.h"
#include "windowedmode.h"
#include "d3dhook.h"
#include "skippablecopyright.h"
#include "verboseerrors.h"
#include "bombexplosion.h"

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

uintptr_t NoDebugQuitASM_jmp = 0x55AAB9;
void __attribute__((naked)) __fastcall NoDebugQuitASM() {
	__asm__ (
		"mov eax, 0x80004004\n\t"
		"cmp eax, 0x80004004\n\t"
		"jmp %0\n\t"
			:
			: "m" (NoDebugQuitASM_jmp)
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

auto LoadExplosions = (void(*)())0x4F3270;
void SetExplosionEffects() {
	static int nLastExplosionEffects = -1;
	if (nLastExplosionEffects != nExplosionEffects) {
		float mult = nExplosionEffects ? 1 : 0.05;
		if (nExplosionEffects == 2) mult = 0;

		// Blast
		*(float*)0x84B254 = 20.0 * mult; // BloomScale
		*(float*)0x84B25C = 1.2 * mult; // CameraShakeScale

		// Bomb
		*(float*)(0x84B254 + 0x30) = 6.13 * mult; // BloomScale
		*(float*)(0x84B25C + 0x30) = 1.0 * mult; // CameraShakeScale
		nLastExplosionEffects = nExplosionEffects;
	}

	if (!*(bool*)0x929908C && GetLiteDB()) {
		LoadExplosions();
		nLastExplosionEffects = -1;
	}
}

float __fastcall NoAILookahead(void* a1, uintptr_t a2) {
	auto ai = *(Player**)(a2 + 0x1C);
	ai->fLookAheadMin = 10.0;
	ai->fLookAheadMax = 10.0;
	ai->fLookAheadModifier = 0.1;
	return 10.0;
}

// disable ai lookahead on fo1 tracks
void SetAILookahead() {
	if (auto game = pGame) {
		bool isFO1Track = game->nLevelId >= TRACK_FO1TOWN2A;
		if (game->nLevelId == TRACK_RETRODEMO1B) isFO1Track = false;

		//NyaHookLib::Patch<uint64_t>(0x406CF3, isFO1Track ? 0x818B90000000DEE9 : 0x818B000000DD840F);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x406FE0, isFO1Track ? (uintptr_t)&NoAILookahead : 0x406E50);
	}
}

void CustomSetterThread() {
	SetSoundtrack();
	SetPlayerModel();
	SetHUDType();
	SetAIFudgeFactor();
	SetArrowColor();
	SetSlideControl();
	SetWindowedMode();
	SetAILookahead();
	SetExplosionEffects();
}

auto EndSceneOrig = (HRESULT(__thiscall*)(void*))nullptr;
HRESULT __fastcall EndSceneHook(void* a1) {
	CustomSetterThread();
	D3DHookMain();
	return EndSceneOrig(a1);
}

auto D3DResetOrig = (void(__thiscall*)(void*))nullptr;
void __fastcall D3DResetHook(void* a1) {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
	return D3DResetOrig(a1);
}

auto wndProcCallback = (LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM))0x60E690;
LRESULT __stdcall WndProcCustom(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	WndProcHook(hWnd, msg, wParam, lParam);
	return wndProcCallback(hWnd, msg, wParam, lParam);
}

auto LoadMapIconsTGA_call = (void*(__stdcall*)(void*, const char*, int, int))0x5A6F00;
void* __stdcall LoadMapIconsTGA(void* a1, const char* a2, int a3, int a4) {
	SetTextureFolder("data/global/overlay/");
	auto ret = LoadMapIconsTGA_call(a1, a2, a3, a4);
	SetTextureFolder("data/global/map/");
	return ret;
}

auto UpdateCameraHooked_call = (void(__thiscall*)(void*, float))0x4FAEA0;
void __fastcall UpdateCameraHooked(void* a1, void*, float a2) {
	if (nHighCarCam) {
		auto bak = pCameraManager->pTarget->mMatrix[13];
		pCameraManager->pTarget->mMatrix[13] += 0.25;
		UpdateCameraHooked_call(a1, a2);
		pCameraManager->pTarget->mMatrix[13] = bak;
	}
	else {
		UpdateCameraHooked_call(a1, a2);
	}
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x24CEF7) {
				MessageBoxA(nullptr, aFOUCVersionFail, "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

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

			static const char* aiDamageMeter = "ai_damage_meter_2";
			NyaHookLib::Patch(0x4DEC01 + 1, aiDamageMeter);
			NyaHookLib::Patch(0x4DEC2A + 1, aiDamageMeter);

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
			D3DResetOrig = (void(__thiscall*)(void*))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x60F744, &D3DResetHook);
			wndProcCallback = (LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM))(*(uintptr_t*)0x60F0CF);
			NyaHookLib::Patch(0x60F0CF, &WndProcCustom);

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
			ApplySetupSkipPatches();
			ApplyWindowedModePatches();
			ApplyUltrawidePatches();
			ApplyProfilePatches();
			ApplyVerboseErrorsPatches();
			ApplySkippableCopyrightPatches();
			ApplyBombExplosionPatches();
			ApplyCarLimitAdjuster();
			ApplyCarTuningPatches();
			ApplyStuntModeAirControlPatch();
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

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55AAB4, &NoDebugQuitASM);

			// road king gives out of bounds when reading the Car[%d] db
			// value is set to 255 by 00487953 (profile reset) and is then read as 255 by 4C7EB7
			// was 32 on game startup, inited by sub_45EBE0
			// removing this for now, it resets arcade scores seemingly, so useless here cuz of the new arcade save system
			NyaHookLib::Patch<uint8_t>(0x48794C, 0xEB);
			NyaHookLib::Patch<uint8_t>(0x48796F, 0xEB);

			UpdateCameraHooked_call = (void(__thiscall*)(void*, float))(*(uintptr_t*)0x6EB7DC);
			NyaHookLib::Patch(0x6EB7DC, &UpdateCameraHooked);

			// increase lua variables memory limit
			NyaHookLib::Patch(0x673E0D + 1, 0x900000);

			// don't set motion blur based on particle quality
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x46D732, 0x46D757);

			// remove hardcoded ReplicatedSession car classes
			const char* tmpCarClass = "CARCLASS_AAAA";
			for (int i = 0x714008; i < 0x714028; i += 4) {
				NyaHookLib::Patch(i, tmpCarClass);
			}

			// remove hardcoded ReplicatedSession gamemodes
			for (int i = 0x71406C; i < 0x714098; i += 4) {
				NyaHookLib::Patch(i, tmpCarClass);
			}
		} break;
		default:
			break;
	}
	return TRUE;
}