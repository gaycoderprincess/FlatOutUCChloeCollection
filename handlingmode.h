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

void SetSlideControl() {
	static int nLastSlideControl = -1;
	if (nLastSlideControl != nSlideControlDisabled) {
		SetSlideControl(nSlideControlDisabled);
		nLastSlideControl = nSlideControlDisabled;
	}
}