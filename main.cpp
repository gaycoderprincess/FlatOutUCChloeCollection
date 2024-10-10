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

#include "fouc.h"
#include "chloemenulib.h"

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutUCChloeCollection_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

#include "config.h"
#include "mallochook.h"
#include "customsave.h"
#include "savegamemover.h"
#include "arcadescoring.h"
#include "playermodels.h"
#include "custominput.h"
#include "profiles.h"
#include "customsettings.h"
#include "stuntmode.h"
#include "soundtrackswapper.h"
#include "ultrawide.h"
#include "cartuning.h"
#include "carlimitadjuster.h"
#include "skippablecopyright.h"
#include "aiextender.h"
#include "luafunctions.h"
#include "soundtweaks.h"
#include "aifudge.h"
#include "aiupgrades.h"
#include "ailuahacks.h"
#include "handlingmode.h"
#include "skindbextender.h"
#include "instantaction.h"
#include "customhud.h"
#include "setupskip.h"
#include "windowedmode.h"
#include "d3dhook.h"
#include "verboseerrors.h"
#include "bombexplosion.h"
#include "testhud.h"
#include "debugmenu.h"
#include "fo2sharedtextures.h"

void SetArcadeCareerCar() {
	if (nArcadeCareerCarVariant) {
		pGame->nInstantActionCar = GetCarMatchup(pGame->nInstantActionCar);
	}
	pGame->nInstantActionCarSkin = nArcadeCareerCarSkin;
}

uintptr_t ArcadeCareerCarSkinASM_jmp = 0x467D63;
void __attribute__((naked)) __fastcall ArcadeCareerCarSkinASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (ArcadeCareerCarSkinASM_jmp), "i" (SetArcadeCareerCar)
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
		if (game->nGameState != GAME_STATE_RACE) return;
		bool isFO1Track = DoesTrackValueExist(game->nLevelId, "UseLowAILookahead");

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

void OnD3DReset() {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
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

float __fastcall MenuCameraRotation(void* a1) {
	float value = 0;
	if (IsKeyPressed(VK_NEXT)) value -= 1;
	if (IsKeyPressed(VK_PRIOR)) value += 1;
	value += GetPadKeyState(NYA_PAD_KEY_RSTICK_X) / 32767.0;
	return value;
}

void SetTrackVisibility() {
	bool increased = false;
	bool increasedNegY = false;
	if (pGame->nGameState == GAME_STATE_RACE) {
		increased = DoesTrackValueExist(pGame->nLevelId, "IncreasedVisibility");
		increasedNegY = DoesTrackValueExist(pGame->nLevelId, "IncreasedNegYVisibility");
	}

	// increase VisibilitySet grid extents for rally trophy tracks
	// rally russia extends to about 4600 for reference
	static float fNegExtentsExtended = -5000.0;
	static float fPosExtentsExtended = 5000.0;
	static float fNegExtents = -4096.0;
	static float fPosExtents = 4096.0;
	static float fNegYExtentsExtended = -100.0;
	static float fNegYExtents = -50.0;
	NyaHookLib::Patch(0x57AD5F + 2, increased ? &fNegExtentsExtended : &fNegExtents);
	NyaHookLib::Patch(0x57AD8E + 2, increased ? &fPosExtentsExtended : &fPosExtents);
	NyaHookLib::Patch(0x57AD6A + 2, increasedNegY ? &fNegYExtentsExtended : &fNegYExtents);
}

uintptr_t InitVisibilityASM_jmp = 0x55AAB9;
void __attribute__((naked)) __fastcall InitVisibilityASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (InitVisibilityASM_jmp), "i" (SetTrackVisibility)
	);
}

void __attribute__((naked)) __fastcall NoTimeTrialGracePeriodASM() {
	__asm__ (
		"mov dword ptr [eax+0x58], 1\n\t"
		"pop ebx\n"
		"add esp, 0x2C\n"
		"ret\n\t"
	);
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
			NyaHookLib::Patch<uint8_t>(0x492B0F, 0xEB); // no new lap record popup

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x465F46, 0x467312); // disable video recording

			NyaFO2Hooks::PlaceD3DHooks();
			NyaFO2Hooks::aEndSceneFuncs.push_back(CustomSetterThread);
			NyaFO2Hooks::aEndSceneFuncs.push_back(D3DHookMain);
			NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);
			NyaFO2Hooks::PlaceWndProcHook();
			NyaFO2Hooks::aWndProcFuncs.push_back(WndProcHook);

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
			ApplyTestHUDPatches();
			ApplyDebugMenuPatches();
			ApplyFO2SharedTexturesPatches();
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

			NyaHookLib::Patch(0x6F38DC+0x54, &MenuCameraRotation);

			InitVisibilityASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55E775, &InitVisibilityASM);

			// never read VS_dynamicScale for cars
			// not sure if this ever does anything otherwise but it makes the FO2 Chili crash for some reason???
			NyaHookLib::Patch<uint8_t>(0x631C88, 0xEB);

			//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A35C3, &NoTimeTrialGracePeriodASM);

			srand(time(0));
		} break;
		default:
			break;
	}
	return TRUE;
}