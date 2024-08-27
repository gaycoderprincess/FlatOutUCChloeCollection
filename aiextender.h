uintptr_t MoreAIProfilesASM_jmp = 0x4698D3;
float __attribute__((naked)) MoreAIProfilesASM() {
	__asm__ (
		"mov ecx, %1\n\t"
		"idiv ecx\n\t"
		"mov ecx, 0xB\n\t"
		"mov ebx, edx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (MoreAIProfilesASM_jmp), "m" (nNumAIProfiles)
	);
}

auto InitAIHooked_call = (void(__stdcall*)(void*, int))0x4693F0;
void __stdcall InitAIHooked(void* a1, int count) {
	count = GetOpponentCount();
	if (pGame->nGameRules == GR_STUNT) count = 7;
	if (nForceAICountNextRace >= 0) {
		count = nForceAICountNextRace;
		nForceAICountNextRace = -1;
	}
	if (pGame->nGameMode == GM_CAREER && count > 11) {
		count = 11;
	}
	return InitAIHooked_call(a1, count);
}

const wchar_t* __fastcall GetAIName(int id) {
	int nNumNames = nNumAIProfiles;
	const wchar_t* aNames[32] = {
			L"JACK BENTON",
			L"KATIE JACKSON",
			L"SOFIA MARTINEZ",
			L"SALLY TAYLOR",
			L"JASON WALKER",
			L"RAY CARTER",
			L"FRANK MALCOV",
			L"CURTIS WOLFE",
			L"LEWIS DURAN",
			L"LEI BING",
			L"JILL RICHARDS",
			L"NEVILLE"
	};
	static bool bOnce = true;
	static wchar_t** aCustomNames = nullptr;
	if (bOnce) {
		aCustomNames = new wchar_t*[nNumNames];
		memset(aCustomNames, 0, sizeof(wchar_t*)*nNumNames);
		bOnce = false;
	}

	auto nameId = id % nNumNames;
	if (aCustomNames[nameId]) return aCustomNames[nameId];

	auto config = toml::parse_file("Config/AIConfig.toml");
	auto str = (std::string)config["main"]["AI" + std::to_string(nameId + 1)].value_or("*NULL*");
	if (str != "*NULL*") {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		auto retStr = converter.from_bytes(str);
		auto wchar = new wchar_t[retStr.length()+1];
		wcscpy_s(wchar, retStr.length()+1, retStr.c_str());
		aCustomNames[nameId] = wchar;
		return aCustomNames[nameId];
	}
	return aNames[nameId];
}

void __attribute__((naked)) __fastcall GetAINameASM() {
	__asm__ (
		"mov ecx, eax\n\t"
		"jmp %0\n\t"
			:
			: "i" (GetAIName)
	);
}

void __fastcall GhostForMoreOpponents(Player* pPlayer) {
	if (GetOpponentCount() > 11) {
		int eventProperties[] = {6040, 0, 0, 0, 500};
		pPlayer->TriggerEvent(eventProperties);
	}
}

uintptr_t GhostForMoreOpponentsASM_jmp = 0x42884D;
void __attribute__((naked)) __fastcall GhostForMoreOpponentsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, [ebp+0x469C]\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov edi, [ebp+0x1C5C]\n\t"
		"cmp edi, [ebp+0x1C60]\n\t"
		"jmp %0\n\t"
			:
			: "m" (GhostForMoreOpponentsASM_jmp), "i" (GhostForMoreOpponents)
	);
}

void ApplyAIExtenderPatches() {
	auto config = toml::parse_file("Config/AIConfig.toml");
	nNumAIProfiles = config["main"]["NumAIProfiles"].value_or(11);
	if (nNumAIProfiles > 31) nNumAIProfiles = 31;
	NyaHookLib::Patch<uint8_t>(0x47FC8C + 1, nNumAIProfiles + 1);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4698CA, &MoreAIProfilesASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469459, &GetAINameASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x468291, &InitAIHooked);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4682A2, &InitAIHooked);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4682BA, &InitAIHooked);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x46A2B9, &InitAIHooked);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x428841, &GhostForMoreOpponentsASM);
}