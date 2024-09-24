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

int GetCarDBID(int dataId) {
	return newCarMatchupArrayDataToDB[dataId];
}

void ApplyCarLimitAdjuster() {
	newCarMatchupArrayDataToDB = new int[8192];
	newCarMatchupArrayDBToData = new int[8192]; // +512
	NyaHookLib::Patch(0x4C7EB9, &newCarMatchupArrayDataToDB);
	NyaHookLib::Patch(0x47FB8E, &newCarMatchupArrayDBToData);
	NyaHookLib::Patch(0x55B713, &newCarMatchupArrayDBToData);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x45ED17, &CarMatchupSwapASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x45ECDB, &CarMatchupSwap2ASM);
}