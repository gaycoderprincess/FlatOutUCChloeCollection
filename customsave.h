const int nNumArcadeRacesX = 6;
const int nNumArcadeRacesY = 6;

const int nNumCareerEventsX = 8;
const int nNumCareerEventsY = 16;

const int nNumRallyCareerEventsX = 16;
const int nNumRallyCareerEventsY = 16;

int nSaveSlot = 999;

std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}
std::string GetCheatSavePath(int id) {
	return std::format("Savegame/customsave{:03}.cht", id);
}

std::vector<std::string> aCarCheatsEntered;
std::vector<std::string> aCarCheatsEnteredInSavegame;

int nArcadePlatinumTargets[nNumArcadeRacesX][nNumArcadeRacesY];
bool bPropCarsUnlocked = false;
bool bUnlockAllArcadeEvents = false;
void UpdateTrackmasterAchievements();

struct tCarTuning {
	bool initialized = false;
	float fNitroModifier;
	float fBrakeBias;
	float fEngineModifier;
	float fSuspensionStiffness;
	float tmp[15];
};

enum ePlaytimeType {
	PLAYTIME_TOTAL,
	PLAYTIME_MENU,
	PLAYTIME_INGAME,
	PLAYTIME_INGAME_SINGLEPLAYER,
	PLAYTIME_INGAME_MULTIPLAYER,
	PLAYTIME_INGAME_CAREER,
	PLAYTIME_INGAME_CARNAGE,
	PLAYTIME_INGAME_SINGLE,
	PLAYTIME_INGAME_ALLRACE,
	PLAYTIME_INGAME_RACE,
	PLAYTIME_INGAME_PONGRACE,
	PLAYTIME_INGAME_ARCADERACE,
	PLAYTIME_INGAME_BEATTHEBOMB,
	PLAYTIME_INGAME_ALLDERBY,
	PLAYTIME_INGAME_WRECKINGDERBY,
	PLAYTIME_INGAME_LMSDERBY,
	PLAYTIME_INGAME_FRAGDERBY,
	PLAYTIME_INGAME_STUNT,
	PLAYTIME_INGAME_STUNTSHOW,
	PLAYTIME_INGAME_TIMETRIAL,
	NUM_PLAYTIME_TYPES_OLD,

	PLAYTIME_INGAME_DRIFT = NUM_PLAYTIME_TYPES_OLD,
	PLAYTIME_INGAME_LAPKNOCKOUT,
	PLAYTIME_INGAME_SPEEDTRAP,
	PLAYTIME_INGAME_RALLYMODE,
	NUM_PLAYTIME_TYPES_NEW,
};

const char* aPlaytimeTypeNames[] = {
	"Total",
	"Menus",
	"In-game",
	"Singleplayer",
	"Multiplayer",
	"Career",
	"Carnage Mode",
	"Single Events",
	"All Races",
	"Race",
	"Head-On Race",
	"Carnage Race",
	"Beat the Bomb",
	"All Derbies",
	"Wrecking Derby",
	"Survivor Derby",
	"Deathmatch Derby",
	"All Stunts",
	"Stunt Show",
	"Time Trial",
	"Drift",
	"Knockout",
	"Speedtrap",
	"Rally Mode",
};

