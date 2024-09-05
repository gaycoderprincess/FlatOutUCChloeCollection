uintptr_t PostEvent_call = 0x4611D0;
float __attribute__((naked)) __fastcall PostEvent(int* eventData) {
	__asm__ (
		"pushad\n\t"
		"mov edx, 0x9298FB4\n\t"
		"mov edx, [edx]\n\t"
		"mov eax, ecx\n\t"
		"call %0\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (PostEvent_call)
	);
}

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

PlayerScoreDerby* GetPlayerScoreDerby(int playerId) {
	if (!pScoreManager) return nullptr;

	auto score = (PlayerScoreDerby**)pScoreManager->pScoresStart;
	auto end = (PlayerScoreDerby**)pScoreManager->pScoresEnd;
	while (score < end) {
		if ((*score)->nPlayerId + 1 == playerId) {
			return *score;
		}
		score++;
	}
	return nullptr;
}

void AddFragDerbyScore(int amount) {
	//if (!nFragDerbyCrashRewards) return;
	if (amount <= 0) return;
	if (pGame->nGameRules != GR_DEFAULT && pGame->nGameRules != GR_DERBY) return;
	if (pGame->nDerbyType != DERBY_FRAG) return;

	auto score = GetPlayerScoreDerby(1);
	if (!score) return;
	score->nScore1 += amount;

	int eventData[9] = {};
	eventData[0] = 6060;
	eventData[2] = score->nPlayerId + 1; // PlayerScoreDerby->4 = 0
	eventData[3] = 0xFFFF;
	eventData[4] = amount;
	PostEvent(eventData);
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

bool bArcadePlatinumEnabled = false;
int nArcadePlatinumCurrentLevelX = 0;
int nArcadePlatinumCurrentLevelY = 0;
bool bAchievedPlatinumThisRace = false;
void __stdcall ArcadePlatinums(void* a3, void** a1, int numPoints) {
	if (!bArcadePlatinumEnabled) return;

	auto target = nArcadePlatinumTargets[nArcadePlatinumCurrentLevelX][nArcadePlatinumCurrentLevelY];
	if (!target) return;

	// reset platinum status if we restarted
	if (numPoints <= 0) bAchievedPlatinumThisRace = false;
	// otherwise check for platinum score
	else if (numPoints > target && !bAchievedPlatinumThisRace) {
		CreatePopup(a1[2686], a3);
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
}