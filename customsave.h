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

int nArcadePlatinumTargets[nNumArcadeRacesX][nNumArcadeRacesY];
bool bUnlockAllArcadeEvents = false;

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
	NUM_PLAYTIME_TYPES
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
};

struct tCustomSaveStructure {
	wchar_t playerName[32];
	bool bWelcomeScreenDisplayed;
	struct {
		uint32_t score;
		uint32_t placement;
	} aArcadeRaces[nNumArcadeRacesX][nNumArcadeRacesY];
	uint32_t numCarsUnlocked;
	uint32_t numCupsPassed;
	uint32_t gameProgress;
	uint8_t _tmp; // old player portrait var
	tCarTuning aCarTunings[256];
	double playtime[NUM_PLAYTIME_TYPES];
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
	uint8_t playerColor;
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
	void SetDefaultPlayerSettings() {
		imperialUnits = gGameRegion == 1;
		ingameMap = 1;
		playerFlag = 0;
		playerFlag2 = 0;
		playerModel = 0;
		playerPortrait = 12; // none
		playerColor = 0;
		hudType = 0;
		highCarCam = 0;
		displaySplits = 1;
		splitType = 1;

		// default the rally cup to none
		nRallyCup = -1;
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
		nArrowColor = playerColor;
		nHUDType = hudType;
		nHighCarCam = highCarCam;
		nDisplaySplits = displaySplits;
		nSplitType = splitType;
	}
	void ReadPlayerSettings() {
		imperialUnits = bImperialUnits;
		ingameMap = bIngameMap;
		playerFlag = nPlayerFlag;
		playerFlag2 = nPlayerFlag2;
		playerModel = nPlayerModel;
		playerPortrait = nAvatarID;
		playerColor = nArrowColor;
		hudType = nHUDType;
		displaySplits = nDisplaySplits;
		splitType = nSplitType;
	}
	void Load(int saveSlot, bool overrideArcadeScores) {
		// override all scores on the first load since swapping profiles doesn't properly clear it
		if (overrideArcadeScores) bOverrideAllArcadeScores = true;

		memset(this,0,sizeof(*this));
		memset(nArcadePlatinumTargets,0,sizeof(nArcadePlatinumTargets));
		SetDefaultPlayerSettings();

		auto file = std::ifstream(GetCustomSavePath(saveSlot), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
		if (playerName[31]) playerName[31] = 0;

		if (!splitsInitialized) {
			displaySplits = 1;
			splitType = 1;
			splitsInitialized = 1;
		}
	}
	void Save() {
		ReadPlayerSettings();

		auto file = std::ofstream(GetCustomSavePath(nSaveSlot), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));
	}

	void CalculateArcadePlacement(PlayerProfile* profile, int x, int y) {
		auto pRace = &profile->aArcadeClasses[x].races[y];
		auto score = pRace->score;
		int placement = 255;
		if (score >= pRace->targetScores[2]) placement = 3;
		if (score >= pRace->targetScores[1]) placement = 2;
		if (score >= pRace->targetScores[0]) placement = 1;
		pRace->placement = aArcadeRaces[x][y].placement = placement;
	}

