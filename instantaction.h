uintptr_t NoInstantActionTrackIntrosASM_jmp = 0x4687C7;
uintptr_t NoInstantActionTrackIntrosASM_jmpout = 0x468802;
void __attribute__((naked)) NoInstantActionTrackIntrosASM() {
	__asm__ (
		"cmp dword ptr [ebp+0x4B0], 6\n\t"
		"jz loc_468802\n\t"
		"cmp dword ptr [ebp+0x4B0], 3\n\t"
		"jz loc_468802\n\t"
		"jmp %0\n\t"
		"loc_468802:\n\t"
		"jmp %1\n\t"
			:
			:  "m" (NoInstantActionTrackIntrosASM_jmp), "m" (NoInstantActionTrackIntrosASM_jmpout)
	);
}

uintptr_t NoInstantActionTrackIntrosASM2_jmp = 0x46F88C;
void __attribute__((naked)) NoInstantActionTrackIntrosASM2() {
	__asm__ (
		"cmp dword ptr [ecx+0x4B0], 6\n\t"
		"jz noIntro\n\t"
		"cmp dword ptr [ecx+0x4B0], 3\n\t"
		"jz noIntro\n\t"
		"jmp %0\n\t"
		"noIntro:\n\t"
		"xor al, al\n\t"
		"ret\n\t"
			:
			:  "m" (NoInstantActionTrackIntrosASM2_jmp)
	);
}

void RandomizeInstantActionSkin() {
	pGame->nInstantActionCarSkin = (rand() % GetNumSkinsForCar(pGame->nInstantActionCar)) + 1;
}

uintptr_t InstantActionSkinRandomizerASM_jmp = 0x469295;
void __attribute__((naked)) InstantActionSkinRandomizerASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"

		"push ebx\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"mov edi, eax\n\t"
		"jmp %0\n\t"
			:
			:  "m" (InstantActionSkinRandomizerASM_jmp), "i" (RandomizeInstantActionSkin)
	);
}

void ApplyInstantActionPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4687BE, &NoInstantActionTrackIntrosASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x46F880, &NoInstantActionTrackIntrosASM2);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x469290, &InstantActionSkinRandomizerASM);
}