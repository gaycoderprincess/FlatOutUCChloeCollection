void SetArcadeRaceMultiplierPointer(float* values) {
	uintptr_t addresses[] = {
			0x46F590,
			0x46F793,
			0x4E9DD5,
	};
	for (auto& addr : addresses) {
		NyaHookLib::Patch(addr, values);
	}
}

void AddFragDerbyScore(int amount) {
	//if (!nFragDerbyCrashRewards) return;
	if (amount <= 0) return;
	if (pGame->nGameRules != GR_DEFAULT && pGame->nGameRules != GR_DERBY) return;
	if (pGame->nDerbyType != DERBY_FRAG) return;

	auto score = GetPlayerScore<PlayerScoreDerby>(1);
	if (!score) return;

	if (pScoreManager->nSurvivorId == score->nPlayerId) amount *= 2;
	score->nScore1 += amount * score->fScoreMultiplier;

	auto eventData = tEventData(EVENT_PLAYER_UPDATE_FRAGDERBY_SCORE, score->nPlayerId + 1);
	eventData.data[3] = amount;
	pEventManager->PostEvent(&eventData);
}

void __fastcall MoreFragDerbyRewards(uint32_t a1) {
	switch (a1) {
		case INGAME_CRASHBONUS_SUPERFLIP:
			AddFragDerbyScore(nFragDerbyRewardSuperFlip);
			break;
		case INGAME_CRASHBONUS_SLAM:
			AddFragDerbyScore(nFragDerbyRewardSlam);
			break;
		case INGAME_CRASHBONUS_POWERHIT:
			AddFragDerbyScore(nFragDerbyRewardPowerHit);
			break;
		case INGAME_CRASHBONUS_BLASTOUT:
			AddFragDerbyScore(nFragDerbyRewardBlastOut);
			break;
		default:
			break;
	}
}

uintptr_t MoreFragDerbyRewardsASM_jmp = 0x459DF0;
float __attribute__((naked)) MoreFragDerbyRewardsASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (MoreFragDerbyRewardsASM_jmp), "i" (MoreFragDerbyRewards)
	);
}

void ReadFragDerbyRewardConfig() {
	auto config = toml::parse_file("Config/FragDerbyRewards.toml");
	nFragDerbyRewardSlam = config["main"]["Slam"].value_or(nFragDerbyRewardSlam);
	nFragDerbyRewardSuperFlip = config["main"]["SuperFlip"].value_or(nFragDerbyRewardSuperFlip);
	nFragDerbyRewardPowerHit = config["main"]["PowerHit"].value_or(nFragDerbyRewardPowerHit);
	nFragDerbyRewardBlastOut = config["main"]["BlastOut"].value_or(nFragDerbyRewardBlastOut);
}

const char* sPopupName = "PlatinumEarned";
uintptr_t CreatePopup_call = 0x4EA770;
float __attribute__((naked)) __fastcall CreatePopup(void* a2, void* a3) {
	__asm__ (
		"pushad\n\t"
		"mov eax, %1\n\t" // popup name in eax
		"mov esi, ecx\n\t" // a2 in esi
		"push edx\n\t" // a3 pushed
		"call %0\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (CreatePopup_call), "m" (sPopupName)
	);
}

HUDElement* pArcadePlatinumImage = nullptr;
HUDElement* pArcadeGoldImage = nullptr;

bool bArcadePlatinumEnabled = false;
int nArcadePlatinumCurrentLevelX = 0;
int nArcadePlatinumCurrentLevelY = 0;
bool bAchievedPlatinumThisRace = false;
void __stdcall ArcadePlatinums(void* a3, void** a1, int numPoints) {
	if (!bArcadePlatinumEnabled || pGame->nGameMode != GM_ARCADE_CAREER) {
		if (pArcadePlatinumImage) pArcadePlatinumImage->bVisible = false;
		return;
	}

	auto silverTarget = pGame->nArcadeTargets[1];
	auto goldTarget = pGame->nArcadeTargets[0];
	auto platTarget = nArcadePlatinumTargets[nArcadePlatinumCurrentLevelX][nArcadePlatinumCurrentLevelY];
	if (platTarget <= 0) return;

	if (pArcadeGoldImage) {
		pArcadeGoldImage->bVisible = numPoints >= silverTarget;
		if (pArcadePlatinumImage) {
			pArcadePlatinumImage->bVisible = false;
			if (numPoints >= goldTarget) {
				pArcadeGoldImage->bVisible = false;
				pArcadePlatinumImage->bVisible = true;
			}
		}
	}

	// reset platinum status if we restarted
	if (numPoints <= 0) bAchievedPlatinumThisRace = false;
	// otherwise check for platinum score
	else if (numPoints >= platTarget && !bAchievedPlatinumThisRace) {
		CreatePopup(a1[2686], a3);
		auto data = tEventData(EVENT_SFX_ARCADE_AWARD);
		pEventManager->SendEvent(&data);
		bAchievedPlatinumThisRace = true;
	}
}

