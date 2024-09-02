int nAIUpgradeCarTargetReturn = -1;
void __fastcall SetAIUpgradeCarTarget(int carId) {
	nAIUpgradeCarTargetReturn = carId;

	// venom for derby cars
	if (carId == 67) nAIUpgradeCarTargetReturn = 8; // neville's blocker
	if (carId < 8) nAIUpgradeCarTargetReturn = 8;

	// insetta for race cars
	if (carId >= 9 && carId < 19) nAIUpgradeCarTargetReturn = 19;

	// road king for street cars
	if (carId >= 20 && carId < 33) nAIUpgradeCarTargetReturn = 32;

	// 0 for chili
	// 45 46 for bonecracker grinder
	// 67 for blocker
}

uintptr_t AIUpgradesCareerASM_jmp = 0x469E84;
void __attribute__((naked)) __fastcall AIUpgradesCareerASM() {
	__asm__ (
		"mov edx, [ebx+0x298]\n\t"
		"pushad\n\t"
		"mov ecx, edx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov edx, %2\n\t"
		"fstp dword ptr [esp+0x10]\n\t"
		"fld dword ptr [esp+0x10]\n\t"
		"jmp %0\n\t"
			:
			: "m" (AIUpgradesCareerASM_jmp), "i" (SetAIUpgradeCarTarget), "m" (nAIUpgradeCarTargetReturn)
	);
}

uintptr_t AIUpgradesSingleRaceASM_jmp = 0x469E08;
void __attribute__((naked)) __fastcall AIUpgradesSingleRaceASM() {
	__asm__ (
		"mov eax, [ebx+0x298]\n\t"
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov eax, %2\n\t"
		"push ebp\n\t"
		"push ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (AIUpgradesSingleRaceASM_jmp), "i" (SetAIUpgradeCarTarget), "m" (nAIUpgradeCarTargetReturn)
	);
}

void ApplyAIUpgradesPatches() {
	// upgrades array read at:
	// 00472E0E
	// 0047200E
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x469E76, &AIUpgradesCareerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x469E00, &AIUpgradesSingleRaceASM);
}