namespace CareerTimeTrial {
	bool bIsCareerTimeTrial = false;
	int nCar = 0;
	bool bUpgrades = false;
	uint32_t nMedalTimes[5];
	uint32_t nEventClass;
	uint32_t nEventId;
	uint32_t nStartPosition;
	enum eMedal {
		MEDAL_GOLD,
		MEDAL_SILVER,
		MEDAL_BRONZE,
		MEDAL_AUTHOR,
		MEDAL_SAUTHOR
	};

	void DoRaceStandings() {
		// if super author opponents are on, subtract 2 from player position so you still finish 1st
		auto ply = GetPlayerScore<PlayerScoreRace>(1);
		if (!ply) return;
		if (nShowSuperAuthors < 2) return;

		ply->nPosition -= 2;
		if (ply->nPosition < 1) ply->nPosition = 1;
	}

	void __attribute__((naked)) __fastcall RaceStandingsASM() {
		__asm__ (
			"pushad\n\t"
			"call %0\n\t"
			"popad\n\t"
			"pop edi\n\t"
			"pop esi\n\t"
			"pop ebp\n\t"
			"pop ebx\n\t"
			"ret 4\n\t"
				:
				: "i" (DoRaceStandings)
		);
	}

	void ApplyPatches(bool apply) {
		bIsTimeTrial = apply;
		bIsCareerTimeTrial = apply;
		if (!apply) bSkipTuningThisRace = false;
		NyaHookLib::Patch<uint8_t>(0x46937D, apply ? 0xEB : 0x74); // never use career active car
		NyaHookLib::Patch<uint64_t>(0x469BFE, apply ? 0xF883909090909090 : 0xF8830000020A840F); // use UpgradeLevel
		NyaHookLib::Patch<uint8_t>(0x469CE1, apply && bUpgrades ? 0xEB : 0x74); // use UpgradeLevel SingleRace
		NyaHookLib::Patch(0x4DC0FF + 1, apply ? "Data.Overlay.HUD.ChloeTimeTrial" : "Data.Overlay.HUD.Race"); // use UpgradeLevel SingleRace

		if (apply) {
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x48BBAE, &RaceStandingsASM);
		}
		else {
			NyaHookLib::Patch<uint64_t>(0x48BBAE, 0xCC0004C25B5D5E5F);
		}
	}

	void SetPlayerColor(Player* ply) {
		if (!ply) return;

		auto tmp = *(NyaDrawing::CNyaRGBA32*)&ply->nArrowColor;
		auto color = NyaDrawing::CNyaRGBA32(tmp.b, tmp.g, tmp.r, tmp.a);
		switch (ply->nPlayerId) {
			// gold
			case 2:
				color.r = 236;
				color.g = 221;
				color.b = 16;
				break;
			// silver
			case 3:
				color.r = 186;
				color.g = 186;
				color.b = 186;
				break;
			// bronze
			case 4:
				color.r = 175;
				color.g = 100;
				color.b = 0;
				break;
			// author
			case 5:
				color.r = 30;
				color.g = 160;
				color.b = 0;
				break;
			// super author
			case 6:
				color.r = 219;
				color.g = 100;
				color.b = 193;
				break;
			default:
				break;
		}
		tmp = color;
		tmp.r = color.b;
		tmp.b = color.r;
		ply->nArrowColor = *(uint32_t*)&tmp;
	}

	void OnTick() {
		if (bIsTimeTrial && pGameFlow->nGameState == GAME_STATE_RACE && GetPlayer(0)) {
			GetPlayer(0)->nStartPosition = bIsCareerTimeTrial ? nStartPosition : 1;
		}

		if (!bIsCareerTimeTrial) return;
		if (pGameFlow->PreRace.nMode != GM_CAREER) return;
		bSkipTuningThisRace = true;
		pGameFlow->PreRace.nCar = nCar;
		pGameFlow->PreRace.nCarSkin = 1;

		if (pGameFlow->nGameState == GAME_STATE_RACE && GetPlayer(0)) {
			for (int i = 1; i < 32; i++) {
				SetPlayerColor(GetPlayer(i));
			}
		}
	}

	void SetMedal(uint32_t pbTime) {
		auto& event = gCustomSave.aCareerEvents[CareerTimeTrial::nEventClass][CareerTimeTrial::nEventId];
		if (!event.pbTime || pbTime <= event.pbTime) {
			event.pbTime = pbTime;

			auto& medals = nMedalTimes;
			if (medals[MEDAL_BRONZE] && event.pbTime <= medals[MEDAL_BRONZE]) event.medal = MEDAL_BRONZE;
			if (medals[MEDAL_SILVER] && event.pbTime <= medals[MEDAL_SILVER]) event.medal = MEDAL_SILVER;
			if (medals[MEDAL_GOLD] && event.pbTime <= medals[MEDAL_GOLD]) event.medal = MEDAL_GOLD;
			if (medals[MEDAL_AUTHOR] && event.pbTime <= medals[MEDAL_AUTHOR]) event.medal = MEDAL_AUTHOR;
			if (medals[MEDAL_SAUTHOR] && event.pbTime <= medals[MEDAL_SAUTHOR]) event.medal = MEDAL_SAUTHOR;
		}
	}
}