struct tCustomSaveStructure {
	wchar_t playerName[32];
	bool bWelcomeScreenDisplayed;
	struct tSavedArcadeRace {
		uint32_t score;
		uint32_t placement;
	} aArcadeRaces[nNumArcadeRacesX][nNumArcadeRacesY];
	uint32_t numCarsUnlocked;
	uint32_t numCupsPassed;
	uint32_t gameProgress;
	uint8_t _tmp; // old player portrait var
	tCarTuning aCarTunings[256];
	double playtimeOld[NUM_PLAYTIME_TYPES_OLD];
	struct {
		uint32_t pbTime;
		uint32_t medal;
	} aCareerEvents[nNumCareerEventsX][nNumCareerEventsY];
	uint8_t imperialUnits;
	uint8_t ingameMap;
	uint8_t playerFlag;
	uint8_t playerFlag2;
	uint8_t playerModel;
	uint8_t playerPortrait;
	uint8_t _tmp2; // old player color var
	uint8_t hudType;
	uint8_t highCarCam;
	uint8_t displaySplits;
	uint8_t splitType;
	uint8_t splitsInitialized;
	struct {
		uint8_t bEventUnlocked;
		uint8_t nEventPosition;
	} aRallyCareer[nNumRallyCareerEventsX][nNumRallyCareerEventsY];
	uint32_t nRallyCash;
	uint8_t nRallyCarId;
	uint8_t nRallyCarSkinId;
	int8_t nRallyClass;
	int8_t nRallyCup;
	uint8_t nRallyCupNextStage;
	uint8_t nRallyCupPoints[32];
	uint8_t nRallyCupStagePosition[16];
	uint8_t nRallyCupStagePoints[16][32];
	uint8_t bRallyClassUnlocked[8];
	int8_t nRallyEvent;
	struct {
		uint8_t bEventUnlocked;
		uint8_t nEventPosition;
	} aRallyCareerEvents[nNumRallyCareerEventsX][nNumRallyCareerEventsY];
	uint8_t playerColor;
	uint64_t playtimeNew[NUM_PLAYTIME_TYPES_NEW];
	struct {
		uint8_t bPurchased : 1;
		uint8_t nSkinId;
		uint32_t nUpgrades[2];
	} aRallyCareerGarage[256];
	uint8_t playerList;
	struct {
		uint16_t car;
		uint16_t level;
	} aArcadeRaceVerify[nNumArcadeRacesX][nNumArcadeRacesY];
	uint32_t numCarsUnlockedNoBonus;
	bool tracksWon[256];

	static inline uint8_t aRallyPlayersByPosition[32];
	static inline uint8_t aRallyPlayerPosition[32];
	static inline bool bOverrideAllArcadeScores = false;

	void CalculateRallyPlayersByPosition() {
		struct tLeaderboardEntry {
			int playerId;
			int score;

			static bool compFunction(tLeaderboardEntry a, tLeaderboardEntry b) {
				if (a.score == b.score) return a.playerId < b.playerId;
				return a.score > b.score;
			}
		};
		std::vector<tLeaderboardEntry> players;

		for (int i = 0; i < 32; i++) {
			players.push_back({i, nRallyCupPoints[i]});
		}
		sort(players.begin(), players.end(), tLeaderboardEntry::compFunction);
		for (int i = 0; i < 32; i++) {
			aRallyPlayersByPosition[i] = players[i].playerId;
			aRallyPlayerPosition[players[i].playerId] = i;
		}
	}

