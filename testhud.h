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

uintptr_t UltrawideJumpTable[] = {
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

void GetTestTrackCarName(wchar_t* str, size_t len) {
	auto ply = GetPlayer(0);
	mbstowcs(str, GetCarName(ply->nCarId), len);
}

void GetTestTrackTopSpeed(wchar_t* str, size_t len) {
	auto& bImperial = *(bool*)0x849494;

	auto ply = GetPlayerScore<PlayerScoreTest>(1);
	_snwprintf(str, len, L"%d %s", ply->nTopSpeed, bImperial ? L"MPH" : L"KMH");
}

void __fastcall TestTrackKeyword(void* a3) {
	AddHUDKeyword("TEST_CARNAME", (void*)&GetTestTrackCarName, a3);
	AddHUDKeyword("TEST_TOPSPEED", (void*)&GetTestTrackTopSpeed, a3);
}

uintptr_t TestTrackKeywordASM_jmp = 0x4ECB20;
float __attribute__((naked)) TestTrackKeywordASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (TestTrackKeywordASM_jmp), "i" (TestTrackKeyword)
	);
}

void ApplyTestHUDPatches() {
	NyaHookLib::Patch(0x4DC001, &UltrawideJumpTable);
	NyaHookLib::Patch<uint8_t>(0x4DBFF3 + 2, 11); // increase jmptable size
	TestTrackKeywordASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4EA8C7, &TestTrackKeywordASM);
}