	void UpdateArcadeRace(PlayerProfile* profile) {
		bool customSaveModified = false;

		int numClasses = nNumArcadeRacesX;
		int numRaces = nNumArcadeRacesY;
		for (int x = 0; x < numClasses; x++) {
			profile->aArcadeClasses[x].numRaces = numRaces;
			for (int y = 0; y < numRaces; y++) {
				auto vanillaSave = &profile->aArcadeClasses[x].races[y];
				auto customSave = &aArcadeRaces[x][y];
				if (customSave->score > vanillaSave->score || bOverrideAllArcadeScores) {
					vanillaSave->score = customSave->score;
				}
				else if (vanillaSave->score > customSave->score) {
					customSave->score = vanillaSave->score;
					customSaveModified = true;
				}
				CalculateArcadePlacement(profile, x, y);
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

void ProcessPlayStats() {
	if (!pGameFlow) return;
	if (pLoadingScreen) return;

	static CNyaTimer gTimer;
	gTimer.Process();

	auto time = gTimer.fDeltaTime;
	gCustomSave.playtime[PLAYTIME_TOTAL] += time;
	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		gCustomSave.playtime[PLAYTIME_INGAME] += time;

		if (!bIsInMultiplayer && pGameFlow->PreRace.nMode != GM_ONLINE_MULTIPLAYER) {
			gCustomSave.playtime[PLAYTIME_INGAME_SINGLEPLAYER] += time;
		}

		switch (pGameFlow->PreRace.nMode) {
			case GM_CAREER:
				gCustomSave.playtime[PLAYTIME_INGAME_CAREER] += time;
				break;
			case GM_ARCADE_CAREER:
				gCustomSave.playtime[PLAYTIME_INGAME_CARNAGE] += time;
				break;
			case GM_SINGLE_RACE:
				gCustomSave.playtime[PLAYTIME_INGAME_SINGLE] += time;
				break;
			// GFWL multiplayer
			case GM_ONLINE_MULTIPLAYER:
				gCustomSave.playtime[PLAYTIME_INGAME_MULTIPLAYER] += time;
				break;
			default:
				break;
		}

		switch (pGameFlow->nDerbyType) {
			case DERBY_LMS:
				gCustomSave.playtime[PLAYTIME_INGAME_LMSDERBY] += time;
				gCustomSave.playtime[PLAYTIME_INGAME_ALLDERBY] += time;
				break;
			case DERBY_WRECKING:
				gCustomSave.playtime[PLAYTIME_INGAME_WRECKINGDERBY] += time;
				gCustomSave.playtime[PLAYTIME_INGAME_ALLDERBY] += time;
				break;
			case DERBY_FRAG:
				gCustomSave.playtime[PLAYTIME_INGAME_FRAGDERBY] += time;
				gCustomSave.playtime[PLAYTIME_INGAME_ALLDERBY] += time;
				break;
			default:
				break;
		}

		if (bIsInMultiplayer) {
			gCustomSave.playtime[PLAYTIME_INGAME_MULTIPLAYER] += time;
		}

		if (bIsTimeTrial) {
			gCustomSave.playtime[PLAYTIME_INGAME_TIMETRIAL] += time;
			gCustomSave.playtime[PLAYTIME_INGAME_ALLRACE] += time;
		}
		else if (bIsStuntMode) {
			gCustomSave.playtime[PLAYTIME_INGAME_STUNTSHOW] += time;
		}
		else if (pGameFlow->nDerbyType == DERBY_NONE) {
			switch (pGameFlow->nGameRules) {
				case GR_DEFAULT:
				case GR_RACE:
					gCustomSave.playtime[PLAYTIME_INGAME_RACE] += time;
					gCustomSave.playtime[PLAYTIME_INGAME_ALLRACE] += time;
					break;
				case GR_PONGRACE:
					gCustomSave.playtime[PLAYTIME_INGAME_PONGRACE] += time;
					gCustomSave.playtime[PLAYTIME_INGAME_ALLRACE] += time;
					break;
				case GR_ARCADE_RACE:
					gCustomSave.playtime[PLAYTIME_INGAME_ARCADERACE] += time;
					gCustomSave.playtime[PLAYTIME_INGAME_ALLRACE] += time;
					break;
				case GR_BEAT_THE_BOMB:
					gCustomSave.playtime[PLAYTIME_INGAME_BEATTHEBOMB] += time;
					gCustomSave.playtime[PLAYTIME_INGAME_ALLRACE] += time;
					break;
				case GR_STUNT:
					gCustomSave.playtime[PLAYTIME_INGAME_STUNT] += time;
					break;
			}
		}
	}
	else if (pGameFlow->nGameState == GAME_STATE_MENU) {
		gCustomSave.playtime[PLAYTIME_MENU] += time;
	}

	static auto lastGameState = pGameFlow->nGameState;
	if (lastGameState == GAME_STATE_RACE && pGameFlow->nGameState == GAME_STATE_MENU) {
		gCustomSave.Save();
	}
	lastGameState = pGameFlow->nGameState;
}