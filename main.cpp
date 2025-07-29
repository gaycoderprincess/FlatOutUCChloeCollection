#include <windows.h>
#include <fstream>
#include <format>
#include <thread>
#include <codecvt>
#include <filesystem>
#include <d3dx9.h>
#include "toml++/toml.hpp"
#include "nya_dx9_hookbase.h"
#include "nya_commonhooklib.h"
#include "nya_commonmath.h"

#include "fouc.h"
#include "fo2versioncheck.h"
#include "chloemenulib.h"

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutUCChloeCollection_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

#include "config.h"
#include "mallochook.h"
#include "achievements.h"
#include "customsave.h"
#include "savegamemover.h"
#include "arcadescoring.h"
#include "custominput.h"
#include "playermodels.h"
#include "profiles.h"
#include "customsettings.h"
#include "stuntmode.h"
#include "speedtrapmode.h"
#include "lapknockoutmode.h"
#include "driftcamera.h"
#include "driftmode.h"
#include "soundtrackswapper.h"
#include "ultrawide.h"
#include "cartuning.h"
#include "carlimitadjuster.h"
#include "skippablecopyright.h"
#include "careertimetrial.h"
#include "careerrally.h"
#include "aiextender.h"
#include "pacenotes.h"
#include "handlingmode.h"
#include "luafunctions.h"
#include "soundtweaks.h"
#include "aifudge.h"
#include "aiupgrades.h"
#include "ailuahacks.h"
#include "skindbextender.h"
#include "instantaction.h"
#include "customhud.h"
#include "setupskip.h"
#include "windowedmode.h"
#include "trackextender.h"
#include "d3dhook.h"
#include "verboseerrors.h"
#include "bombexplosion.h"
#include "testhud.h"
#include "fo2sharedtextures.h"
#include "buoyancy.h"
#include "nonetwork.h"
#include "ddsparser.h"
#include "carunlocks.h"
#include "debugmenu.h"

void SetArcadeCareerCar() {
	if (nArcadeCareerCarVariant) {
		pGameFlow->PreRace.nCar = GetCarMatchup(pGameFlow->PreRace.nCar);
	}
	pGameFlow->PreRace.nCarSkin = nArcadeCareerCarSkin;
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

NyaDrawing::CNyaRGBA32 aPlayerColors[] = {
		{255,241,195}, // default off white
		{236,221,16}, // jack yellow
		{255,122,0}, // orange
		{247,34,27}, // katie red
		{131,24,28}, // lei red
		{186,186,186}, // sofia gray
		{100,100,100}, // jason gray
		{245,135,135}, // sally pink
		{219,100,193}, // chloe collection pink
		{255,0,154}, // jill pink
		{135,50,220}, // light purple
		{89,232,247}, // ray cyan
		{54,93,246}, // frank blue
		{50,50,165}, // neville blue
		{230,175,105}, // lewis cream
		{0,190,0}, // light green
		{0,115,0}, // dark green
};

void SetPlayerColor() {
	for (auto& color : aPlayerColors) {
		auto& dest = *(NyaDrawing::CNyaRGBA32*)&gPalette[(&color - &aPlayerColors[0]) + 100];
		dest.r = color.b;
		dest.g = color.g;
		dest.b = color.r;
		dest.a = 255;
	}

	static int nLastPlayerColor = -1;
	if (nLastPlayerColor != nPlayerColor) {
		int colorId = nPlayerColor;
		if (colorId < 0) colorId = 0;
		NyaHookLib::Patch(0x469835, colorId + 100);
		NyaHookLib::Patch(0x46983B, &gPalette[colorId + 100]);
		NyaHookLib::Patch(0x469875, colorId + 100);
		NyaHookLib::Patch(0x46987B, &gPalette[colorId + 100]);
		nLastPlayerColor = nPlayerColor;
	}
}

void SetExplosionEffects() {
	static int nLastExplosionEffects = -1;
	if (nLastExplosionEffects != nExplosionEffects) {
		float mult = nExplosionEffects ? 1 : 0.05;
		if (nExplosionEffects == 2) mult = 0;

		gExplosionBloom[EXPLOSION_BLAST].fBloomScale = 20.0 * mult;
		gExplosionBloom[EXPLOSION_BLAST].fCameraShakeScale = 1.2 * mult;
		gExplosionBloom[EXPLOSION_BOMB].fBloomScale = 6.13 * mult;
		gExplosionBloom[EXPLOSION_BOMB].fCameraShakeScale = 1.0 * mult;

		nLastExplosionEffects = nExplosionEffects;
	}

	if (!ExplosionBloom::bInitialized && GetLiteDB()) {
		ExplosionBloom::Initialize();
		nLastExplosionEffects = -1;
	}
}

void SetPlayerList() {
	NyaHookLib::Patch<uint8_t>(0x4E48F6 + 1, nPlayerListDefaultState ? 0x85 : 0x84);
}

/*NyaDrawing::CNyaRGBA32& GetPaletteColor(int id) {
	return *(NyaDrawing::CNyaRGBA32*)&gPalette[id];
}

void SetColorPalette() {
	static int nLastColorPalette = -1;
	if (nLastColorPalette != nColorPalette) {
		if (nColorPalette == 1) {
			// upgrade bar start color
			GetPaletteColor(30).b = 238;
			GetPaletteColor(30).g = 63;
			GetPaletteColor(30).r = 191;

			// upgrade bar end color
			GetPaletteColor(31).b = 241;
			GetPaletteColor(31).g = 10;
			GetPaletteColor(31).r = 113;

			// menu yellow -> chloe collection pink
			//GetPaletteColor(33).b = 219;
			//GetPaletteColor(33).g = 100;
			//GetPaletteColor(33).r = 193;
			//GetPaletteColor(33).b = 241;
			//GetPaletteColor(33).g = 45;
			//GetPaletteColor(33).r = 193;
			//GetPaletteColor(33).b = 236;
			//GetPaletteColor(33).g = 127;
			//GetPaletteColor(33).r = 211;
			GetPaletteColor(33).b = 255;
			GetPaletteColor(33).g = 115;
			GetPaletteColor(33).r = 255;

			// menu slight yellow -> slight pink
			//GetPaletteColor(34).b = 255;
			//GetPaletteColor(34).g = 195;
			//GetPaletteColor(34).r = 241;
			GetPaletteColor(34).b = 255;
			GetPaletteColor(34).g = 201;
			GetPaletteColor(34).r = 243;
		}
		else {
			GetPaletteColor(30).b = 238;
			GetPaletteColor(30).g = 191;
			GetPaletteColor(30).r = 63;

			GetPaletteColor(31).b = 241;
			GetPaletteColor(31).g = 113;
			GetPaletteColor(31).r = 10;

			GetPaletteColor(33).b = 241;
			GetPaletteColor(33).g = 193;
			GetPaletteColor(33).r = 45;

			GetPaletteColor(34).b = 255;
			GetPaletteColor(34).g = 241;
			GetPaletteColor(34).r = 195;
		}

		nLastColorPalette = nColorPalette;
	}
}*/

bool HasLocalPlayerFinished() {
	return pGameFlow->nRaceState >= RACE_STATE_FINISHED;
}

void ApplyAutoresolvePatch(bool apply) {
	// race autoresolve
	NyaHookLib::Patch<uint8_t>(0x493411, apply ? 0xEB : 0x75);

	uint32_t aFinishedCalls[] = {
			0x4AD6C7,
			0x4AD898,
			0x4ADD99,
			0x4AE51E,
			0x4AF7D0,
			0x4AF941,
			0x4AFA08,
			0x4AFBF3,
			0x4B082D,
	};
	for (auto& addr : aFinishedCalls) {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, addr, apply ? (uintptr_t)&HasLocalPlayerFinished : 0x4ACB90);
	}
}