	tCustomSaveStructure() {
		memset(this,0,sizeof(*this));
		SetDefaultPlayerSettings();
	}
	static bool IsTrackValidForStat(int category, int track) {
		static int8_t trackCategory[256] = {};
		if (trackCategory[track] == 0) {
			if (!DoesTrackExist(track)) {
				trackCategory[track] = -1;
				return false;
			}
			if (GetTrackValueNumber(track, "TrackType") != category) return false;
			if (DoesTrackValueExist(track, "CheatCode")) {
				trackCategory[track] = -1;
				return false;
			}
			if (DoesTrackValueExist(track, "IsMultiplayerOnly")) {
				trackCategory[track] = -1;
				return false;
			}
			if (category == TRACKTYPE_RACING && DoesTrackValueExist(track, "IsRallyTrack")) {
				trackCategory[track] = -1;
				return false;
			}
			trackCategory[track] = category;
			return true;
		}
		return trackCategory[track] == category;
	}
	static int GetNumTracksInCategory(int category) {
		static int numTracks[NUM_TRACKTYPES] = {};
		if (numTracks[category] > 0) return numTracks[category];

		int count = 0;
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!IsTrackValidForStat(category, i)) continue;
			count++;
		}
		if (count > 0) numTracks[category] = count;
		return count;
	}
	int GetNumTracksWonOfCategory(int category) {
		int count = 0;
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!IsTrackValidForStat(category, i)) continue;
			if (!tracksWon[i]) continue;
			count++;
		}
		return count;
	}
	void SetDefaultPlayerSettings() {
		imperialUnits = gGameRegion == 1;
		ingameMap = 1;
		playerFlag = 0;
		playerFlag2 = 0;
		playerModel = 0;
		playerPortrait = 12; // none
		hudType = 0;
		highCarCam = 0;
		displaySplits = 1;
		splitType = 1;
		playerColor = 0;
		playerList = 1;

		// default the rally cup to none
		nRallyCup = -1;
		nRallyEvent = -1;
		// unlock first cup of each class
		for (int i = 0; i < 16; i++) {
			aRallyCareer[i][0].bEventUnlocked = 1;
		}
		// unlock novice class
		bRallyClassUnlocked[0] = 1;
	}
	void ApplyPlayerSettings() const {
		bImperialUnits = imperialUnits;
		bIngameMap = ingameMap;
		nPlayerFlag = playerFlag;
		nPlayerFlag2 = playerFlag2;
		nPlayerModel = playerModel;
		nAvatarID = playerPortrait;
		nHUDType = hudType;
		nHighCarCam = highCarCam;
		nDisplaySplits = displaySplits;
		nSplitType = splitType;
		nPlayerColor = playerColor;
		nPlayerListDefaultState = playerList;
		aCarCheatsEntered = aCarCheatsEnteredInSavegame;
		for (auto& cheat : aCarCheatsEntered) {
			if (cheat == "temp350") {
				bPropCarsUnlocked = true;
			}
		}
		Achievements::Load(nSaveSlot);
		UpdateTrackmasterAchievements();
	}
	void ReadPlayerSettings() {
		imperialUnits = bImperialUnits;
		ingameMap = bIngameMap;
		playerFlag = nPlayerFlag;
		playerFlag2 = nPlayerFlag2;
		playerModel = nPlayerModel;
		playerPortrait = nAvatarID;
		playerColor = nPlayerColor;
		hudType = nHUDType;
		highCarCam = nHighCarCam;
		displaySplits = nDisplaySplits;
		splitType = nSplitType;
		playerList = nPlayerListDefaultState;
	}
	void CreateArcadeVerify() {
		for (int x = 0; x < nNumArcadeRacesX; x++) {
			for (int y = 0; y < nNumArcadeRacesY; y++) {
				auto verify = &aArcadeRaceVerify[x][y];
				auto race = &pGameFlow->Profile.aArcadeClasses[x].races[y];
				verify->car = race->nCar;
				verify->level = race->nLevel;
			}
		}
	}
	void WriteArcadeScore(int car, int level, int score, int placement) {
		for (int x = 0; x < nNumArcadeRacesX; x++) {
			for (int y = 0; y < nNumArcadeRacesY; y++) {
				auto race = &pGameFlow->Profile.aArcadeClasses[x].races[y];
				if (race->nLevel != level) continue;
				if (race->nRules != GR_STUNT && race->nCar != car) continue; // don't verify car if it's a stunt event
				aArcadeRaces[x][y].score = score;
				aArcadeRaces[x][y].placement = placement;
			}
		}
	}
	void CheckArcadeVerify() {
		tSavedArcadeRace data[nNumArcadeRacesX][nNumArcadeRacesY];
		memcpy(data, aArcadeRaces, sizeof(data));
		memset(aArcadeRaces, 0, sizeof(aArcadeRaces)); // null all old data so random events don't get overridden

		bool hasData = false;
		for (int x = 0; x < nNumArcadeRacesX; x++) {
			for (int y = 0; y < nNumArcadeRacesY; y++) {
				if (aArcadeRaceVerify[x][y].car != 0) hasData = true;
				if (aArcadeRaceVerify[x][y].level != 0) hasData = true;
			}
		}

		// data from v1.69 as fallback
		if (!hasData) {
			auto verify = aArcadeRaceVerify;
			verify[0][0] = { (uint16_t)GetCarByName("Grinder"), 13 };
			verify[0][1] = { (uint16_t)GetCarByName("FO1 Pepper"), 74 };
			verify[0][2] = { (uint16_t)GetCarByName("Blaster XL"), 35 };
			verify[0][3] = { (uint16_t)GetCarByName("Trailblazer"), 48 };
			verify[0][4] = { (uint16_t)GetCarByName("FO1 Trasher"), 61 };
			verify[0][5] = { (uint16_t)GetCarByName("Canyon"), 19 };
			verify[1][0] = { (uint16_t)GetCarByName("Lancea"), 1 };
			verify[1][1] = { (uint16_t)GetCarByName("Crusader"), 36 };
			verify[1][2] = { (uint16_t)GetCarByName("Bullet"), 15 };
			verify[1][3] = { (uint16_t)GetCarByName("Flatmobile"), 66 };
			verify[1][4] = { (uint16_t)GetCarByName("School Bus"), 8 };
			verify[1][5] = { (uint16_t)GetCarByName("Road King"), 103 };
			verify[2][0] = { (uint16_t)GetCarByName("Nevada"), 33 };
			verify[2][1] = { (uint16_t)GetCarByName("Siboner 6 WT-F"), 7 };
			verify[2][2] = { (uint16_t)GetCarByName("FO1 Slider"), 80 };
			verify[2][3] = { (uint16_t)GetCarByName("Shaker"), 4 };
			verify[2][4] = { (uint16_t)GetCarByName("Insetta"), 42 };
			verify[2][5] = { (uint16_t)GetCarByName("Terrator"), 165 };
			verify[3][0] = { (uint16_t)GetCarByName("FO1 Blade"), 73 };
			verify[3][1] = { (uint16_t)GetCarByName("CTR Sport"), 25 };
			verify[3][2] = { (uint16_t)GetCarByName("Terrator"), 20 };
			verify[3][3] = { (uint16_t)GetCarByName("Lancea"), 159 };
			verify[3][4] = { (uint16_t)GetCarByName("Afterburner"), 49 };
			verify[3][5] = { (uint16_t)GetCarByName("Flatmobile"), 31 };
			verify[4][0] = { (uint16_t)GetCarByName("Nucleon"), 57 };
			verify[4][1] = { (uint16_t)GetCarByName("FO1 Blockhead"), 6 };
			verify[4][2] = { (uint16_t)GetCarByName("CTR Sport"), 78 };
			verify[4][3] = { (uint16_t)GetCarByName("Bullet GT"), 62 };
			verify[4][4] = { (uint16_t)GetCarByName("Bonecracker"), 32 };
			verify[4][5] = { (uint16_t)GetCarByName("Insetta"), 65 };
			verify[5][0] = { (uint16_t)GetCarByName("Blaster XL"), 111 };
			verify[5][1] = { (uint16_t)GetCarByName("FO1 Speedevil"), 69 };
			verify[5][2] = { (uint16_t)GetCarByName("Rocket"), 51 };
			verify[5][3] = { (uint16_t)GetCarByName("Chili Pepper"), 98 };
			verify[5][4] = { (uint16_t)GetCarByName("Road King"), 29 };
			verify[5][5] = { (uint16_t)GetCarByName("Flatmobile"), 10 };
		}

		// write all saved events into any found equivalent in the current version's event list
		for (int x = 0; x < nNumArcadeRacesX; x++) {
			for (int y = 0; y < nNumArcadeRacesY; y++) {
				auto verify = &aArcadeRaceVerify[x][y];
				auto race = &data[x][y];
				WriteArcadeScore(verify->car, verify->level, race->score, race->placement);
			}
		}

		// update verification data afterwards
		CreateArcadeVerify();
	}
	static std::string ReadCheatString(std::ifstream& file) {
		std::string string;
		char value = 0;
		do {
			if (file.eof()) return string;

			file.read(&value, 1);
			if (value) string.push_back(value);
		} while (value);
		return string;
	}
	void Load(int saveSlot, bool overrideArcadeScores) {
		// override all scores on the first load since swapping profiles doesn't properly clear it
		if (overrideArcadeScores) bOverrideAllArcadeScores = true;

		memset(this,0,sizeof(*this));
		memset(nArcadePlatinumTargets,0,sizeof(nArcadePlatinumTargets));
		SetDefaultPlayerSettings();
		aCarCheatsEnteredInSavegame.clear();

		auto file = std::ifstream(GetCustomSavePath(saveSlot), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
		if (playerName[31]) playerName[31] = 0;

		if (!splitsInitialized) {
			displaySplits = 1;
			splitType = 1;
			splitsInitialized = 1;
		}

		CheckArcadeVerify();

		auto cht = std::ifstream(GetCheatSavePath(saveSlot), std::ios::in | std::ios::binary);
		if (!cht.is_open()) return;

		std::string cheatString = ReadCheatString(cht);
		while (!cheatString.empty()) {
			aCarCheatsEnteredInSavegame.push_back(cheatString);
			cheatString = ReadCheatString(cht);
		}
	}
	void Save() {
		ReadPlayerSettings();
		CreateArcadeVerify();

		auto file = std::ofstream(GetCustomSavePath(nSaveSlot), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));

		Achievements::Save(nSaveSlot);

		auto cheatSave = GetCheatSavePath(nSaveSlot);
		if (aCarCheatsEntered.empty()) {
			if (std::filesystem::exists(cheatSave)) {
				std::filesystem::remove(cheatSave);
			}
			return;
		}

		auto cht = std::ofstream(cheatSave, std::ios::out | std::ios::binary);
		if (!cht.is_open()) return;
		for (auto& cheat : aCarCheatsEntered) {
			cht.write(cheat.c_str(), cheat.length()+1);
		}
	}

	void CalculateArcadePlacement(PlayerProfile* profile, int x, int y) {
		auto pRace = &profile->aArcadeClasses[x].races[y];
		auto score = pRace->nScore;
		int placement = 255;
		if (score >= pRace->nGoalScores[2]) placement = 3;
		if (score >= pRace->nGoalScores[1]) placement = 2;
		if (score >= pRace->nGoalScores[0]) placement = 1;
		pRace->nPlacement = aArcadeRaces[x][y].placement = placement;
	}

	void UpdateArcadeRace(PlayerProfile* profile) {
		bool customSaveModified = false;

		auto achievement = GetAchievement("COMPLETE_CARNAGE");
		auto achievementGold = GetAchievement("COMPLETE_CARNAGE_GOLD");
		auto achievementAuthor = GetAchievement("COMPLETE_CARNAGE_AUTHOR");
		achievement->fInternalProgress = 0;
		achievementGold->fInternalProgress = 0;
		achievementAuthor->fInternalProgress = 0;

		int numClasses = nNumArcadeRacesX;
		int numRaces = nNumArcadeRacesY;
		for (int x = 0; x < numClasses; x++) {
			profile->aArcadeClasses[x].numRaces = numRaces;
			for (int y = 0; y < numRaces; y++) {
				auto vanillaSave = &profile->aArcadeClasses[x].races[y];
				auto customSave = &aArcadeRaces[x][y];
				if (customSave->score > vanillaSave->nScore || bOverrideAllArcadeScores) {
					vanillaSave->nScore = customSave->score;
				}
				else if (vanillaSave->nScore > customSave->score) {
					customSave->score = vanillaSave->nScore;
					customSaveModified = true;
				}
				CalculateArcadePlacement(profile, x, y);

				if (customSave->placement == 1) {
					achievementGold->fInternalProgress += 1;
					tracksWon[vanillaSave->nLevel] = true;
				}
				if (customSave->placement >= 1 && customSave->placement <= 3) achievement->fInternalProgress += 1;
				if (nArcadePlatinumTargets[x][y] > 0 && customSave->score >= nArcadePlatinumTargets[x][y]) achievementAuthor->fInternalProgress += 1;
			}
		}

		if (customSaveModified) {
			Save();
		}

		bOverrideAllArcadeScores = false;
	}
} gCustomSave;

