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

void ApplySetupSkipPatches() {
	// skip setup window if already configured
	if (std::filesystem::exists("Savegame/device.cfg")) {
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x457248, &SkipSetupASM);
	}
	else bSetupDialogRan = true;
}