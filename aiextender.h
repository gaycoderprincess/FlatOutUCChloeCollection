int nForceAllAICarsNextRace = -1;
int nForceAllAICarsNextRaceDuo[2] = {-1, -1};
int nForceAICountNextRace = -1;

int GetOpponentCount() {
	int count = 11;
	if (pGameFlow->PreRace.nMode == GM_ARCADE_CAREER) {
		switch (nOpponentCountTypeArcade) {
			case 0:
				count = 11;
				break;
			case 1:
				count = nNumAIProfiles;
				break;
			default:
				break;
		}
	}
	else if (pGameFlow->PreRace.nMode == GM_CAREER) {
		switch (nOpponentCountTypeCareer) {
			case 0:
				count = 7;
				break;
			case 1:
				count = 11;
				break;
			default:
				break;
		}
	}
	// single events
	else {
		switch (nOpponentCountTypeSingleEvent) {
			case 0:
				count = 7;
				break;
			case 1:
				count = 11;
				break;
			case 2:
				count = nNumAIProfiles;
				break;
			case 3:
				count = 31;
				break;
			default:
				break;
		}
	}
	return count;
}

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
	if (count >= 7) {
		count = GetOpponentCount();

		// set arcade opponent count to either 11 or 12 for balance
		if (pGameFlow->PreRace.nMode == GM_ARCADE_CAREER) {
			if (count <= 11) count = 11;
			else count = 12;
		}

		if (bIsStuntMode) count = 0;
		if (pGameFlow->nGameRulesIngame == GR_STUNT) count = 7;
		if (nForceAICountNextRace >= 0) {
			count = nForceAICountNextRace;
			nForceAICountNextRace = -1;
		}
		if (pGameFlow->PreRace.nMode == GM_CAREER && count > 11) {
			count = 11;
		}
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
	if (GetOpponentCount() > 7 || DoesTrackValueExist(pGameFlow->PreRace.nLevel, "AlwaysUseStartingGhost")) {
		auto eventData = tEventData(EVENT_PLAYER_RESPAWN_GHOST);
		eventData.data[3] = 500;
		pPlayer->TriggerEvent(&eventData);
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

void __fastcall SetAICarDataID(Player* pPlayer) {
	pPlayer->nCarId = GetCarDBID(pPlayer->nAICarNum);
}

uintptr_t AICarDataIDsASM_jmp = 0x409308;
void __attribute__((naked)) __fastcall AICarDataIDsASM() {
	__asm__ (
		"mov [esi+0x298], edx\n\t"
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (AICarDataIDsASM_jmp), "i" (SetAICarDataID)
	);
}

bool bRayAltProfileState = false;
bool bForceRayAltProfile = false;
void __fastcall SetCustomAIProfiles(Player* pPlayer) {
	NyaHookLib::Patch(0x40953D + 1, "%s.Class[%d]");
	NyaHookLib::Patch(0x409400 + 1, "%s[%d].Class[%d]");

	int id = pPlayer->nAIId - 1;
	auto table = GetLiteDB()->GetTable("FlatOut2.Profiles");
	auto prerace = GetLiteDB()->GetTable("GameFlow.PreRace");
	auto profileNode = table->GetPropertyAsNode("Profile", id);
	auto nodeName = profileNode->GetName();
	if (!strcmp(nodeName, "AI6_RaySmith")) {
		bRayAltProfileState = false;
		int chance = 10;
		if (pGameFlow->PreRace.nMode == GM_CAREER) {
			if (prerace->GetPropertyAsInt("Class", 0) == 2 && prerace->GetPropertyAsInt("Cup", 0) == 9) {
				chance = 100; // 100% chance in race finals
			}
			if (prerace->GetPropertyAsInt("Class", 0) == 3 && prerace->GetPropertyAsInt("Cup", 0) == 10) {
				chance = 100; // 100% chance in street finals
			}
			if (prerace->GetPropertyAsInt("Class", 0) == 4) {
				chance = 100; // 100% chance in grand finals
			}
		}
		if ((pPlayer->nAIClassId == 1 || pPlayer->nAIClassId == 2) && (bForceRayAltProfile || rand() % 100 < chance)) {
			bRayAltProfileState = true;
			NyaHookLib::Patch(0x40953D + 1, "%s.Class[%d]Alt");
			NyaHookLib::Patch(0x409400 + 1, "%s[%d].Class[%d]Alt");
		}
	}
}

uintptr_t CustomAIProfilesASM_jmp = 0x409457;
void __attribute__((naked)) __fastcall CustomAIProfilesASM() {
	__asm__ (
		"fst dword ptr [esi+0x97C]\n\t"
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (CustomAIProfilesASM_jmp), "i" (SetCustomAIProfiles)
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

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x409302, &AICarDataIDsASM);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x409451, &CustomAIProfilesASM);
}