void InitCustomSave() {
	// always set playername, required for some mp stuff
	NyaHookLib::Patch<uint8_t>(0x4879E7, 0xEB);
	NyaHookLib::Patch(0x487A2B + 1, &gCustomSave.playerName);
	NyaHookLib::Patch(0x48767E + 1, &gCustomSave.playerName);
}

void UpdateTrackmasterAchievements() {
	struct tTrackTypeAssoc {
		int category;
		const char* achievement;
	};
	tTrackTypeAssoc trackTypes[] = {
			{ TRACKTYPE_FOREST, "TRACKMASTER_FOREST" },
			{ TRACKTYPE_FIELDS, "TRACKMASTER_FIELDS" },
			{ TRACKTYPE_DESERT, "TRACKMASTER_DESERT" },
			{ TRACKTYPE_CANAL, "TRACKMASTER_CANAL" },
			{ TRACKTYPE_CITY, "TRACKMASTER_CITY" },
			{ TRACKTYPE_RACING, "TRACKMASTER_RACE" },
			{ TRACKTYPE_FO1_TOWN, "TRACKMASTER_TOWN" },
			{ TRACKTYPE_FO1_PIT, "TRACKMASTER_PIT" },
			{ TRACKTYPE_FO1_WINTER, "TRACKMASTER_WINTER" },
			{ TRACKTYPE_TOUGHTRUCKS, "TRACKMASTER_TT" },
			{ TRACKTYPE_RALLYTROPHY, "TRACKMASTER_RT" },
			{ TRACKTYPE_EVENT, "TRACKMASTER_EVENT" },
			{ TRACKTYPE_DERBY, "TRACKMASTER_DERBY" },
	};

	for (auto& trackType : trackTypes) {
		int numTracksWon = gCustomSave.GetNumTracksWonOfCategory(trackType.category);
		auto achievement = GetAchievement(trackType.achievement);
		if (achievement->fMaxInternalProgress <= 0) {
			achievement->fMaxInternalProgress = gCustomSave.GetNumTracksInCategory(trackType.category);
		}
		achievement->fInternalProgress = numTracksWon;

		achievement->sTrackString = "";
		if (!achievement->bUnlocked) {
			for (int i = 1; i < GetNumTracks() + 1; i++) {
				if (!gCustomSave.IsTrackValidForStat(trackType.category, i)) continue;
				if (gCustomSave.tracksWon[i]) continue;

				if (!achievement->sTrackString.empty()) achievement->sTrackString += ", ";
				achievement->sTrackString += GetTrackName(i);
			}
			achievement->sTrackString = "Remaining: " + achievement->sTrackString;
			achievement->pTrackFunction = Achievements::OnTrack_GenericString;
		}
	}
}