uintptr_t ArcadePlatinumsASM_jmp = 0x4EA194;
float __attribute__((naked)) ArcadePlatinumsASM() {
	__asm__ (
		"push ecx\n\t"
		"mov ecx, [esp+0x40]\n\t"
		"pushad\n\t"
		"push esi\n\t" // numPoints
		"push edi\n\t" // a1
		"push ecx\n\t" // a3
		"call %1\n\t"
		"popad\n\t"
		"pop ecx\n\t"
		"cmp [edi+0x2A08], bl\n"
		"mov [esp+0xB], bl\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ArcadePlatinumsASM_jmp), "i" (ArcadePlatinums)
	);
}

void FormatPoints(int a1, wchar_t* str, size_t len) {
	if (a1 < 1000) _snwprintf(str, len, L"%d", a1);

	if (a1 >= 1000000) {
		_snwprintf(str, len, L"%d,%03d,%03d", a1 / 1000000, (a1 / 1000) % 1000, a1 % 1000);
	}
	else {
		_snwprintf(str, len, L"%d,%03d", a1 / 1000, a1 % 1000);
	}
}

void DrawArcadePlatinums(wchar_t* str, size_t len) {
	auto target = nArcadePlatinumTargets[nArcadePlatinumCurrentLevelX][nArcadePlatinumCurrentLevelY];
	FormatPoints(target, str, len);
}

void __fastcall ArcadePlatinumKeyword(void* a3) {
	AddHUDKeyword("PLATINUM", (void*)&DrawArcadePlatinums, a3);
}

uintptr_t ArcadePlatinumKeywordASM_jmp = 0x4ECB20;
float __attribute__((naked)) ArcadePlatinumKeywordASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ArcadePlatinumKeywordASM_jmp), "i" (ArcadePlatinumKeyword)
	);
}

auto GetArcadePlatinumTexture_call = (HUDElement*(__thiscall*)(HUDElement*, const char*))0x4ECA60;
HUDElement* __fastcall GetArcadePlatinumTexture(HUDElement* pThis, void*, const char* name) {
	pArcadePlatinumImage = GetArcadePlatinumTexture_call(pThis, "MedalPlatinum");
	return pArcadeGoldImage = GetArcadePlatinumTexture_call(pThis, name);
}

void ApplyArcadeScoringPatches() {
	ReadFragDerbyRewardConfig();

	for (auto& value : fArcadeRacePositionMultiplier) {
		value = 1.0;
	}
	uintptr_t addresses[] = {
			0x48C632,
			0x48C685,
			0x48E02F,
	};
	for (auto& addr : addresses) {
		NyaHookLib::Patch(addr, fArcadeRacePositionMultiplier);
	}
	SetArcadeRaceMultiplierPointer(fArcadeRacePositionMultiplier);

	MoreFragDerbyRewardsASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E84D3, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E84E4, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E84F5, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E8506, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E8517, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E8528, &MoreFragDerbyRewardsASM);
	NyaHookLib::Fill(0x476DE4, 0x90, 0x476DEA - 0x476DE4); // disable reading of showbonus

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4EA18A, &ArcadePlatinumsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4EA07E, &GetArcadePlatinumTexture);
	ArcadePlatinumKeywordASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4F24FF, &ArcadePlatinumKeywordASM);

	// arcade targets are drawn in the hud at 004F2D14
	// arcade hud elements are drawn at 4EC4A2
}