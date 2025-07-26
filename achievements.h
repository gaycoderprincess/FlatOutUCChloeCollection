namespace Achievements {
	int nCurrentSaveSlot = 0;

	std::string GetAchievementSavePath(int id) {
		return std::format("Savegame/customsave{:03}.ach", id);
	}

	class CAchievement {
	public:
		const char* sIdentifier;
		const char* sName;
		const char* sDescription;
		int nProgress = 0;
		float fInternalProgress = 0;
		bool bUnlocked = false;
		bool bHidden = false;

		CAchievement() = delete;
		CAchievement(const char* identifier, const char* name, const char* description, bool hidden = false) {
			sIdentifier = identifier;
			sName = name;
			sDescription = description;
			bHidden = hidden;
		}
	};

	std::vector<CAchievement*> gAchievements = {
		new CAchievement("WIN_RACE", "Starting Point", "Win a race"),
		new CAchievement("WIN_MP_RACE", "Friendly Competition", "Win a multiplayer race"),
		new CAchievement("WIN_RALLY_RACE", "Aspiring Rally Driver", "Win a rally cup"),
		new CAchievement("WIN_RACE_WRECK", "Eliminator", "Win a race after wrecking every player"),
		new CAchievement("JACK_WRECKED", "Jack Benton is Wrecked", "You know what to do."),
		new CAchievement("AUTHOR_MEDAL", "Trackmaster", "Achieve an author score"),
		new CAchievement("FRANK_WIN_RACE", "True Frank Malcov Award", "Have Frank Malcov win a race"),
		new CAchievement("ALL_AWARDS", "Clean Sweep", "Get all post-race awards at once"),
		new CAchievement("DRIFT_RACES", "Burning Rubber", "Play 3 drift events"),
		new CAchievement("COMPLETE_RALLY", "Rally Trophy", "Complete the rally mode"),
	};

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
			if (!achievement->bUnlocked && achievement->nProgress == 0) continue;

			WriteStringToFile(file, achievement->sIdentifier);
			file.write((char*)&achievement->fInternalProgress, sizeof(achievement->fInternalProgress));
			file.write((char*)&achievement->bUnlocked, sizeof(achievement->bUnlocked));
		}
	}

	std::vector<CAchievement*> aUnlockBuffer;
	auto gFailAchievement = CAchievement("ERROR", "ERROR", "ERROR");

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

		static auto bgTex = LoadTexture("data/textures/bg.png");
		static auto bg2Tex = LoadTexture("data/textures/flowersponk.png");
		static auto fgTex = LoadTexture("data/textures/flowers.png");

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
		if (auto achievement = GetAchievement("DRIFT_RACES")) {
			achievement->nProgress = (achievement->fInternalProgress / 3.0) * 100;
			if (achievement->nProgress >= 100) {
				AwardAchievement(achievement);
			}
		}

		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			static bool bLastDriftEnded = false;
			if (bIsDriftEvent && pGameFlow->nRaceState == RACE_STATE_FINISHED) {
				if (!bLastDriftEnded && GetPlayerScore<PlayerScoreArcadeRace>(1)->fScore > 50000) {
					GetAchievement("DRIFT_RACES")->fInternalProgress += 1;
				}
			}
			bLastDriftEnded = bIsDriftEvent && pGameFlow->nRaceState == RACE_STATE_FINISHED;

			if (IsRaceMode() && !bIsTimeTrial && pGameFlow->nRaceState == RACE_STATE_FINISHED && pPlayerHost->GetNumPlayers() > 1) {
				auto ply = GetPlayerScore<PlayerScoreRace>(1);
				if (ply->bHasFinished && ply->nPosition == 1) {
					AwardAchievement(GetAchievement("WIN_RACE"));
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
		}

		DrawUnlockUI();
	}
}

Achievements::CAchievement* GetAchievement(const std::string& identifier) {
	return Achievements::GetAchievement(identifier);
}