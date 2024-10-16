uintptr_t CarMatchupSwapASM_call = 0x45AFB0;
uintptr_t CarMatchupSwapASM_jmp = 0x45ED1D;
void __attribute__((naked)) __fastcall CarMatchupSwapASM() {
	__asm__ (
		"mov ebp, 0x9298FC8\n\t"
		"mov ebp, [ebp]\n\t"
		"push ebp\n\t"
		"call %1\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarMatchupSwapASM_jmp), "m" (CarMatchupSwapASM_call)
	);
}

int* newCarMatchupArrayDataToDB = nullptr;
int* newCarMatchupArrayDBToData = nullptr;
uintptr_t CarMatchupSwap2ASM_jmp = 0x45ECE2;
void __attribute__((naked)) __fastcall CarMatchupSwap2ASM() {
	__asm__ (
		"mov ecx, %1\n"
		"mov [ecx+eax*4], edi\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarMatchupSwap2ASM_jmp), "m" (newCarMatchupArrayDataToDB)
	);
}

int GetCarDataID(int dbId) {
	return newCarMatchupArrayDBToData[dbId+128];
}

int* newCarUpgradeArray = nullptr;
uintptr_t CarUpgradesASM1_jmp = 0x471B9F;
void __attribute__((naked)) __fastcall CarUpgradesASM1() {
	__asm__ (
		"mov ebp, %1\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM1_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM2_jmp = 0x470CA1;
void __attribute__((naked)) __fastcall CarUpgradesASM2() {
	__asm__ (
		"mov ebx, %1\n"
		"add eax, ebx\n"
		"shl esi, 4\n\t"
		"mov dl, [esi+eax]\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM2_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM3_jmp = 0x472015;
void __attribute__((naked)) __fastcall CarUpgradesASM3() {
	__asm__ (
		"push esi\n"
		"mov esi, %1\n"
		"add eax, esi\n"
		"pop esi\n"
		"test [ebx+eax], dl\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM3_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM4_jmp = 0x47208F;
void __attribute__((naked)) __fastcall CarUpgradesASM4() {
	__asm__ (
		"push esi\n"
		"mov esi, %1\n"
		"add eax, esi\n"
		"pop esi\n"
		"test [ebx+eax], dl\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM4_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM5_jmp = 0x472106;
void __attribute__((naked)) __fastcall CarUpgradesASM5() {
	__asm__ (
		"push esi\n"
		"mov esi, %1\n"
		"add eax, esi\n"
		"pop esi\n"
		"test [ebx+eax], dl\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM5_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM6_jmp = 0x47217F;
void __attribute__((naked)) __fastcall CarUpgradesASM6() {
	__asm__ (
		"push esi\n"
		"mov esi, %1\n"
		"add eax, esi\n"
		"pop esi\n"
		"test [ebx+eax], dl\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM6_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM7_jmp = 0x472C15;
void __attribute__((naked)) __fastcall CarUpgradesASM7() {
	__asm__ (
		"push esi\n"
		"mov esi, %1\n"
		"mov cl, [esi+ebp]\n\t"
		"pop esi\n"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM7_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM8_jmp = 0x472E15;
void __attribute__((naked)) __fastcall CarUpgradesASM8() {
	__asm__ (
		"push edx\n"
		"mov edx, %1\n"
		"mov cl, [ecx+edx]\n\t"
		"pop edx\n"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM8_jmp), "m" (newCarUpgradeArray)
	);
}

uintptr_t CarUpgradesASM9_jmp = 0x4A6FBB;
void __attribute__((naked)) __fastcall CarUpgradesASM9() {
	__asm__ (
		"push edi\n"
		"mov edi, %1\n"
		"mov cl, [ecx+edi]\n\t"
		"pop edi\n"
		"jmp %0\n\t"
			:
			: "m" (CarUpgradesASM9_jmp), "m" (newCarUpgradeArray)
	);
}

void ApplyCarLimitAdjuster() {
	newCarMatchupArrayDataToDB = new int[8192];
	newCarMatchupArrayDBToData = new int[8192]; // +512
	newCarUpgradeArray = new int[8192];
	memset(newCarMatchupArrayDataToDB, 0, sizeof(int)*8192);
	memset(newCarMatchupArrayDBToData, 0, sizeof(int)*8192);
	NyaHookLib::Patch(0x4C7EB9, &newCarMatchupArrayDataToDB);
	NyaHookLib::Patch(0x47FB8E, &newCarMatchupArrayDBToData);
	NyaHookLib::Patch(0x55B713, &newCarMatchupArrayDBToData);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x45ED17, &CarMatchupSwapASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x45ECDB, &CarMatchupSwap2ASM);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x471B99, &CarUpgradesASM1); // write

	// readers
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x470C95, &CarUpgradesASM2);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47200C, &CarUpgradesASM3);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x472086, &CarUpgradesASM4);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4720FD, &CarUpgradesASM5);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x472176, &CarUpgradesASM6);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x472C0E, &CarUpgradesASM7);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x472E0E, &CarUpgradesASM8);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A6FB4, &CarUpgradesASM9);
}