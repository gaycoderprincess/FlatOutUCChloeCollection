uintptr_t SetSharedTextureFolder_call = 0x5A6E90;
void __attribute__((naked)) __fastcall SetSharedTextureFolderASM(const char* path) {
	__asm__ (
		"mov eax, ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (SetSharedTextureFolder_call)
	);
}

uintptr_t SetTextureFolder_call = 0x5A6E20;
void __attribute__((naked)) __fastcall SetTextureFolderASM(const char* path) {
	__asm__ (
		"mov eax, ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (SetTextureFolder_call)
	);
}

void HUDTexturePathHook() {
	if (nHUDType == 1) {
		SetTextureFolderASM("data/global/overlay/fo2hud");
	}
	else SetTextureFolderASM("data/global/overlay");
	SetSharedTextureFolderASM("data/global/overlay");
}

auto LoadHUDTexturesHooked_call = (void*(__stdcall*)(void*, const char*, const char*, void*, int, int, float))0x4DBA90;
void* __stdcall LoadHUDTexturesHooked(void* a1, const char* a2, const char* a3, void* a4, int a5, int a6, float a7) {
	if (nHUDType == 1) {
		a2 = "data/global/overlay/fo2hud";
		a7 = 1.5;
	}
	return LoadHUDTexturesHooked_call(a1, a2, a3, a4, a5, a6, a7);
}

void SetHUDType() {
	static int nLastHUDType = -1;
	if (nLastHUDType != nHUDType) {
		static const char* baseHudDBPath = "Settings.HUD";
		static const char* baseHudGearFontPath = "Title";
		static const char* fo2HudDBPath = "Settings.HUDFO2";
		static const char* fo2HudGearFontPath = "FontLarge";

		bool bFO2Hud = nHUDType == 1;
		NyaHookLib::Patch(0x4DDA2A + 1, bFO2Hud ? fo2HudDBPath : baseHudDBPath);
		NyaHookLib::Patch(0x4E54CF + 1, bFO2Hud ? fo2HudGearFontPath : baseHudGearFontPath);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DEB01, &LoadHUDTexturesHooked);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DEB2D, &LoadHUDTexturesHooked);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DEB55, &LoadHUDTexturesHooked);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DEB7E, &LoadHUDTexturesHooked);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4DEBA7, &LoadHUDTexturesHooked);
		*(uint32_t*)0x846568 = 0; // reload the hud db when switched
		nLastHUDType = nHUDType;
	}
}

void ApplyCustomHUDPatches() {
	// speedo needle read at 004E537B, read from file at 4DE2E2
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x48929D, &HUDTexturePathHook);
}