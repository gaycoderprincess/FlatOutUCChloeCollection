namespace CareerRally {
	int nNumRacesInThisCup = 0;
	bool bNewCupJustFinished = false; // only hit if the cup wasn't finished before

	void OnTick() {
		// already done by the time trial check in careertimetrial
		//if (bIsTimeTrial && pGameFlow->nGameState == GAME_STATE_RACE && GetPlayer(0)) {
		//	GetPlayer(0)->nStartPosition = 1;
		//}
	}

	//void ApplyPatches(bool apply) {
	//	// make the scoreboard display 13 players at a time
	//	NyaHookLib::Patch<uint8_t>(0x4B3DD7, apply && GetOpponentCount() >= 11 ? 13 : 12);
	//	NyaHookLib::Patch<uint8_t>(0x4B3DDD, apply && GetOpponentCount() >= 11 ? 13 : 12);
	//}

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
		bNewCupJustFinished = false;

		gCustomSave.CalculateRallyPlayersByPosition();
		auto cup = &gCustomSave.aRallyCareer[gCustomSave.nRallyClass][gCustomSave.nRallyCup];
		auto nextCup = &gCustomSave.aRallyCareer[gCustomSave.nRallyClass][gCustomSave.nRallyCup+1];
		cup->bEventUnlocked = 1;
		int pos = gCustomSave.aRallyPlayerPosition[0]+1;
		if (!cup->nEventPosition || pos < cup->nEventPosition) {
			cup->nEventPosition = pos;
		}
		// unlock the next cup if finished 3rd or higher
		if (cup->nEventPosition && cup->nEventPosition <= 3) {
			if (cup->nEventPosition == 1) {
				AwardAchievement(GetAchievement("WIN_RALLY_RACE"));
			}
			if (!nextCup->bEventUnlocked) bNewCupJustFinished = true;
			nextCup->bEventUnlocked = 1;
		}
	}

	void OnEventFinished() {
		auto event = &gCustomSave.aRallyCareerEvents[gCustomSave.nRallyClass][gCustomSave.nRallyEvent];
		auto nextEvent = &gCustomSave.aRallyCareerEvents[gCustomSave.nRallyClass][gCustomSave.nRallyEvent+1];
		event->bEventUnlocked = 1;
		int pos = pGameFlow->PostRace.nPlayerPosition[0];
		if (!event->nEventPosition || pos < event->nEventPosition) {
			event->nEventPosition = pos;
		}
		// unlock the next event if finished 3rd or higher
		if (event->nEventPosition && event->nEventPosition <= 3) {
			nextEvent->bEventUnlocked = 1;
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

	void AdvanceEvent() {
		OnEventFinished();
		gCustomSave.Save();
	}
}