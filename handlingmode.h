uintptr_t NoSlideControlASM_jmp = 0x42B4AE;
void __attribute__((naked)) __fastcall NoSlideControlASM() {
	__asm__ (
		"push ebp\n\t"
		"mov ebp, esp\n\t"
		"and esp, 0xFFFFFFF8\n\t"
		"sub esp, 0x98\n\t"
		"fldz\n\t"
		"jmp %0\n\t"
			:
			: "m" (NoSlideControlASM_jmp)
	);
}

void SetSlideControl(bool disabled) {
	if (disabled) {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42F9CE, &NoSlideControlASM);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480D2C, &NoSlideControlASM);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x51460E, &NoSlideControlASM);

		// disable slidecontrol stuff in the fouc code
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42B6B6, 0x42BCF7);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42C02C, 0x42C26D);
	}
	else {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42F9CE, 0x42B4A0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480D2C, 0x42B4A0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x51460E, 0x42B4A0);
	}
}

void SetBetaHandling(bool enabled) {
	NyaHookLib::Patch(0x45CC35 + 1, enabled ? "BetaBody" : "Body");
	NyaHookLib::Patch(0x45DA4F + 1, enabled ? "BetaEngine" : "Engine");
	//NyaHookLib::Patch(0x423CCA + 1, enabled ? "BetaEngineSound" : "EngineSound");
	NyaHookLib::Patch(0x45CF8D + 1, enabled ? "BetaGearbox" : "Gearbox");
	NyaHookLib::Patch(0x45D200 + 1, enabled ? "BetaSuspension" : "Suspension");
	NyaHookLib::Patch(0x45D7B6 + 1, enabled ? "BetaTires" : "Tires");
}

int nMultiplayerHandlingMode = 0;
void SetSlideControl() {
	static int nLastHandling = -1;
	static bool bLastDriftEvent = false;

	int nCurrentHandling = nHandlingMode;
	if (bIsInMultiplayer) nCurrentHandling = nMultiplayerHandlingMode;
	if (nLastHandling != nCurrentHandling/* || bLastDriftEvent != bIsDriftEvent*/) {
		SetSlideControl(nCurrentHandling == 1/* || bIsDriftEvent*/);
		SetBetaHandling(nCurrentHandling == 2);
		nLastHandling = nCurrentHandling;
		bLastDriftEvent = bIsDriftEvent;
	}
}