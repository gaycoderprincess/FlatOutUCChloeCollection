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
#include "custominput.h"
#include "profiles.h"
#include "customsettings.h"
#include "stuntmode.h"
#include "soundtrackswapper.h"
#include "ultrawide.h"
#include "cartuning.h"
#include "carlimitadjuster.h"
#include "playermodels.h"
#include "skippablecopyright.h"
#include "aiextender.h"
#include "pacenotes.h"
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
#include "fo2sharedtextures.h"
#include "trackextender.h"
#include "buoyancy.h"
#include "nonetwork.h"
#include "debugmenu.h"

void SetArcadeCareerCar() {
	if (nArcadeCareerCarVariant) {
		pGameFlow->nInstantActionCar = GetCarMatchup(pGameFlow->nInstantActionCar);
	}
	pGameFlow->nInstantActionCarSkin = nArcadeCareerCarSkin;
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
			ApplyTrackExtenderPatches();
			ApplyBuoyancyPatches();
			ApplyNoNetworkPatches();
			*(uint32_t*)0x8494D4 = 1; // set ShowBonus to always true

			InitCustomSave();
			LoadPacenoteConfigs();

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

			// never read VS_dynamicScale for cars
			// not sure if this ever does anything otherwise but it makes the FO2 Chili crash for some reason???
			NyaHookLib::Patch<uint8_t>(0x631C88, 0xEB);

			srand(time(0));
		} break;
		default:
			break;
	}
	return TRUE;
}