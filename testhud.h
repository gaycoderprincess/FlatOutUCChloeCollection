void SetupTestTrackHUD(uintptr_t* pHud) {
	pHud[0xA7D] = (uintptr_t)LoadHUDFromDB(malloc(0x120), "Data.Overlay.HUD.TestTrack", nullptr);
}

uintptr_t SetupTestTrackHUDASM_jmp = 0x4DC266;
void __attribute__((naked)) __fastcall SetupTestTrackHUDASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (SetupTestTrackHUDASM_jmp), "i" (SetupTestTrackHUD)
	);
}

uintptr_t SetupHUDJumpTable[] = {
	0x4DC0ED,
	0x4DC0ED,
	0x4DC142,
	0x4DC186,
	0x4DC266,
	0x4DC266,
	0x4DC0ED,
	0x4DC266,
	0x4DC06B,
	0x4DC005,
	0x4DC1FC,
	(uintptr_t)&SetupTestTrackHUDASM
};

int GetTestTrackCarName(wchar_t* str, size_t len, void* a3, void* a4) {
	auto ply = GetPlayer(0);
	return mbstowcs(str, GetCarName(ply->nCarId), len);
}

int GetTestTrackTopSpeed(wchar_t* str, size_t len, void* a3, void* a4) {
	auto& bImperial = *(bool*)0x849494;

	auto ply = GetPlayerScore<PlayerScoreTest>(1);
	return _snwprintf(str, len, L"%d %s", ply->nTopSpeed, bImperial ? L"MPH" : L"KMH");
}

void TestTrackKeyword(void* a3) {
	AddHUDKeyword("TEST_CARNAME", &GetTestTrackCarName, a3);
	AddHUDKeyword("TEST_TOPSPEED", &GetTestTrackTopSpeed, a3);
}

void ApplyTestHUDPatches() {
	NyaHookLib::Patch(0x4DC001, &SetupHUDJumpTable);
	NyaHookLib::Patch<uint8_t>(0x4DBFF3 + 2, 11); // increase jmptable size

	NyaFO2Hooks::PlaceHUDKeywordHook();
	NyaFO2Hooks::aHUDKeywordFuncs.push_back(TestTrackKeyword);
}