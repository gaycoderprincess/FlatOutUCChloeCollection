namespace LapKnockoutMode {
	bool bTimedMode = true;
	const int nKnockoutTimer = 15000;
	const int nKnockoutTimerGracePeriod = 10000;
	int nTimedLastLap = 0;

	int GetNumPlayersKnockedOutPerLap() {
		int numPlayers = pGameFlow->pHost->GetNumPlayers();
		int numLaps = pScoreManager->nNumLaps;

		int div = numPlayers / numLaps;
		while (div * numLaps > numPlayers) {
			div--;
		}
		if (div < 1) div = 1;
		return div;
	}

	void GetRaceTypeString(wchar_t* str, size_t len) {
		_snwprintf(str, len, bTimedMode ? L"KNOCKOUT" : L"LAP KNOCKOUT");
	}

	void GetRaceDescString(wchar_t* str, size_t len) {
		const wchar_t* descString1 = L"\n\nKnockout races always take place on circuit tracks.\n\nAt the end of each lap the slowest racers are knocked out.\n\nTry to keep to the front of the pack, otherwise you won't last long.";
		const wchar_t* descString2 = L"\n\nKnockout races always take place on circuit tracks.\n\nEvery 15 seconds the slowest racer is knocked out.\n\nTry to keep to the front of the pack, otherwise you won't last long.";
		_snwprintf(str, len, bTimedMode ? descString2 : descString1);
	}

	int GetLowestLap() {
		int lap = 99;
		for (int i = 0; i < 32; i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nIsWrecked) continue;
			if (ply->nCurrentLap < lap) lap = ply->nCurrentLap;
		}
		return lap;
	}

	Player* GetFirstPlacePlayer() {
		for (int i = 0; i < 32; i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
			if (score->nPosition == 1) return ply;
		}
		// default to player 0 if nobody's first
		return GetPlayer(0);
	}

	int GetHighestPositionToLeaveAlive(int lap) {
		// on lap 1 with 13 ai this would be:
		// 13 - 1 * 1
		// 12
		// knock out 12th
		int highestPos = pGameFlow->pHost->GetNumPlayers() - (lap * GetNumPlayersKnockedOutPerLap());
		if (lap > pScoreManager->nNumLaps) highestPos = 1; // always knock out every player once 1st place finishes
		return highestPos;
	}

	int GetHighestPositionToLeaveAliveByTime() {
		auto time = pPlayerHost->nRaceTime - nKnockoutTimerGracePeriod;

		int count = 0;
		while (time > nKnockoutTimer) {
			count++;
			time -= nKnockoutTimer;
		}

		return pGameFlow->pHost->GetNumPlayers() - count;
	}

	void KnockOutLowestPlayers() {
		int highestPos;
		if (bTimedMode) {
			highestPos = GetHighestPositionToLeaveAliveByTime();
			if (highestPos <= 1) {
				highestPos = 1;
				pScoreManager->nNumLaps = nTimedLastLap - 1;
				if (auto ply = GetFirstPlacePlayer()) {
					if (!GetPlayerScore<PlayerScoreRace>(ply->nPlayerId)->bHasFinished) {
						auto eventData = tEventData(EVENT_PLAYER_ON_FINISH, ply->nPlayerId);
						pEventManager->PostEvent(&eventData);
					}
				}
			}
			else {
				nTimedLastLap = GetFirstPlacePlayer()->nCurrentLap + 1;
			}
		}
		else {
			highestPos = GetHighestPositionToLeaveAlive(GetFirstPlacePlayer()->nCurrentLap);
		}

		for (int i = 0; i < 32; i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nIsWrecked) continue;
			auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
			if (score->nPosition == 1) continue;
			if (score->nPosition > highestPos) {
				auto eventData = tEventData(EVENT_PLAYER_WRECKED, ply->nPlayerId);
				pEventManager->PostEvent(&eventData);
				ply->nIsWrecked = true;
			}
		}
	}

	void OnEvent(tEventData* event, void* data) {
		if (!bIsLapKnockout) return;
		if (bTimedMode) return;
		if (event->type != EVENT_PLAYER_ON_LAP) return;

		int playerId = event->data[1] - 1;
		auto ply = GetPlayer(playerId);
		if (ply == GetFirstPlacePlayer()) {
			KnockOutLowestPlayers();
		}
	}

	int32_t GetTimeKnockoutTimeLeft() {
		// initial grace period
		if (pPlayerHost->nRaceTime <= nKnockoutTimerGracePeriod) {
			return (nKnockoutTimer - pPlayerHost->nRaceTime) + nKnockoutTimerGracePeriod;
		}
		return nKnockoutTimer - ((pPlayerHost->nRaceTime - nKnockoutTimerGracePeriod) % nKnockoutTimer);
	}

	void ProcessTimerTick() {
		static CNyaTimer gTimer;
		gTimer.Process();

		static int32_t lastTimeLeft = 0;

		int32_t timeLeft = GetTimeKnockoutTimeLeft();
		if (timeLeft == lastTimeLeft) return;
		lastTimeLeft = timeLeft;

		if (timeLeft > 5000) {
			gTimer.fTotalTime = 0.5;
			return;
		}

		double frequency = 1.0;
		if (timeLeft <= 5000) {
			frequency = 0.5;
		}
		if (timeLeft <= 3000) {
			frequency = 0.25;
		}
		if (timeLeft <= 1000) {
			frequency = 0.1;
		}
		if (gTimer.fTotalTime >= frequency) {
			auto eventData = tEventData(EVENT_SFX_TIME_TICK);
			pEventManager->PostEvent(&eventData);
			while (gTimer.fTotalTime >= frequency) {
				gTimer.fTotalTime -= frequency;
			}
		}
	}

	void OnTick() {
		if (!bIsLapKnockout) return;
		if (!bTimedMode) return;
		if (!pScoreManager) return;
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nRaceState != RACE_STATE_RACING && pGameFlow->nRaceState != RACE_STATE_FINISHED) return;

		pScoreManager->nNumLaps = 10;
		KnockOutLowestPlayers();
		ProcessTimerTick();
	}

	int GetTimeLeftString(wchar_t* str, size_t len, void* a3, void* a4) {
		int32_t time = GetTimeKnockoutTimeLeft();
		std::string timestr = (time <= 4500) ? "#40#" : "#39#";
		timestr += GetTimeFromMilliseconds(time, true);
		timestr.pop_back(); // remove trailing 0, the game has a tickrate of 100fps
		return mbstowcs(str, timestr.c_str(), len);
	}

	void LapKnockoutKeyword(void* a3) {
		AddHUDKeyword("LAPKNOCKOUT_TIMELEFT", &GetTimeLeftString, a3);
	}

	void ApplyPatches(bool apply) {
		static bool bRegistered = false;
		if (!bRegistered) {
			auto tmp = new int;
			EventManager::AddHandler(1, pEventManager, OnEvent, 43, tmp);
			bRegistered = true;
		}

		bIsLapKnockout = apply;
		NyaHookLib::Patch(0x4F2494 + 1, apply ? (uintptr_t)&GetRaceTypeString : 0x4F2920);
		NyaHookLib::Patch(0x4F24BC + 1, apply ? (uintptr_t)&GetRaceDescString : 0x4F25F0);

		NyaHookLib::Patch(0x4DC0FF + 1, apply && bTimedMode ? "Data.Overlay.HUD.Knockout" : "Data.Overlay.HUD.Race");

		NyaFO2Hooks::PlaceHUDKeywordHook();
		NyaFO2Hooks::aHUDKeywordFuncs.push_back(LapKnockoutKeyword);
	}
}