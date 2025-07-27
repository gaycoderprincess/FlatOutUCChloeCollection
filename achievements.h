namespace Achievements {
	int nTotalProgression = 0;
	int nCurrentSaveSlot = 0;

	std::string GetAchievementSavePath(int id) {
		return std::format("Savegame/customsave{:03}.ach", id);
	}

	enum eAchievementCategory {
		CAT_GENERAL = 1,
		CAT_SINGLEPLAYER = 2,
		CAT_MULTIPLAYER = 4,
		CAT_CAREER = 8,
		CAT_CARNAGE = 16,
		CAT_RALLY = 32,
	};

	class CAchievement {
	public:
		const char* sIdentifier;
		const char* sName;
		const char* sDescription;
		int nProgress = 0;
		float fInternalProgress = 0;
		bool bUnlocked = false;
		bool bHidden = false;
		int nCategory = 0;

		CAchievement() = delete;
		CAchievement(const char* identifier, const char* name, const char* description, int category, bool hidden = false) {
			sIdentifier = identifier;
			sName = name;
			sDescription = description;
			bHidden = hidden;
			nCategory = category;
		}
	};

	std::vector<CAchievement*> gAchievements = {
		new CAchievement("WIN_RACE", "Starting Point", "Win a race", CAT_GENERAL),
		new CAchievement("WIN_MP_RACE", "Friendly Competition", "Win a multiplayer race", CAT_MULTIPLAYER),
		new CAchievement("WIN_RALLY_RACE", "Aspiring Rally Driver", "Win a rally cup", CAT_RALLY),
		new CAchievement("WIN_RACE_WRECK", "Eliminator", "Win a race after wrecking everyone", CAT_SINGLEPLAYER),
		new CAchievement("WIN_RACE_BUG", "Retro Demo", "Win a race with the Retro Bug", CAT_GENERAL),
		new CAchievement("WIN_RALLY_SAAB", "Boat Award", "Win a rally stage with the Saab 96", CAT_RALLY),
		new CAchievement("WIN_RALLY_SAAB_2", "Hardcore Boat Award", "Win a rally stage with the Saab 96 on Sadistic", CAT_RALLY, true),
		new CAchievement("WIN_CUP_PEPPER", "Real Habanero", "Win the last Derby cup with the Pepper", CAT_CAREER),
		new CAchievement("WRECK_MP", "First Blood", "Wreck a car in multiplayer", CAT_MULTIPLAYER),
		new CAchievement("BLAST_MP", "Unfriendly Competition", "Get 100 crash bonuses in multiplayer", CAT_MULTIPLAYER),
		new CAchievement("SPEEDRUN_CARNAGE", "Speedrunner", "Gold a Carnage Mode event in less than 1:30", CAT_CARNAGE),
		new CAchievement("JACK_WRECKED", "Jack Benton is Wrecked", "You know what to do.", CAT_SINGLEPLAYER),
		new CAchievement("AUTHOR_MEDAL", "Trackmaster", "Achieve an author score", CAT_SINGLEPLAYER | CAT_CAREER | CAT_CARNAGE),
		new CAchievement("SAUTHOR_MEDAL", "Super Trackmaster", "Achieve a super author score", CAT_CAREER, true),
		new CAchievement("FRANK_WIN_RACE", "True Frank Malcov Award", "Have Frank Malcov win a race", CAT_SINGLEPLAYER),
		new CAchievement("ALL_AWARDS", "Total Domination", "Win a race with all Top Driver awards", CAT_SINGLEPLAYER),
		new CAchievement("DRIFT_RACES", "Burning Rubber", "Play 3 drift events", CAT_GENERAL),
		new CAchievement("KNOCKOUT_RACES", "Volatile Racing", "Win 5 knockout events", CAT_GENERAL),
		new CAchievement("DRIFT_SCORE", "Professional Drifter", "Get 100,000pts in one drift chain", CAT_GENERAL),
		new CAchievement("HIGH_SPEED", "Ludicrous Speed", "Reach a speed of 500KM/H", CAT_GENERAL),
		new CAchievement("BUY_MATCHUP", "Picky Buyer", "Purchase a car's alternate variant", CAT_CAREER),
		new CAchievement("BUY_CUSTOM_SKIN", "Community-Run", "Purchase a car with a custom livery", CAT_CAREER),
		new CAchievement("CHEAT_CAR", "Hidden Assets", "Drive a secret car", CAT_GENERAL),
		new CAchievement("WATER_FLOAT", "Sleep with the fishes!", "Float on water for 10 seconds total", CAT_GENERAL),
		new CAchievement("LOW_HP", "Dead Man Walking", "Win a race on less than 10% health", CAT_GENERAL),
		new CAchievement("RALLY_RAGDOLL", "Samir Award", "Fly through the windshield in a rally", CAT_RALLY),
		new CAchievement("CASH_AWARD", "Makin' it Big", "Reach a total balance of 100,000CR", CAT_CAREER),
		new CAchievement("COMPLETE_CAREER", "Race Master", "Complete FlatOut mode", CAT_CAREER),
		new CAchievement("COMPLETE_CAREER_GOLD", "Race Wizard", "Complete FlatOut mode with all gold", CAT_CAREER),
		new CAchievement("COMPLETE_CARNAGE", "Carnage Veteran", "Complete Carnage Mode", CAT_CARNAGE),
		new CAchievement("COMPLETE_CARNAGE_GOLD", "Carnage Wizard", "Complete Carnage Mode with all gold", CAT_CARNAGE),
		new CAchievement("COMPLETE_CARNAGE_AUTHOR", "Carnage Master", "Complete Carnage Mode with all author", CAT_CARNAGE, true),
		new CAchievement("COMPLETE_RALLY", "Rally Trophy", "Complete Rally Mode", CAT_RALLY),
		new CAchievement("COMPLETE_RALLY_GOLD", "Rally Gold Trophy", "Complete Rally Mode with all gold", CAT_RALLY),
	};

	std::vector<CAchievement*> GetAchievementsInCategory(uint32_t category) {
		std::vector<CAchievement*> achievements;
		for (auto& achievement : gAchievements) {
			if (achievement->bHidden && !achievement->bUnlocked) continue;
			if ((achievement->nCategory & category) != 0) {
				achievements.push_back(achievement);
			}
		}
		return achievements;
	}

	const float fSpriteBGX = 960;
	const float fSpriteBGY = 960;
	const float fSpriteFGX = 1187;
	const float fSpriteFGY = 953;
	const float fSpriteBGSX = 507;
	const float fSpriteFGSX = 202;
	const float fSpriteBGSY = 83;
	const float fSpriteFGSY = 159;
	const float fSpritePopTimerSpeed = 2;
	const float fSpritePopBalloonSize = 1.2;
	const float fSpriteExpandTimerSpeed = 1;
	const float fSpriteStayTimerSpeed = 1.0 / 6.0;
	const float fTextFGX = 740;
	const float fTextFGY = 942;
	const float fTextFGS = 0.03;
	const float fTextBGX = 740;
	const float fTextBGY = 972;
	const float fTextBGS = 0.025;

	double fSpritePopTimer = 0;
	double fSpriteExpandTimer = 0;
	double fSpriteStayTimer = 1;
	double fSpriteHideTimer = 1;
	std::string sTextTitle;
	std::string sTextDesc;

	bool DrawAchievementSprite(float left, float right, float top, float bottom, NyaDrawing::CNyaRGBA32 rgb, TEXTURE_TYPE* texture) {
		left -= 960;
		right -= 960;
		left /= 1920.0;
		right /= 1920.0;
		top /= 1080.0;
		bottom /= 1080.0;

		// aspect correction
		left *= 16.0 / 9.0;
		right *= 16.0 / 9.0;
		left /= GetAspectRatio();
		right /= GetAspectRatio();

		// recenter
		left += 0.5;
		right += 0.5;
		return DrawRectangle(left, right, top, bottom, rgb, 0, texture);
	}

	void DrawAchievementString(tNyaStringData data, const std::string& text) {
		data.x -= 960;
		data.x /= 1920.0;
		data.y /= 1080.0;

		// aspect correction
		data.x *= 16.0 / 9.0;
		data.x /= GetAspectRatio();

		// recenter
		data.x += 0.5;
		return DrawString(data, text, &DrawStringFO2);
	}

	void Save(int saveSlot) {
		auto file = std::ofstream(GetAchievementSavePath(saveSlot), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		for (auto& achievement : gAchievements) {
			if (!achievement->bUnlocked && achievement->fInternalProgress == 0.0) continue;

			WriteStringToFile(file, achievement->sIdentifier);
			file.write((char*)&achievement->fInternalProgress, sizeof(achievement->fInternalProgress));
			file.write((char*)&achievement->bUnlocked, sizeof(achievement->bUnlocked));
		}
	}

	std::vector<CAchievement*> aUnlockBuffer;
	auto gFailAchievement = CAchievement("ERROR", "ERROR", "ERROR", 0);

	CAchievement* GetAchievement(const std::string& identifier) {
		for (auto& achievement : gAchievements) {
			if (achievement->sIdentifier == identifier) return achievement;
		}

		static std::string failDesc;
		failDesc = "Failed to find achievement " + identifier;
		gFailAchievement.sDescription = failDesc.c_str();
		aUnlockBuffer.push_back(&gFailAchievement);
		return nullptr;
	}


	void AwardAchievement(CAchievement* achievement) {
		if (!achievement) return;

		if (achievement->bUnlocked) return;
		achievement->bUnlocked = true;
		Save(nCurrentSaveSlot);

		aUnlockBuffer.push_back(achievement);
	}

	void Load(int saveSlot) {
		for (auto& achievement : gAchievements) {
			achievement->nProgress = 0;
			achievement->fInternalProgress = 0;
			achievement->bUnlocked = false;
		}

		nCurrentSaveSlot = saveSlot;

		auto file = std::ifstream(GetAchievementSavePath(saveSlot), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		auto identifier = ReadStringFromFile(file);
		while (!identifier.empty()) {
			if (auto achievement = GetAchievement(identifier)) {
				file.read((char*)&achievement->fInternalProgress, sizeof(achievement->fInternalProgress));
				file.read((char*)&achievement->bUnlocked, sizeof(achievement->bUnlocked));
			}
			else {
				file.read((char*)&gFailAchievement.fInternalProgress, sizeof(gFailAchievement.fInternalProgress));
				file.read((char*)&gFailAchievement.bUnlocked, sizeof(gFailAchievement.bUnlocked));
			}
			identifier = ReadStringFromFile(file);
		}
	}

	// from easing-functions by nicolausYes
	double easeInOutQuart(double t) {
		if (t < 0.5) {
			t *= t;
			return 8 * t * t;
		} else {
			t = (--t) * t;
			return 1 - 8 * t * t;
		}
	}

	int GetNumUnlockedAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (achievement->bHidden) continue;
			if (achievement->bUnlocked) count++;
		}
		return count;
	}

	int GetNumVisibleAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (achievement->bHidden) continue;
			count++;
		}
		return count;
	}

	void DrawUnlockUI() {
		if (fSpriteHideTimer >= 1 && fSpritePopTimer <= 0) {
			if (aUnlockBuffer.empty()) return;

			auto achievement = aUnlockBuffer[0];
			fSpritePopTimer = 0;
			fSpriteExpandTimer = 0;
			fSpriteStayTimer = 0;
			fSpriteHideTimer = 0;
			sTextTitle = achievement->sName;
			sTextDesc = achievement->sDescription;
			aUnlockBuffer.erase(aUnlockBuffer.begin());

			static auto sound = NyaAudio::LoadFile("data/sound/achievement/unlock.mp3");
			if (sound) {
				NyaAudio::SetVolume(sound, *(int*)0x849550 / 100.0);
				NyaAudio::Play(sound);
			}
		}

		static CNyaTimer timer;
		float delta = 0;
		bool isExpanding = false;
		// stage 4 - remove icon
		if (fSpriteHideTimer >= 1) {
			//if (fSpritePopTimer > 0.5) fSpritePopTimer = 0.5;
			fSpritePopTimer -= timer.Process() * fSpritePopTimerSpeed;
			delta = fSpritePopTimer;
		}
		// stage 3 - hide
		else if (fSpriteStayTimer >= 1) {
			fSpriteHideTimer += timer.Process() * fSpriteExpandTimerSpeed;
			delta = 1.0 - fSpriteHideTimer;
			isExpanding = true;
		}
		// stage 3 - stay for a few seconds
		else if (fSpriteExpandTimer >= 1) {
			fSpriteStayTimer += timer.Process() * fSpriteStayTimerSpeed;
			delta = 1.0;
			isExpanding = true;
		}
		// stage 2 - expand to size
		else if (fSpritePopTimer >= 1) {
			fSpriteExpandTimer += timer.Process() * fSpriteExpandTimerSpeed;
			delta = fSpriteExpandTimer;
			isExpanding = true;
		}
		// stage 1 - pop and deflate
		else {
			fSpritePopTimer += timer.Process() * fSpritePopTimerSpeed;
			delta = fSpritePopTimer;
		}

		static auto bgTex = LoadTexture("data/textures/achievement/bg.png");
		static auto bg2Tex = LoadTexture("data/textures/achievement/flowersponk.png");
		static auto fgTex = LoadTexture("data/textures/achievement/flowers.png");

		if (delta <= 0) delta = 0;
		if (delta >= 1) delta = 1;

		if (isExpanding) {
			float bgLeft = std::lerp(fSpriteBGX + (fSpriteBGSX * 0.5), fSpriteBGX - (fSpriteBGSX * 0.5), easeInOutQuart(delta));
			DrawAchievementSprite(bgLeft, fSpriteBGX + (fSpriteBGSX * 0.5), fSpriteBGY - (fSpriteBGSY * 0.5), fSpriteBGY + (fSpriteBGSY * 0.5), {255,255,255,255}, bgTex);
			float textAlpha = (delta - 0.9) * 10;
			delta = 1;

			// if we've expanded further than the flower's extents, start drawing the background
			if (bgLeft < fSpriteFGX - (fSpriteFGSX * 0.5)) {
				DrawAchievementSprite(fSpriteBGX - (fSpriteBGSX * 0.5), fSpriteBGX + (fSpriteBGSX * 0.5), fSpriteBGY - (fSpriteBGSY * 0.5), fSpriteBGY + (fSpriteBGSY * 0.5), {255,255,255,255}, bg2Tex);
			}

			if (textAlpha > 0) {
				if (fSpriteStayTimer > 2.0 / 3.0 && aUnlockBuffer.empty()) {
					sTextTitle = "Achievement progress";
					sTextDesc = std::format("{}/{}", GetNumUnlockedAchievements(), GetNumVisibleAchievements());
				}

				tNyaStringData data;
				data.a = 255 * textAlpha;
				data.x = fTextFGX;
				data.y = fTextFGY;
				data.size = fTextFGS;
				DrawAchievementString(data, sTextTitle);
				data.x = fTextBGX;
				data.y = fTextBGY;
				data.size = fTextBGS;
				DrawAchievementString(data, sTextDesc);
			}
		}

		float fgSize = 1;
		if (delta <= 0.5) {
			float newDelta = delta * 2;
			fgSize = std::lerp(0, fSpritePopBalloonSize, easeInOutQuart(newDelta));
		}
		else {
			float newDelta = (delta - 0.5) * 2;
			fgSize = std::lerp(fSpritePopBalloonSize, 1, easeInOutQuart(newDelta));
		}
		fgSize *= 0.5;

		DrawAchievementSprite(fSpriteFGX - (fSpriteFGSX * fgSize), fSpriteFGX + (fSpriteFGSX * fgSize), fSpriteFGY - (fSpriteFGSY * fgSize), fSpriteFGY + (fSpriteFGSY * fgSize), {255,255,255,255}, fgTex);
	}

	void OnTick() {
		nTotalProgression = ((double)GetNumUnlockedAchievements() / (double)GetNumVisibleAchievements()) * 100;

		if (auto achievement = GetAchievement("DRIFT_RACES")) {
			achievement->nProgress = (achievement->fInternalProgress / 3.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}
		if (auto achievement = GetAchievement("KNOCKOUT_RACES")) {
			achievement->nProgress = (achievement->fInternalProgress / 5.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}
		if (auto achievement = GetAchievement("COMPLETE_CARNAGE")) {
			achievement->nProgress = (achievement->fInternalProgress / 36.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}
		if (auto achievement = GetAchievement("COMPLETE_CARNAGE_GOLD")) {
			achievement->nProgress = (achievement->fInternalProgress / 36.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}
		if (auto achievement = GetAchievement("COMPLETE_CARNAGE_AUTHOR")) {
			achievement->nProgress = (achievement->fInternalProgress / 36.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}
		if (auto achievement = GetAchievement("WATER_FLOAT")) {
			achievement->nProgress = (achievement->fInternalProgress / 10.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}
		if (auto achievement = GetAchievement("CASH_AWARD")) {
			achievement->fInternalProgress = pGameFlow->Profile.nMoney;
			achievement->nProgress = (achievement->fInternalProgress / 100000.0) * 100;
		}
		if (auto achievement = GetAchievement("LOW_HP")) {
			achievement->nProgress = (achievement->fInternalProgress / 0.9) * 100;
		}
		if (auto achievement = GetAchievement("BLAST_MP")) {
			achievement->nProgress = achievement->fInternalProgress;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}

		if (pLoadingScreen) return;

		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			if (bIsCareerRally && pGameFlow->nRaceState == RACE_STATE_RACING) {
				if (GetPlayer(0)->pCar->nIsRagdolled) {
					AwardAchievement(GetAchievement("RALLY_RAGDOLL"));
				}
			}

			static bool bLastDriftEnded = false;
			if (bIsDriftEvent && pGameFlow->nRaceState >= RACE_STATE_FINISHED) {
				if (!bLastDriftEnded && GetPlayerScore<PlayerScoreArcadeRace>(1)->fScore > 50000) {
					GetAchievement("DRIFT_RACES")->fInternalProgress += 1;
					Save(nCurrentSaveSlot);
				}
			}
			bLastDriftEnded = bIsDriftEvent && pGameFlow->nRaceState >= RACE_STATE_FINISHED;

			static bool bLastKOEnded = false;
			if (bIsLapKnockout && pGameFlow->nRaceState >= RACE_STATE_FINISHED) {
				if (!bLastKOEnded && GetPlayerScore<PlayerScoreRace>(1)->nPosition == 1 && GetPlayerScore<PlayerScoreRace>(1)->bHasFinished) {
					GetAchievement("KNOCKOUT_RACES")->fInternalProgress += 1;
					Save(nCurrentSaveSlot);
				}
			}
			bLastKOEnded = bIsLapKnockout && pGameFlow->nRaceState >= RACE_STATE_FINISHED;

			static bool bLastRaceEnded = false;
			if (IsRaceMode() && !bIsTimeTrial && pGameFlow->nRaceState >= RACE_STATE_FINISHED) {
				if (!bLastRaceEnded && GetPlayerScore<PlayerScoreRace>(1)->nPosition == 1 && !GetPlayerScore<PlayerScoreRace>(1)->bIsDNF) {
					auto achievement = GetAchievement("LOW_HP");
					auto damage = GetPlayer(0)->pCar->fDamage;
					if (damage > achievement->fInternalProgress) achievement->fInternalProgress = damage;
					if (damage >= 0.9) {
						AwardAchievement(GetAchievement("LOW_HP"));
					}
				}
			}
			bLastRaceEnded = IsRaceMode() && !bIsTimeTrial && pGameFlow->nRaceState >= RACE_STATE_FINISHED;

			if (IsRaceMode() && !bIsTimeTrial && pGameFlow->nRaceState == RACE_STATE_FINISHED && pPlayerHost->GetNumPlayers() > 1) {
				auto ply = GetPlayerScore<PlayerScoreRace>(1);
				if (ply->bHasFinished && ply->nPosition == 1) {
					AwardAchievement(GetAchievement("WIN_RACE"));
					if (GetPlayer(0)->nCarId == GetCarDBID(153)) {
						AwardAchievement(GetAchievement("WIN_RACE_BUG"));
					}
					if (bIsInMultiplayer) {
						AwardAchievement(GetAchievement("WIN_MP_RACE"));
					}
					// SP and 8+ players only for the all wreck achievement
					else if (!bIsLapKnockout && pPlayerHost->GetNumPlayers() >= 8) {
						bool anyoneAlive = false;
						for (int i = 1; i < 32; i++) {
							auto ply = GetPlayer(i);
							if (!ply) continue;
							if (!ply->nIsWrecked) anyoneAlive = true;
						}
						if (!anyoneAlive) AwardAchievement(GetAchievement("WIN_RACE_WRECK"));
					}
				}
			}

			if (IsRaceMode() && pGameFlow->nRaceState >= RACE_STATE_RACING) {
				if (!bIsInMultiplayer) {
					if (!bIsLapKnockout && !bIsTimeTrial) {
						if (auto ply = GetPlayerScore<PlayerScoreRace>(2)) {
							if (ply->bIsDNF) {
								AwardAchievement(GetAchievement("JACK_WRECKED"));
							}
						}
					}

					if (auto ply = GetPlayerScore<PlayerScoreRace>(8)) {
						if (ply->bHasFinished && ply->nPosition == 1) {
							AwardAchievement(GetAchievement("FRANK_WIN_RACE"));
						}
					}
				}
			}

			if (pGameFlow->nRaceState == RACE_STATE_RACING) {
				if (auto ply = GetPlayer(0)) {
					if (ply->pCar->GetVelocity()->length() >= 138.8) {
						AwardAchievement(GetAchievement("HIGH_SPEED"));
					}

					if (!bIsInMultiplayer) {
						auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", ply->nCarId).c_str());
						if (table->DoesPropertyExist("CheatCode")) {
							AwardAchievement(GetAchievement("CHEAT_CAR"));
						}
					}
				}
			}
		}

		DrawUnlockUI();
	}
}

Achievements::CAchievement* GetAchievement(const std::string& identifier) {
	return Achievements::GetAchievement(identifier);
}