void ProcessPlayStats() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (!pGameFlow) return;
	if (pLoadingScreen) return;

	// migrate playtime stats from doubles to the new int64s
	for (int i = 0; i < NUM_PLAYTIME_TYPES_OLD; i++) {
		if (gCustomSave.playtimeOld[i] > 0) {
			gCustomSave.playtimeNew[i] = gCustomSave.playtimeOld[i];
			gCustomSave.playtimeOld[i] = 0;
		}
	}

	if (gTimer.fTotalTime > 1) {
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			gCustomSave.playtimeNew[PLAYTIME_INGAME]++;

			if (!bIsInMultiplayer && pGameFlow->PreRace.nMode != GM_ONLINE_MULTIPLAYER) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_SINGLEPLAYER]++;
			}

			switch (pGameFlow->PreRace.nMode) {
				case GM_CAREER:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_CAREER]++;
					break;
				case GM_ARCADE_CAREER:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_CARNAGE]++;
					break;
				case GM_SINGLE_RACE:
					if (bIsCareerRally) {
						gCustomSave.playtimeNew[PLAYTIME_INGAME_RALLYMODE]++;
					}
					else {
						gCustomSave.playtimeNew[PLAYTIME_INGAME_SINGLE]++;
					}
					break;
				// GFWL multiplayer
				case GM_ONLINE_MULTIPLAYER:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_MULTIPLAYER]++;
					break;
				default:
					break;
			}

			switch (pGameFlow->nDerbyType) {
				case DERBY_LMS:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_LMSDERBY]++;
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLDERBY]++;
					break;
				case DERBY_WRECKING:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_WRECKINGDERBY]++;
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLDERBY]++;
					break;
				case DERBY_FRAG:
					gCustomSave.playtimeNew[PLAYTIME_INGAME_FRAGDERBY]++;
					gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLDERBY]++;
					break;
				default:
					break;
			}

			if (bIsInMultiplayer) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_MULTIPLAYER]++;
			}

			if (bIsTimeTrial) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_TIMETRIAL]++;
				gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
			}
			else if (bIsStuntMode) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_STUNTSHOW]++;
			}
			else if (bIsDriftEvent) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_DRIFT]++;
				gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
			}
			else if (bIsLapKnockout) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_LAPKNOCKOUT]++;
				gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
			}
			else if (bIsSpeedtrap) {
				gCustomSave.playtimeNew[PLAYTIME_INGAME_SPEEDTRAP]++;
				gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
			}
			else if (pGameFlow->nDerbyType == DERBY_NONE) {
				switch (pGameFlow->nGameRules) {
					case GR_DEFAULT:
					case GR_RACE:
						gCustomSave.playtimeNew[PLAYTIME_INGAME_RACE]++;
						gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
						break;
					case GR_PONGRACE:
						gCustomSave.playtimeNew[PLAYTIME_INGAME_PONGRACE]++;
						gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
						break;
					case GR_ARCADE_RACE:
						gCustomSave.playtimeNew[PLAYTIME_INGAME_ARCADERACE]++;
						gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
						break;
					case GR_BEAT_THE_BOMB:
						gCustomSave.playtimeNew[PLAYTIME_INGAME_BEATTHEBOMB]++;
						gCustomSave.playtimeNew[PLAYTIME_INGAME_ALLRACE]++;
						break;
					case GR_STUNT:
						gCustomSave.playtimeNew[PLAYTIME_INGAME_STUNT]++;
						break;
				}
			}
		}
		else if (pGameFlow->nGameState == GAME_STATE_MENU) {
			gCustomSave.playtimeNew[PLAYTIME_MENU]++;
		}
		gCustomSave.playtimeNew[PLAYTIME_TOTAL] = gCustomSave.playtimeNew[PLAYTIME_MENU] + gCustomSave.playtimeNew[PLAYTIME_INGAME];

		gTimer.fTotalTime -= 1;
	}

	if (pGameFlow->nGameState == GAME_STATE_RACE && pGameFlow->nRaceState == RACE_STATE_FINISHED && pPlayerHost->GetNumPlayers() > 1) {
		bool changed = false;
		int track = pGameFlow->PreRace.nLevel;

		// time trials should count for this
		if (IsRaceMode()) {
			auto ply = GetPlayerScore<PlayerScoreRace>(1);
			if (ply->bHasFinished) {
				if (ply->nPosition == 1 && !gCustomSave.tracksWon[track]) {
					gCustomSave.tracksWon[track] = true;
					changed = true;
				}
			}
		}
		else if (pGameFlow->nDerbyType != DERBY_NONE) {
			auto ply = GetPlayerScore<PlayerScoreDerby>(1);
			if (ply->bHasFinished) {
				if (ply->nPosition == 1 && !gCustomSave.tracksWon[track]) {
					gCustomSave.tracksWon[track] = true;
					changed = true;
				}
			}
		}

		if (changed) {
			UpdateTrackmasterAchievements();
			gCustomSave.Save();
		}
	}

	static auto lastGameState = pGameFlow->nGameState;
	if (lastGameState == GAME_STATE_RACE && pGameFlow->nGameState == GAME_STATE_MENU) {
		UpdateTrackmasterAchievements();
		gCustomSave.Save();
	}
	lastGameState = pGameFlow->nGameState;
}