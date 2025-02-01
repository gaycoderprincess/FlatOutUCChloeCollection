void __fastcall SetAICarToForcedCar(Player* player) {
	if (nForceAllAICarsNextRace >= 0) player->nCarId = nForceAllAICarsNextRace;
	if (nForceAllAICarsNextRaceDuo[0] >= 0 && nForceAllAICarsNextRaceDuo[1] >= 0) {
		static int skinId[2] = {0,0};

		static bool tmp = false;
		player->nCarId = nForceAllAICarsNextRaceDuo[tmp];
		player->nCarSkinId = (skinId[tmp] % GetNumSkinsForCar(player->nCarId)) + 1;
		skinId[tmp]++;
		tmp = !tmp;
	}
}

uintptr_t ForceAllAICarsASM_jmp = 0x46969A;
void __attribute__((naked)) __fastcall ForceAllAICarsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"sub eax, esi\n\t"
		"sar eax, 1\n\t"
		"push eax\n\t"
		"push edx\n\t"
		"lea esi, [ebx+0x324]\n\t"
		"jmp %0\n\t"
			:
			: "m" (ForceAllAICarsASM_jmp), "i" (SetAICarToForcedCar)
	);
}

void ResetAICarForce() {
	nForceAllAICarsNextRace = -1;
	nForceAllAICarsNextRaceDuo[0] = -1;
	nForceAllAICarsNextRaceDuo[1] = -1;
	NyaHookLib::Patch<uint8_t>(0x43407E, 0x75);
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0x75);
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0x75);
}

void __attribute__((naked)) __fastcall ForceAllAICarsResetASM() {
	__asm__ (
		"pushad\n\t"
		"call %0\n\t"
		"popad\n\t"
		"pop ebx\n\t"
		"add esp, 0x80\n\t"
		"ret 4\n\t"
			:
			: "i" (ResetAICarForce)
	);
}

void ApplyAILUAHacks() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x46968E, &ForceAllAICarsASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47FD14, &ForceAllAICarsResetASM);
}