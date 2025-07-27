namespace SpeedtrapMode {
	bool bSimpleUI = false;

	float fSpeedMultiplier = 1;

	void GetRaceTypeString(wchar_t* str, size_t len) {
		_snwprintf(str, len, L"SPEEDTRAP");
	}

	void GetRaceDescString(wchar_t* str, size_t len) {
		const wchar_t* descString = L"\n\nGet the highest speed possible through each checkpoint.\n\nThe racer with the highest cumulative speed through all the checkpoints wins.\n\nDon't fall behind - points will be lost after the first car crosses the finish line!";
		_snwprintf(str, len, descString);
	}

	void AddScore(int playerId, const wchar_t* str, float amount, int category) {
		auto playerScore = GetPlayerScore<PlayerScoreArcadeRace>(playerId+1);
		GameFlow::AddArcadeRaceScore(str, category, pGameFlow, amount, playerScore->nPosition);
	}

	std::string GetStringNarrow(const wchar_t* string) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(string);
	}

	std::string GetMPSLocalized(int value) {
		double fValue = value / 100.0;
		if (bImperialUnits) return std::format("{} MPH", (int)(fValue * 2.23694));
		return std::format("{} KMH", (int)(fValue * 3.6));
	}

	uint32_t nPlayerScore[32];
	uint32_t nLastPlayerScore[32];
	double fSplitTimer = 0;
	uint32_t nLastSplitSpeed = 0;
	void DrawScoreboard() {
		if (bIsInMultiplayer) return;

		tNyaStringData data;
		data.x = 0.05 * GetAspectRatioInv();
		data.y = 0.26 - (data.size * 3);
		data.size = 0.025;
		data.outlinea = 255;

		struct tLeaderboardEntry {
			std::wstring name;
			uint32_t score;
			bool knockedOut;
			bool isLocalPlayer;

			static bool compFunction(const tLeaderboardEntry& a, const tLeaderboardEntry& b) {
				return a.score > b.score;
			}
		};
		std::vector<tLeaderboardEntry> aLeaderboard;

		for (int i = 0; i < 32; i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			aLeaderboard.push_back({pGameFlow->aPlayerInfos[i].sPlayerName, nPlayerScore[i], ply->nIsWrecked != 0, i == 0});
		}

		std::sort(aLeaderboard.begin(), aLeaderboard.end(), tLeaderboardEntry::compFunction);

		for (auto& ply : aLeaderboard) {
			if (ply.knockedOut) {
				data.SetColor(64, 64, 64, 255);
			}
			else {
				if (ply.isLocalPlayer) {
					data.SetColor(241, 193, 41, 255); // 0xFFF1C129
				}
				else data.SetColor(255, 237, 195, 255);
			}

			data.x = 0.15 * GetAspectRatioInv();
			data.XRightAlign = false;
			DrawString(data, std::format("{}. {}", (&ply - &aLeaderboard[0]) + 1, GetStringNarrow(ply.name.c_str()), ply.score), &DrawStringFO2);
			//data.x = 0.45 * GetAspectRatioInv();
			data.x = 0.47 * GetAspectRatioInv();
			data.XRightAlign = true;
			DrawString(data, GetMPSLocalized(ply.score), &DrawStringFO2);
			data.y += data.size;
		}
	}

	bool HasAnyoneFinished() {
		for (int i = 0; i < 32; i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			if (ply->nCurrentLap >= pScoreManager->nNumLaps) return true;
		}
		return false;
	}

	bool HasPlayerFinished() {
		return GetPlayer(0)->nCurrentLap >= pScoreManager->nNumLaps;
	}

	void DrawSplitHUD() {
		DrawScoreboard();

		static CNyaTimer gSplitTimer;
		gSplitTimer.Process();

		if (fSplitTimer > 0) {
			tNyaStringData data;
			data.x = 0.5;
			data.y = 0.3 - 0.04;
			data.size = 0.04;
			data.XCenterAlign = true;
			data.outlinea = 255;
			if (fSplitTimer < 0.5) {
				data.a = data.outlinea = fSplitTimer * 2 * 255;
			}

			DrawString(data, std::format("+{}", GetMPSLocalized(nLastSplitSpeed)), &DrawStringFO2);
		}
		fSplitTimer -= gSplitTimer.fDeltaTime;

		if (!HasPlayerFinished() && HasAnyoneFinished()) {
			tNyaStringData data;
			data.x = 0.5;
			data.y = 0.3 + 0.04;
			data.size = 0.04;
			data.XCenterAlign = true;
			data.outlinea = 255;
			data.SetColor(255,0,0,255);
			DrawString(data, "HURRY UP!", &DrawStringFO2);
			data.y += data.size;
			DrawString(data, "SPEED PENALTY", &DrawStringFO2);
		}
	}

	void OnEvent(tEventData* event, void* data) {
		if (!bIsSpeedtrap) return;
		if (event->type != EVENT_PLAYER_ON_ARCADE_CHECKPOINT) return;

		int playerId = event->data[1] - 1;
		auto ply = GetPlayer(playerId);
		uint32_t points = (uint32_t)(ply->pCar->GetVelocity()->length() * 100 * fSpeedMultiplier);
		if (playerId == 0) {
			auto spd = ply->pCar->GetVelocity()->length() * 100;
			if (spd >= 83.3 * 100) {
				AwardAchievement(GetAchievement("FAST_SPEEDTRAP"));
			}
			nLastSplitSpeed = spd;
			fSplitTimer = 3;
			AddScore(playerId, L"CHECKPOINT!", points, 2);
		}
		nPlayerScore[playerId] += points;
	}

	void OnTick() {
		static CNyaTimer gTimer;
		gTimer.Process();
		static double fDeductTimer = 0;

		if (pGameFlow->nRaceState != RACE_STATE_RACING) {
			memset(nPlayerScore, 0, sizeof(nPlayerScore));
			nLastSplitSpeed = 0;
			fSplitTimer = 0;
			fDeductTimer = 0;
			return;
		}
		memcpy(nLastPlayerScore, nPlayerScore, sizeof(nPlayerScore));

		if (!bIsSpeedtrap) return;
		if (pLoadingScreen) return;
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nGameRules != GR_ARCADE_RACE) return;

		if (HasAnyoneFinished()) {
			fDeductTimer += gTimer.fDeltaTime;
			if (fDeductTimer >= 0.75) {
				for (int i = 0; i < 32; i++) {
					auto ply = GetPlayer(i);
					if (!ply) continue;
					if (ply->nCurrentLap < pScoreManager->nNumLaps) {
						int penalty = 2.77777777 * 100;
						if (nPlayerScore[i] >= penalty) {
							if (i == 0) {
								AddScore(i, L"HURRY UP!", -penalty, 1);
							}
							nPlayerScore[i] -= penalty;
						}
					}
				}
				fDeductTimer -= 0.75;
			}
		}
		else fDeductTimer = 0;

		DrawSplitHUD();
	}

	float GetLapDecayMultiplier() {
		return 0.0f;
	}

	int GetPlayerPosition(int id) {
		int pos = 1;
		int score = nLastPlayerScore[id];
		// position = 1 + amount of players with a higher score
		for (int i = 0; i < 32; i++) {
			if (nLastPlayerScore[i] > score) pos++;
		}
		return pos;
	}

	void DoRaceStandings() {
		for (int i = 0; i < pScoreManager->aScores.GetSize(); i++) {
			auto score = pScoreManager->aScores[i];
			score->nPosition = GetPlayerPosition(score->nPlayerId);
		}
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
		static bool bRegistered = false;
		if (!bRegistered) {
			auto tmp = new int;
			EventManager::AddHandler(1, pEventManager, OnEvent, 43, tmp);
			bRegistered = true;
		}

		bIsSpeedtrap = apply;
		SetArcadeRaceMultiplierPointer(apply ? fArcadeRacePositionMultiplierStunt : fArcadeRacePositionMultiplier);
		// remove scenery crash bonus
		NyaHookLib::Patch<uint8_t>(0x48D175, apply ? 0xEB : 0x75);
		// remove airtime bonus
		NyaHookLib::Patch<uint8_t>(0x48D109, apply ? 0xEB : 0x75);
		// remove checkpoint bonus
		NyaHookLib::Patch<uint8_t>(0x48CB46, apply ? 0xEB : 0x75);
		// don't read starttime
		NyaHookLib::Patch<uint16_t>(0x48C78F, apply ? 0x9090 : 0x0C7F);
		NyaHookLib::Patch(0x48C797, apply ? (INT_MAX / 2) : 120000);

		static float fZero = 0.0;
		// no checkpoint timer popup
		NyaHookLib::Patch(0x48CAB9 + 2, apply ? &fZero : (float*)0x6F85A0);
		// immediately apply bonuses
		NyaHookLib::Patch(0x46F344 + 2, apply ? &fZero : (float*)0x6DB840);

		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x48CA54, apply ? (uintptr_t)&GetLapDecayMultiplier : 0x48E130);

		static uint64_t _0x4DC005Backup = *(uint64_t*)0x4DC005; // this is going to change as part of it calls my malloc hook

		if (apply && bSimpleUI) {
			// load FragDerbyOnline HUD
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4DC005, 0x4DC186);
			NyaHookLib::Patch<uint16_t>(0x4DC192, 0x9090);
			NyaHookLib::Patch<uint16_t>(0x4DC19B, 0x9090);
		}
		else {
			NyaHookLib::Patch<uint64_t>(0x4DC005, _0x4DC005Backup);
			NyaHookLib::Patch<uint16_t>(0x4DC192, 0x4D75);
			NyaHookLib::Patch<uint16_t>(0x4DC19B, 0x2275);
		}
		NyaHookLib::Patch(0x4DC1AE + 1, apply ? "Data.Overlay.HUD.Speedtrap" : "Data.Overlay.HUD.FragDerbyOnline");
		NyaHookLib::Patch(0x4DC017 + 1, apply ? "Data.Overlay.HUD.SpeedtrapArcade" : "Data.Overlay.HUD.ArcadeRace");

		// remove vanilla leaderboards
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E0CDC, apply ? 0x4E5291 : 0x4E4560);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E0F6B, apply ? 0x4E5291 : 0x4E4560);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4E3A58, apply ? 0x4E5291 : 0x4E4560);

		// remove crash bonuses
		NyaHookLib::Patch<uint64_t>(0x48C957, apply ? 0x4B8B9000000210E9 : 0x4B8B0000020F850F);
		NyaHookLib::Patch<uint64_t>(0x48C897, apply ? 0x4B8B90000002D0E9 : 0x4B8B000002CF850F);
		NyaHookLib::Patch<uint64_t>(0x48C8D7, apply ? 0x438B9000000290E9 : 0x438B0000028F850F);
		NyaHookLib::Patch<uint64_t>(0x48C997, apply ? 0x438B90000001D0E9 : 0x438B000001CF850F);
		NyaHookLib::Patch<uint64_t>(0x48C9D7, apply ? 0x538B9000000190E9 : 0x538B0000018F850F);
		NyaHookLib::Patch<uint64_t>(0x48C917, apply ? 0x538B9000000250E9 : 0x538B0000024F850F);
		NyaHookLib::Patch<uint64_t>(0x48C9D7, apply ? 0x538B9000000190E9 : 0x538B0000018F850F);

		NyaHookLib::Patch(0x4F2494 + 1, apply ? (uintptr_t)&GetRaceTypeString : 0x4F2920);
		NyaHookLib::Patch(0x4F24BC + 1, apply ? (uintptr_t)&GetRaceDescString : 0x4F25F0);

		// don't write arcade data to the gameflow results, fixes career scoring
		NyaHookLib::Patch<uint64_t>(0x48DB89, apply ? 0x868D9000000156E9 : 0x868D000001558E0F);

		if (apply) {
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x48BBAE, &RaceStandingsASM);
		}
		else {
			NyaHookLib::Patch<uint64_t>(0x48BBAE, 0xCC0004C25B5D5E5F);
		}

		// no nitro regen by default in arcade race
		NyaHookLib::Patch<uint32_t>(0x469BB0, apply ? 0x469AF5 : 0x469AAC);
	}
}