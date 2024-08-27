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

void CustomSetterThread() {
	while (true) {
		SetSoundtrack();
		SetPlayerModel();
		SetHUDType();
		SetAIFudgeFactor();
		Sleep(500);
	}
}

auto LoadMapIconsTGA_call = (void*(__stdcall*)(void*, const char*, int, int))0x5A6F00;
void* __stdcall LoadMapIconsTGA(void* a1, const char* a2, int a3, int a4) {
	SetTextureFolderASM("data/global/overlay/");
	auto ret = LoadMapIconsTGA_call(a1, a2, a3, a4);
	SetTextureFolderASM("data/global/map/");
	return ret;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x24CEF7) {
				MessageBoxA(nullptr, aFOUCVersionFail, "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

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

			std::thread(CustomSetterThread).detach();
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
		} break;
		default:
			break;
	}
	return TRUE;
}