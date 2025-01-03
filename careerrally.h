namespace CareerRally {
	bool bIsCareerRally = false;
	int nNumRacesInThisCup = 0;

	void OnTick() {
		// already done by the time trial check in careertimetrial
		//if (bIsTimeTrial && pGameFlow->nGameState == GAME_STATE_RACE && GetPlayer(0)) {
		//	GetPlayer(0)->nStartPosition = 1;
		//}
	}

	int pointsPerPosition[32] = {
			10,
			8,
			6,
			5,
			4,
			3,
			2,
			1,
	};

	void OnCupFinished() {
		gCustomSave.CalculateRallyPlayersByPosition();
		auto cup = &gCustomSave.aRallyCareer[gCustomSave.nRallyClass][gCustomSave.nRallyCup];
		cup->bEventUnlocked = 1;
		int pos = gCustomSave.aRallyPlayerPosition[0]+1;
		if (!cup->nEventPosition || pos < cup->nEventPosition) {
			cup->nEventPosition = pos;
		}
	}

	void AdvanceCup() {
		for (int i = 0; i < 32; i++) {
			auto pos = pGameFlow->PostRace.nPlayerPosition[i]-1;
			if (pos >= 32) continue;
			gCustomSave.nRallyCupPoints[i] += pointsPerPosition[pos];
			gCustomSave.nRallyCupStagePoints[gCustomSave.nRallyCupNextStage][i] = pointsPerPosition[pos];
		}
		gCustomSave.nRallyCupStagePosition[gCustomSave.nRallyCupNextStage] = pGameFlow->PostRace.nPlayerPosition[0];
		gCustomSave.nRallyCupNextStage++;
		if (gCustomSave.nRallyCupNextStage >= CareerRally::nNumRacesInThisCup) {
			OnCupFinished();
		}
		gCustomSave.Save();
	}
}