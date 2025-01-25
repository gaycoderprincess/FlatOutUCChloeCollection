void SetAIFudgeFactor() {
	static int nLastAIFudgeDisabled = -1;

	// set to normal for carnage races
	int currentAIFudge = nAIFudgeDisabled;
	if (pGameFlow->PreRace.nMode == GM_ARCADE_CAREER) currentAIFudge = 0;

	if (nLastAIFudgeDisabled != currentAIFudge) {
		NyaHookLib::Patch<uint16_t>(0x480ABD, currentAIFudge ? 0x9090 : 0x1875);
		NyaHookLib::Patch<uint8_t>(0x480AC5, currentAIFudge ? 0xEB : 0x75);
		static float fFudgeNone = 1.0;
		static float fFudgeHard = 0.75;
		NyaHookLib::Patch(0x480ACF + 2, currentAIFudge == 2 ? &fFudgeHard : &fFudgeNone);
		//NyaHookLib::Patch<uint64_t>(0x481D9A, nAIFudgeDisabled > 1 ? 0x44D990000000D4E9 : 0x44D9000000D38E0F); // disable velocity limits
		nLastAIFudgeDisabled = currentAIFudge;
	}

	// set the bump mass of all opponents to 1x if on competitive or above
	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		for (int i = 0; i < 32; i++) {
			auto ply = (AIPlayer*)GetPlayer(i);
			if (!ply) continue;
			if (ply->nPlayerType != PLAYERTYPE_AI) continue;

			if (currentAIFudge >= 1) {
				ply->AIProfile.fBumpMassDriver = 1.0;
			}
			// no catchup for easy diff
			if (currentAIFudge == 0) {
				//ply->AIProfile.fHandicapMul = 1.0;
				//ply->AIProfile.fRLMagnetMul = 0.0;
				ply->AIProfile.fCatchUpMul = 0.0;
				if (ply->AIProfile.fDefaultTurbo > 0.0) ply->AIProfile.fDefaultTurbo = 0.0;
			}
		}
	}
}