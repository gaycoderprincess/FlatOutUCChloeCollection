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

PlayerScoreDerby* pPlayerScoreDerby = nullptr;

void AddFragDerbyScore(int amount) {
	if (!nFragDerbyCrashRewards) return;
	if (amount <= 0) return;
	if (pGame->nGameRules != GR_DERBY) return;
	if (pGame->nDerbyType != DERBY_FRAG) return;

	auto score = pPlayerScoreDerby;
	if (!score) return;
	score->nScore += amount;

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

auto MoreFragDerbyRewardsGetScoreManager_call = (void*(__thiscall*)(void*, int))0x48FF80;
void* __fastcall MoreFragDerbyRewardsGetScoreManager(void* a1, void*, int a2) {
	auto ret = MoreFragDerbyRewardsGetScoreManager_call(a1, a2);
	if (a2 == 0) pPlayerScoreDerby = (PlayerScoreDerby*)ret;
	return ret;
}

void ReadFragDerbyRewardConfig() {
	auto config = toml::parse_file("Config/FragDerbyRewards.toml");
	nFragDerbyRewardSlam = config["main"]["Slam"].value_or(nFragDerbyRewardSlam);
	nFragDerbyRewardSuperFlip = config["main"]["SuperFlip"].value_or(nFragDerbyRewardSuperFlip);
	nFragDerbyRewardPowerHit = config["main"]["PowerHit"].value_or(nFragDerbyRewardPowerHit);
	nFragDerbyRewardBlastOut = config["main"]["BlastOut"].value_or(nFragDerbyRewardBlastOut);
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

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E84D3, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E84E4, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E84F5, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E8506, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E8517, &MoreFragDerbyRewardsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E8528, &MoreFragDerbyRewardsASM);
	NyaHookLib::Patch(0x6E2BEC, &MoreFragDerbyRewardsGetScoreManager);
	NyaHookLib::Fill(0x476DE4, 0x90, 0x476DEA - 0x476DE4); // disable reading of showbonus
}