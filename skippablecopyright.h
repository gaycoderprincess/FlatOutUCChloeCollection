bool bIsInMultiplayer = false;

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

void CopyrightHooked() {
	LoadingScreen::Create();
	while (!GetCopyrightSkipButton()) {
		LoadingScreen::nCopyrightTimer = GetTickCount() + 4000;
		pLoadingScreen->Draw();
		Sleep(10);
	}
	LoadingScreen::nCopyrightTimer = GetTickCount();
	LoadingScreen::Free();
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

bool bLoadingScreenPressEnter = false;
void OnLoadInGame() {
	if (bIsInMultiplayer || nLoadingSkip) return;

	bLoadingScreenPressEnter = true;
	while (!GetCopyrightSkipButton()) {
		pLoadingScreen->Draw();
	}

	// a bit of sleep for leeway for the return button
	for (int i = 0; i < 100 / 10; i++) {
		pLoadingScreen->Draw();
		Sleep(10);
	}
}

void OnLoadMenu() {
	// skip the first load in the copyright screen
	if (pGame->nGameState == GAME_STATE_NONE) return;

	// skip the first load into the main menu
	static bool bInitialLoad = true;
	if (bInitialLoad) {
		bInitialLoad = false;
		return;
	}
	OnLoadInGame();
}

void __fastcall LoadingScreenPressEnter(int* pElement) {
	if (pElement[6] == LOADINGSCREEN_LOADING && bLoadingScreenPressEnter) {
		pElement[6] = PRESS_START;
		bLoadingScreenPressEnter = false;
	}
}

// null check the loading assets ptr list
uintptr_t LoadingScreenPressEnterASM_jmp = 0x598A5F;
int __attribute__((naked)) LoadingScreenPressEnterASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"test ah, 0x44\n\t"
		"mov eax, [edi+0x18]\n\t"
		"jmp %0\n\t"
			:
			:  "m" (LoadingScreenPressEnterASM_jmp), "i" (LoadingScreenPressEnter)
	);
}

void ApplySkippableCopyrightPatches() {
	NyaHookLib::Patch<uint8_t>(0x55B97C, 0xEB); // always use copyright.tga
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55BA6F, &CopyrightHookedASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5646A3, &FreeLoadingScreenHookedASM);

	NyaFO2Hooks::PlaceOnLoadInGameHook();
	NyaFO2Hooks::aOnLoadInGameFuncs.push_back(OnLoadInGame);
	NyaFO2Hooks::PlaceOnLoadMenuHook();
	NyaFO2Hooks::aOnLoadMenuFuncs.push_back(OnLoadMenu);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x598A59, &LoadingScreenPressEnterASM);
}