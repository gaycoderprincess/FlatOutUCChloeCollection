bool GetCopyrightSkipButton() {
	if ((GetAsyncKeyState(VK_RETURN) & 0x8000) != 0) return true;
	for (int i = 0; i < XUSER_MAX_COUNT; i++) {
		XINPUT_STATE state;
		memset(&state, 0, sizeof(state));

		if (XInputGetState_Dynamic(i, &state) == ERROR_SUCCESS) {
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0) return true;
			if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0) return true;
		}
	}
	return false;
}

uintptr_t DrawLoadingScreen_call = 0x5647E0;
int __attribute__((naked)) __fastcall DrawLoadingScreen(void* a1) {
	__asm__ (
		"pushad\n\t"
		"mov esi, ecx\n\t"
		"call %0\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (DrawLoadingScreen_call)
	);
}

auto CreateLoadingScreen = (void(*)())0x46F8D0;
auto FreeLoadingScreen = (void(*)())0x46F920;
void CopyrightHooked() {
	CreateLoadingScreen();
	while (!GetCopyrightSkipButton()) {
		*(int*)0x8465F4 = GetTickCount() + 4000;
		DrawLoadingScreen(pLoadingScreen);
		Sleep(10);
	}
	*(int*)0x8465F4 = GetTickCount();
	FreeLoadingScreen();
}

uintptr_t CopyrightHookedASM_call = 0x41FF10;
uintptr_t CopyrightHookedASM_jmp = 0x55BA74;
int __attribute__((naked)) CopyrightHookedASM() {
	__asm__ (
		"call %1\n\t"
		"pushad\n\t"
		"call %2\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (CopyrightHookedASM_jmp), "m" (CopyrightHookedASM_call), "i" (CopyrightHooked)
	);
}

// null check the loading assets ptr list
uintptr_t FreeLoadingScreenHookedASM_jmp = 0x5646AC;
int __attribute__((naked)) FreeLoadingScreenHookedASM() {
	__asm__ (
		"mov eax, [esi+0x2FC]\n"
		"test eax, eax\n"
		"jz freeLoadingScreen_jmpOut\n"
		"mov ecx, [eax+4]\n\t"
		"jmp %0\n\t"

		"freeLoadingScreen_jmpOut:\n\t"
		"pop edi\n\t"
		"pop ebx\n\t"
		"ret\n\t"
			:
			:  "m" (FreeLoadingScreenHookedASM_jmp)
	);
}

void ApplySkippableCopyrightPatches() {
	NyaHookLib::Patch<uint8_t>(0x55B97C, 0xEB); // always use copyright.tga
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55BA6F, &CopyrightHookedASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5646A3, &FreeLoadingScreenHookedASM);
}