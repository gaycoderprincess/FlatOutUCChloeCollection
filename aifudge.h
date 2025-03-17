void SetAIFudgeFactor() {
	// don't init until handicap.bed exists
	if (!DoesFileExist("data/scripts/handicap.bed", 0) && !DoesFileExist("data/scripts/handicap_medium.bed", 0)) return;

	static int nLastAIFudge = -1;

	// set to normal for carnage races
	int currentAIFudge = nAIFudgeMode;
	if (pGameFlow->PreRace.nMode == GM_ARCADE_CAREER) currentAIFudge = 0;

	if (currentAIFudge < 0) currentAIFudge = 0;
	if (currentAIFudge >= NUM_FUDGE_MODES) currentAIFudge = NUM_FUDGE_MODES-1;

	if (nLastAIFudge != currentAIFudge) {
		const char* aiUpgrades[] = {
				"AIUpgradeLevelEasy",
				"AIUpgradeLevelMedium",
				"AIUpgradeLevelHard",
		};
		const char* cupWinnings[] = {
				"CupWinningsEasy",
				"CupWinningsMedium",
				"CupWinningsHard",
		};
		const char* handicapFiles[] = {
				"data/scripts/handicap_easy.bed",
				"data/scripts/handicap_medium.bed",
				"data/scripts/handicap_hard.bed",
		};
		auto handicap = handicapFiles[currentAIFudge];
		NyaHookLib::Patch(0x47FD4C + 1, DoesFileExist(handicap, 0) ? handicap : "data/scripts/handicap.bed");
		NyaHookLib::Patch(0x45FA4B + 1, aiUpgrades[currentAIFudge]);
		NyaHookLib::Patch(0x46AA81 + 1, cupWinnings[currentAIFudge]);

		static auto config = toml::parse_file("Config/AIConfig.toml");

		NyaHookLib::Patch<uint16_t>(0x480ABD, 0x9090);
		NyaHookLib::Patch<uint8_t>(0x480AC5, 0xEB);
		static float fFudge = 1.0;
		fFudge = 1.0 / config["main"][std::format("AIFudge_Diff{}", currentAIFudge+1)].value_or(1.0);
		NyaHookLib::Patch(0x480ACF + 2, &fFudge);

		//NyaHookLib::Patch<uint64_t>(0x481D9A, nAIFudgeDisabled > 1 ? 0x44D990000000D4E9 : 0x44D9000000D38E0F); // disable velocity limits
		nLastAIFudge = currentAIFudge;
	}

	// set the bump mass of all opponents to 1x if on competitive or above
	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		for (int i = 0; i < 32; i++) {
			auto ply = (AIPlayer*)GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType != PLAYERTYPE_AI) continue;

			if (currentAIFudge == FUDGE_HARD) {
				ply->AIProfile.fBumpMassDriver = 1.0;
			}
			// no catchup for easy diff
			if (currentAIFudge == FUDGE_EASY) {
				//ply->AIProfile.fHandicapMul = 1.0;
				//ply->AIProfile.fRLMagnetMul = 0.0;
				ply->AIProfile.fCatchUpMul = 0.0;
				if (ply->AIProfile.fDefaultTurbo > 0.0) ply->AIProfile.fDefaultTurbo = 0.0;
			}
		}
	}
}

void SetGlobalFudgeFactor() {
	float fudge = 3.5;
	if (nAIFudgeMode < FUDGE_HARD || bIsInMultiplayer/* && pGameFlow->PreRace.nMode == GM_CAREER*/) fudge = 5;
	*(float*)0x849434 = fudge;
	*(float*)0x849454 = fudge;
}