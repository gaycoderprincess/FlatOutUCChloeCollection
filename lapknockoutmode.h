namespace LapKnockoutMode {
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
		_snwprintf(str, len, L"LAP KNOCKOUT");
	}

	void GetRaceDescString(wchar_t* str, size_t len) {
		const wchar_t* descString = L"\n\nKnockout races always take place on circuit tracks.\n\nAt the end of each lap the slowest racers are knocked out.\n\nTry to keep to the front of the pack, otherwise you won't last long.";
		_snwprintf(str, len, descString);
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
		return nullptr;
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

	void KnockOutLowestPlayers() {
		//auto highestPos = GetHighestPositionToLeaveAlive(GetLowestLap());
		auto highestPos = GetHighestPositionToLeaveAlive(GetFirstPlacePlayer()->nCurrentLap);

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
		if (event->type != EVENT_PLAYER_ON_LAP) return;

		int playerId = event->data[1] - 1;
		auto ply = GetPlayer(playerId);
		if (ply == GetFirstPlacePlayer()) {
			KnockOutLowestPlayers();
		}
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
	}
}