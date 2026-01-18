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
		CAT_GAMEMODES = 64,
		CAT_TRACKS = 128,
		CAT_HIDDEN = 256,
	};

	class CAchievement {
	public:
		const char* sIdentifier;
		const char* sName;
		const char* sDescription;
		int nProgress = 0;
		float fInternalProgress = 0;
		float fMaxInternalProgress = 0;
		bool bUnlocked = false;
		bool bHidden = false;
		uint64_t nCategory = 0;
		void(*pTickFunction)(CAchievement*, double) = nullptr;
		std::string(*pTrackFunction)(CAchievement*) = nullptr;
		std::string sTrackString;

		bool bTracked = false;

		CAchievement() = delete;
		CAchievement(const char* identifier, const char* name, const char* description, uint64_t category, bool hidden = false) {
			sIdentifier = identifier;
			sName = name;
			sDescription = description;
			bHidden = hidden;
			nCategory = category;
		}
	};

	std::vector<CAchievement*> gAchievements = {
		new CAchievement("WIN_RACE", "Starting Point", "Win a race", CAT_GENERAL),
		//new CAchievement("WIN_RACE_PROFESSIONAL", "Simcade Mode", "Win a race with professional handling", CAT_GENERAL),
		new CAchievement("WIN_MP_RACE", "Friendly Competition", "Win a multiplayer race", CAT_MULTIPLAYER),
		new CAchievement("WIN_MP_DERBY", "King of the Kill", "Win a multiplayer derby", CAT_MULTIPLAYER),
		new CAchievement("WIN_RALLY_RACE", "Aspiring Rally Driver", "Win a rally cup", CAT_RALLY),
		new CAchievement("WIN_RACE_WRECK", "Eliminator", "Win a race after wrecking everyone", CAT_SINGLEPLAYER),
		//new CAchievement("WIN_RACE_BUG", "Retro Demo", "Win a race with the Retro Bug", CAT_GENERAL),
		new CAchievement("WIN_DERBY_TRABANT", "Glass Cannon", "Win a derby with the Trabant 601", CAT_GENERAL),
		new CAchievement("WIN_RACE_NODAMAGE", "Not a Scratch", "Win a race without taking any damage", CAT_GENERAL),
		new CAchievement("WIN_RALLY_SAAB", "Boat Award", "Win a rally stage with the Saab 96", CAT_RALLY),
		new CAchievement("WIN_RALLY_SAAB_2", "Hardcore Boat Award", "Win a rally stage with the Saab 96 on Sadistic", CAT_RALLY, true),
		new CAchievement("WIN_CUP_PEPPER", "Real Habanero", "Win the last Derby cup with the Pepper", CAT_CAREER),
		new CAchievement("WRECK_MP", "First Blood", "Wreck a car in multiplayer", CAT_MULTIPLAYER),
		new CAchievement("BLAST_MP", "Unfriendly Competition", "Get 100 crash bonuses in multiplayer", CAT_MULTIPLAYER),
		new CAchievement("BLAST_ALL", "Blast Master", "Get 1000 crash bonuses", CAT_GENERAL),
		new CAchievement("SPEEDRUN_CARNAGE", "Speedrunner", "Gold a Carnage Mode event in less than 1:30", CAT_CARNAGE),
		new CAchievement("JACK_WRECKED", "Jack Benton is Wrecked", "You know what to do.", CAT_SINGLEPLAYER),
		new CAchievement("AUTHOR_MEDAL", "Trackmaster", "Achieve an author score", CAT_CAREER | CAT_CARNAGE),
		new CAchievement("SAUTHOR_MEDAL", "Super Trackmaster", "Achieve a super author score", CAT_CAREER, true),
		new CAchievement("FRANK_WIN_RACE", "True Frank Malcov Award", "Have Frank Malcov win a race", CAT_SINGLEPLAYER),
		new CAchievement("ALL_AWARDS", "Total Domination", "Win a race with all Top Driver awards", CAT_SINGLEPLAYER),
		new CAchievement("DRIFT_RACES", "Burning Rubber", "Play 3 drift events", CAT_GAMEMODES),
		new CAchievement("KNOCKOUT_RACES", "Volatile Racing", "Win 5 knockout events", CAT_GAMEMODES),
		new CAchievement("DRIFT_SCORE", "Professional Drifter", "Get 100,000pts in one drift chain", CAT_GAMEMODES),
		new CAchievement("HIGH_SPEED", "Ludicrous Speed", "Reach a speed of 500KM/H", CAT_GENERAL),
		new CAchievement("BUY_MATCHUP", "Picky Buyer", "Purchase a car's alternate variant", CAT_CAREER),
		new CAchievement("BUY_CUSTOM_SKIN", "Community-Run", "Purchase a car with a custom livery", CAT_CAREER),
		new CAchievement("CHANGE_MUSIC", "Your Own Jukebox", "Change a music playlist", CAT_GENERAL),
		new CAchievement("WATER_FLOAT", "Sleep with the fishes!", "Float on water for 10 seconds total", CAT_GENERAL),
		new CAchievement("LOW_HP", "Dead Man Walking", "Win a race on less than 10%% health", CAT_GENERAL),
		new CAchievement("RALLY_RAGDOLL", "Samir Award", "Fly through the windshield in a rally", CAT_RALLY),
		new CAchievement("CASH_AWARD", "Makin' it Big", "Reach a total balance of 100,000CR", CAT_CAREER),
		new CAchievement("FRAGDERBY_NO_WRECKS", "Rasputin", "Win a Deathmatch Derby without dying", CAT_GAMEMODES),
		new CAchievement("STUNT_4FLIP", "Tony Hawk Style", "Get a 4x Flip or Roll in Stunt Show", CAT_GAMEMODES),
		new CAchievement("CRASHOUT_PEP", "Size Doesn't Matter", "Earn a Crash Out bonus with a Pepper", CAT_GENERAL),
		new CAchievement("ALL_CARS", "Car Collector", "Unlock all cars in the game", CAT_GENERAL),
		new CAchievement("COMPLETE_CAREER", "Race Master", "Complete FlatOut mode", CAT_CAREER),
		new CAchievement("COMPLETE_CAREER_GOLD", "Race Wizard", "Complete FlatOut mode with all gold", CAT_CAREER),
		new CAchievement("COMPLETE_CARNAGE", "Carnage Master", "Complete Carnage Mode", CAT_CARNAGE),
		new CAchievement("COMPLETE_CARNAGE_GOLD", "Carnage Wizard", "Complete Carnage Mode with all gold", CAT_CARNAGE),
		new CAchievement("COMPLETE_CARNAGE_AUTHOR", "Carnage Legend", "Complete Carnage Mode with all author", CAT_CARNAGE, true),
		new CAchievement("COMPLETE_RALLY", "Rally Trophy", "Complete Rally Mode", CAT_RALLY),
		new CAchievement("COMPLETE_RALLY_GOLD", "Rally Gold Trophy", "Complete Rally Mode with all gold", CAT_RALLY),
		new CAchievement("STONESKIPPING_FAR", "Pool to Pool", "Land in the last pool in Stone Skipping", CAT_GAMEMODES, true),
		new CAchievement("FAST_SPEEDTRAP", "Demon Speeding", "Get over 300KM/H in a single speedtrap", CAT_GAMEMODES),
		new CAchievement("BASEBALL_HOMERUN", "Home Run!", "Get a home run in Baseball", CAT_GAMEMODES),
		new CAchievement("BOWLING_STRIKE", "Like an Angel", "Get a strike in Bowling", CAT_GAMEMODES),
		new CAchievement("ROYALFLUSH_FLUSH", "Hit it Big", "Get a flush in Royal Flush", CAT_GAMEMODES),
		new CAchievement("TRACKMASTER_FOREST", "Forest Map Veteran", "Win an event on every Forest track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_FIELDS", "Field Map Veteran", "Win an event on every Field track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_DESERT", "Desert Map Veteran", "Win an event on every Desert track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_CANAL", "Canal Map Veteran", "Win an event on every Canal track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_CITY", "City Map Veteran", "Win an event on every City track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_RACE", "Race Map Veteran", "Win an event on every Race track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_TOWN", "Town Map Veteran", "Win an event on every Town track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_PIT", "Pit Map Veteran", "Win an event on every Pit track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_WINTER", "Winter Map Veteran", "Win an event on every Winter track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_TT", "Stadium Map Veteran", "Win an event on every Tough Trucks track", CAT_TRACKS, true),
		new CAchievement("TRACKMASTER_RT", "Rally Map Veteran", "Win an event on every Rally Trophy track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_EVENT", "Event Map Veteran", "Win an event on every Event track", CAT_TRACKS),
		new CAchievement("TRACKMASTER_DERBY", "Derby Map Veteran", "Win an event on every Derby track", CAT_TRACKS),
		new CAchievement("CHEAT_CAR", "Hidden Assets", "Drive a secret car", CAT_HIDDEN, true),
		new CAchievement("WIN_NFS3", "Hot Pursuit", "Win a race on Atlantica with the Retro Diablo SV", CAT_HIDDEN, true),
		new CAchievement("WIN_REVOLT", "Re-Volter", "Win a race on Toys in the Hood with the Toyeca", CAT_HIDDEN, true),
		new CAchievement("WIN_NFSU2", "URL Racing", "Win a race on Bayview Speedway with the Retro 350Z", CAT_HIDDEN, true),
		new CAchievement("WIN_GT3", "Real Racing Simulator", "Win a race on Laguna Seca with the Evo V", CAT_HIDDEN, true),
		new CAchievement("NEVILLE_WATER", "Out of Soda", "Knock Neville into water", CAT_SINGLEPLAYER),
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
	bool bHiddenAchievementUnlocked = false;

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

	int GetNumUnlockedHiddenAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (!achievement->bHidden) continue;
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

	int GetNumHiddenAchievements() {
		int count = 0;
		for (auto& achievement : gAchievements) {
			if (!achievement->bHidden) continue;
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
			bHiddenAchievementUnlocked = achievement->bHidden;
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
					if (bHiddenAchievementUnlocked) {
						sTextTitle = "Hidden achievement progress";
						sTextDesc = std::format("{}/{}", GetNumUnlockedHiddenAchievements(), GetNumHiddenAchievements());
					}
					else {
						sTextTitle = "Achievement progress";
						sTextDesc = std::format("{}/{}", GetNumUnlockedAchievements(), GetNumVisibleAchievements());
					}
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

	void OnTick_DriftRaces(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			static bool bLast = false;
			bool bCurrent = bIsDriftEvent && pGameFlow->nRaceState >= RACE_STATE_FINISHED;
			if (bCurrent) {
				if (!bLast && GetPlayerScore<PlayerScoreArcadeRace>(1)->fScore > 50000) {
					pThis->fInternalProgress += 1;
					Save(nCurrentSaveSlot);
				}
			}
			bLast = bCurrent;
		}
	}
	void OnTick_KnockoutRaces(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			static bool bLast = false;
			bool bCurrent = bIsLapKnockout && pGameFlow->nRaceState >= RACE_STATE_FINISHED;
			if (bCurrent) {
				if (!bLast && GetPlayerScore<PlayerScoreRace>(1)->nPosition == 1 && GetPlayerScore<PlayerScoreRace>(1)->bHasFinished) {
					pThis->fInternalProgress += 1;
					Save(nCurrentSaveSlot);
				}
			}
			bLast = bCurrent;
		}
	}
	void OnTick_LowHP(CAchievement* pThis, double delta) {
		pThis->nProgress = (pThis->fInternalProgress / 0.9) * 100;

		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			static bool bLast = false;
			bool bCurrent = IsRaceMode() && !bIsTimeTrial && pGameFlow->nRaceState >= RACE_STATE_FINISHED;
			if (bCurrent) {
				if (!bLast && GetPlayerScore<PlayerScoreRace>(1)->nPosition == 1 && !GetPlayerScore<PlayerScoreRace>(1)->bIsDNF) {
					auto damage = GetPlayer(0)->pCar->fDamage;
					if (damage > pThis->fInternalProgress) pThis->fInternalProgress = damage;
					if (damage >= 0.9) {
						AwardAchievement(pThis);
					}
				}
			}
			bLast = bCurrent;
		}
	}
	void OnTick_CashAward(CAchievement* pThis, double delta) {
		pThis->fInternalProgress = pGameFlow->Profile.nMoney;
		pThis->nProgress = (pThis->fInternalProgress / 100000.0) * 100;
	}
	void OnTick_StoneSkippingFar(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nStuntType != STUNT_STONESKIPPING) return;

		// second pool is around 120
		if (pCameraManager->pTarget->GetMatrix()->p.z >= 121) {
			AwardAchievement(pThis);
		}
	}
	void OnTick_RallyRagdoll(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;

		if ((bIsCareerRally || (bIsTimeTrial && DoesTrackValueExist(pGameFlow->PreRace.nLevel, "IsRallyTrack"))) && pGameFlow->nRaceState == RACE_STATE_RACING) {
			if (GetPlayer(0)->pCar->nIsRagdolled) {
				AwardAchievement(pThis);
			}
		}
	}
	void OnTick_FragDerbyNoWrecks(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nDerbyType != DERBY_FRAG) return;

		// reset nIsWrecked flag if a frag derby was restarted
		if (pPlayerHost->nRaceTime < 0) {
			GetPlayer(0)->pCar->nIsWrecked = 0;
		}

		if (pGameFlow->nRaceState == RACE_STATE_FINISHED && pPlayerHost->GetNumPlayers() >= 4) {
			auto ply = GetPlayerScore<PlayerScoreDerby>(1);
			// Car::nIsWrecked isn't reset after respawning
			if (ply->nPosition == 1 && ply->bHasFinished && !GetPlayer(0)->pCar->nIsWrecked) {
				AwardAchievement(pThis);
			}
		}
	}
	void OnTick_HighSpeed(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nRaceState != RACE_STATE_RACING) return;

		if (auto ply = GetPlayer(0)) {
			if (ply->pCar->GetVelocity()->length() >= 138.8) {
				AwardAchievement(pThis);
			}
		}
	}
	void OnTick_CheatCar(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nRaceState != RACE_STATE_RACING) return;
		if (bIsInMultiplayer) return;

		auto ply = GetPlayer(0);
		auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", ply->nCarId).c_str());
		if (table->DoesPropertyExist("CheatCode")) {
			AwardAchievement(pThis);
		}
	}
	void OnTick_JackWrecked(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (!IsRaceMode()) return;
		if (bIsInMultiplayer || bIsLapKnockout || bIsTimeTrial) return;
		if (pGameFlow->nRaceState < RACE_STATE_RACING) return;

		if (auto ply = GetPlayerScore<PlayerScoreRace>(2)) {
			if (ply->bIsDNF) {
				AwardAchievement(pThis);
			}
		}
	}
	void OnTick_FrankWinRace(CAchievement* pThis, double delta) {
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (!IsRaceMode()) return;
		if (bIsInMultiplayer || bIsLapKnockout || bIsTimeTrial) return;
		if (pGameFlow->nRaceState < RACE_STATE_RACING) return;

		if (auto ply = GetPlayerScore<PlayerScoreRace>(8)) {
			if (ply->bHasFinished && ply->nPosition == 1) {
				AwardAchievement(pThis);
			}
		}
	}
	std::string OnTrack_GenericProgress(CAchievement* pThis) {
		return std::format("Progress: {:.0f}/{}", pThis->fInternalProgress, pThis->fMaxInternalProgress);
	}
	std::string OnTrack_GenericString(CAchievement* pThis) {
		return pThis->sTrackString;
	}
	std::string OnTrack_LowHP(CAchievement* pThis) {
		if (pGameFlow->nGameState == GAME_STATE_RACE && IsRaceMode() && !bIsTimeTrial) {
			return std::format("Health: {:.0f}%%", (1.0 - GetPlayer(0)->pCar->fDamage) * 100);
		}
		return "Health: N/A";
	}
	std::string OnTrack_SpeedrunCarnage(CAchievement* pThis) {
		if (pGameFlow->nGameState == GAME_STATE_RACE && pGameFlow->PreRace.nMode == GM_ARCADE_CAREER) {
			if (pGameFlow->nGameRules == GR_ARCADE_RACE || pGameFlow->nGameRules == GR_BEAT_THE_BOMB || pGameFlow->nDerbyType == DERBY_FRAG) {
				std::string timestr = GetTimeFromMilliseconds(90000 - pPlayerHost->nRaceTime, true);
				timestr.pop_back(); // remove trailing 0, the game has a tickrate of 100fps
				return std::format("Time Left: {}", timestr);
			}
		}
		return "Time Left: N/A";
	}
	std::string OnTrack_DriftScore(CAchievement* pThis) {
		return "Current Chain: N/A";
	}

	const float fTrackPosX = 0.04;
	const float fTrackPosY = 0.04;
	const float fTrackSize = 0.02;
	const float fTrackSpacing = 0.03;

	void DrawTrackUI() {
		tNyaStringData data;
		data.x = fTrackPosX * GetAspectRatioInv();
		data.y = fTrackPosY;
		data.size = fTrackSize;

		for (auto achievement: gAchievements) {
			if (!achievement->bTracked) continue;
			if (!achievement->pTrackFunction) continue;
			if (achievement->bUnlocked) continue;
			auto string = achievement->pTrackFunction(achievement);
			if (string.empty()) continue;
			DrawString(data, achievement->sName, &DrawStringFO2);
			data.y += fTrackSpacing;
			DrawString(data, achievement->sDescription, &DrawStringFO2);
			data.y += fTrackSpacing;
			DrawString(data, string, &DrawStringFO2);
			data.y += fTrackSpacing * 2;
		}
	}

	void OnTick() {
		nTotalProgression = ((double)GetNumUnlockedAchievements() / (double)GetNumVisibleAchievements()) * 100;

		static CNyaTimer gTimer;
		gTimer.Process();

		for (auto achievement: gAchievements) {
			if (!pLoadingScreen && achievement->pTickFunction) {
				achievement->pTickFunction(achievement, gTimer.fDeltaTime);
			}

			if (achievement->fMaxInternalProgress > 0) {
				achievement->nProgress = (achievement->fInternalProgress / achievement->fMaxInternalProgress) * 100;
				if (achievement->fInternalProgress >= (achievement->fMaxInternalProgress - 0.001)) {
					AwardAchievement(achievement);
				}
			}
		}

		if (pLoadingScreen) return;

		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			if (IsRaceMode() && !bIsTimeTrial && pGameFlow->nRaceState == RACE_STATE_FINISHED && pPlayerHost->GetNumPlayers() > 1) {
				auto ply = GetPlayerScore<PlayerScoreRace>(1);
				if (ply->bHasFinished && ply->nPosition == 1) {
					AwardAchievement(GetAchievement("WIN_RACE"));
					int level = pGameFlow->PreRace.nLevel;
					int car = GetPlayer(0)->nCarId;
					if (level == TRACK_SECRET1 && car == GetCarDBID(342)) {
						AwardAchievement(GetAchievement("WIN_REVOLT"));
					}
					if (level == TRACK_SECRET2 && car == GetCarDBID(348)) {
						AwardAchievement(GetAchievement("WIN_GT3"));
					}
					if (level == TRACK_SECRET3 && car == GetCarDBID(347)) {
						AwardAchievement(GetAchievement("WIN_NFS3"));
					}
					if ((level >= TRACK_URL1A && level <= TRACK_URL1F) && car == GetCarDBID(352)) {
						AwardAchievement(GetAchievement("WIN_NFSU2"));
					}
					if (GetPlayer(0)->pCar->fDamage <= 0.0) {
						AwardAchievement(GetAchievement("WIN_RACE_NODAMAGE"));
					}
					//if ((bIsInMultiplayer && nMultiplayerHandlingMode == HANDLING_PROFESSIONAL) || (!bIsInMultiplayer && nHandlingMode == HANDLING_PROFESSIONAL)) {
					//	AwardAchievement(GetAchievement("WIN_RACE_PROFESSIONAL"));
					//}
					//if (GetPlayer(0)->nCarId == GetCarDBID(153)) {
					//	AwardAchievement(GetAchievement("WIN_RACE_BUG"));
					//}
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

			if (pGameFlow->nDerbyType != DERBY_NONE && pGameFlow->nRaceState == RACE_STATE_FINISHED && pPlayerHost->GetNumPlayers() > 1) {
				auto ply = GetPlayerScore<PlayerScoreRace>(1);
				if (ply->bHasFinished && ply->nPosition == 1) {
					if (bIsInMultiplayer) {
						AwardAchievement(GetAchievement("WIN_MP_DERBY"));
					}
					if (GetPlayer(0)->nCarId == GetCarDBID(365)) {
						AwardAchievement(GetAchievement("WIN_DERBY_TRABANT"));
					}
				}
			}
		}

		DrawTrackUI();
		DrawUnlockUI();
	}

	void OnHomeRun() {
		AwardAchievement(GetAchievement("BASEBALL_HOMERUN"));
	}

	uintptr_t OnHomeRunASM_jmp = 0x4952E0;
	void __attribute__((naked)) OnHomeRunASM() {
		__asm__ (
			"pushad\n\t"
			"call %1\n\t"
			"popad\n\t"
			"mov ecx, 0x813\n\t"
			"jmp %0\n\t"
				:
				:  "m" (OnHomeRunASM_jmp), "i" (OnHomeRun)
		);
	}

	void OnStrike() {
		AwardAchievement(GetAchievement("BOWLING_STRIKE"));
	}

	uintptr_t OnStrikeASM_jmp = 0x49A873;
	void __attribute__((naked)) OnStrikeASM() {
		__asm__ (
			"pushad\n\t"
			"call %1\n\t"
			"popad\n\t"
			"mov ecx, 0x852\n\t"
			"jmp %0\n\t"
				:
				:  "m" (OnStrikeASM_jmp), "i" (OnStrike)
		);
	}

	void OnRoyalFlush() {
		AwardAchievement(GetAchievement("ROYALFLUSH_FLUSH"));
	}

	uintptr_t OnRoyalFlushASM_jmp = 0x4E2F16;
	void __attribute__((naked)) OnRoyalFlushASM() {
		__asm__ (
			"pushad\n\t"
			"call %1\n\t"
			"popad\n\t"
			"mov ecx, 0x871\n\t"
			"jmp %0\n\t"
				:
				:  "m" (OnRoyalFlushASM_jmp), "i" (OnRoyalFlush)
		);
	}

	void Init() {
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4952DB, &OnHomeRunASM);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x49A86E, &OnStrikeASM);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4E2F5F, &OnRoyalFlushASM);

		GetAchievement("DRIFT_RACES")->pTickFunction = OnTick_DriftRaces;
		GetAchievement("KNOCKOUT_RACES")->pTickFunction = OnTick_KnockoutRaces;
		GetAchievement("LOW_HP")->pTickFunction = OnTick_LowHP;
		GetAchievement("CASH_AWARD")->pTickFunction = OnTick_CashAward;
		GetAchievement("STONESKIPPING_FAR")->pTickFunction = OnTick_StoneSkippingFar;
		GetAchievement("RALLY_RAGDOLL")->pTickFunction = OnTick_RallyRagdoll;
		GetAchievement("FRAGDERBY_NO_WRECKS")->pTickFunction = OnTick_FragDerbyNoWrecks;
		GetAchievement("HIGH_SPEED")->pTickFunction = OnTick_HighSpeed;
		GetAchievement("CHEAT_CAR")->pTickFunction = OnTick_CheatCar;
		GetAchievement("JACK_WRECKED")->pTickFunction = OnTick_JackWrecked;
		GetAchievement("FRANK_WIN_RACE")->pTickFunction = OnTick_FrankWinRace;

		GetAchievement("LOW_HP")->pTrackFunction = OnTrack_LowHP;
		GetAchievement("SPEEDRUN_CARNAGE")->pTrackFunction = OnTrack_SpeedrunCarnage;
		GetAchievement("DRIFT_SCORE")->pTrackFunction = OnTrack_DriftScore;
		GetAchievement("DRIFT_RACES")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("KNOCKOUT_RACES")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("COMPLETE_CARNAGE")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("COMPLETE_CARNAGE_GOLD")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("COMPLETE_CARNAGE_AUTHOR")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("WATER_FLOAT")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("BLAST_MP")->pTrackFunction = OnTrack_GenericProgress;
		GetAchievement("BLAST_ALL")->pTrackFunction = OnTrack_GenericProgress;

		GetAchievement("DRIFT_RACES")->fMaxInternalProgress = 3;
		GetAchievement("KNOCKOUT_RACES")->fMaxInternalProgress = 5;
		GetAchievement("COMPLETE_CARNAGE")->fMaxInternalProgress = 36;
		GetAchievement("COMPLETE_CARNAGE_GOLD")->fMaxInternalProgress = 36;
		GetAchievement("COMPLETE_CARNAGE_AUTHOR")->fMaxInternalProgress = 36;
		GetAchievement("WATER_FLOAT")->fMaxInternalProgress = 10;
		GetAchievement("BLAST_MP")->fMaxInternalProgress = 100;
		GetAchievement("BLAST_ALL")->fMaxInternalProgress = 1000;
	}
}

Achievements::CAchievement* GetAchievement(const std::string& identifier) {
	return Achievements::GetAchievement(identifier);
}