void SetAutoresolve() {
	bool shouldDisable = true;
	if (pGameFlow->nGameState != GAME_STATE_RACE) shouldDisable = false;
	if (pGameFlow->nDerbyType != DERBY_NONE) shouldDisable = false;
	if (pGameFlow->nGameRules != GR_DEFAULT && pGameFlow->nGameRules != GR_RACE && pGameFlow->nGameRules != GR_PONGRACE && pGameFlow->nGameRules != GR_BEAT_THE_BOMB) shouldDisable = false;
	ApplyAutoresolvePatch(shouldDisable);
}

bool IsDustEffect(int effect) {
	if (effect == FX_DUST) return true;
	if (effect == FX_DUST_AND_DEBRIS) return true;
	return false;
}

void SetCarnageRaceSmoke() {
	if (pGameFlow->nGameState != GAME_STATE_RACE) return;
	if (pLoadingScreen) return;
	if (!IsArcadeRace()) return;

	for (int i = 0; i < 64; i++) {
		auto surface = &pEnvironment->aSurfaces[i];
		if (IsDustEffect(surface->nRoadEffect1[0])) {
			surface->nRoadEffect1[0] = 0;
			surface->nRoadEffect1[1] = 0;
		}
		if (IsDustEffect(surface->nRoadEffect2[0])) {
			surface->nRoadEffect2[0] = 0;
			surface->nRoadEffect2[1] = 0;
		}
		if (IsDustEffect(surface->nBodyEffect1[0])) {
			surface->nBodyEffect1[0] = 0;
			surface->nBodyEffect1[1] = 0;
		}
	}
}

void CustomSetterThread() {
	SetSoundtrack();
	SetPlayerModel();
	SetHUDType();
	SetAIFudgeFactor();
	SetPlayerColor();
	SetSlideControl();
	SetWindowedMode();
	SetAILookahead();
	SetExplosionEffects();
	SetAutoresolve();
	SetPlayerList();
	SetGlobalFudgeFactor();
	SetCustomMapExtents();
	SetCarnageRaceSmoke();
	SetCarUnlocks();
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
	DriftCamera::ProcessCam(pCameraManager->pCamera);
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
			DoFlatOutVersionCheck(FO2Version::FOUC_GFWL);

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
			StuntMode::ApplyAirControlPatch();
			ApplySoundTweaks();
			ApplyTestHUDPatches();
			ApplyDebugMenuPatches();
			ApplyFO2SharedTexturesPatches();
			ApplyTrackExtenderPatches();
			ApplyBuoyancyPatches();
			ApplyNoNetworkPatches();
			ApplyRallyPatches();
			ApplyDDSParserPatches();
			Achievements::Init();
			*(uint32_t*)0x8494D4 = 1; // set ShowBonus to always true

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4F0F0A, &LoadMapIconsTGA);

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

			NyaHookLib::Patch(0x4D899F+1, "data/global/overlay/checkpoint2.tga");

			NyaHookLib::Patch(0x4C6B1B + 1, 16777344); // menucar model alloc size
			NyaHookLib::Patch(0x4C6B20 + 3, 16777344); // menucar model alloc size
			NyaHookLib::Patch(0x4C6B32 + 1, 16777344); // menucar skin alloc size
			NyaHookLib::Patch(0x4C6B37 + 3, 16777344); // menucar skin alloc size

			static float fZero = 0.0;
			NyaHookLib::Patch(0x44041F + 2, &fZero); // default engine smoke position

			srand(time(0));
		} break;
		default:
			break;
	}
	return TRUE;
}