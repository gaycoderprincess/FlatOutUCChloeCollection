const int nNumArcadeRacesX = 6;
const int nNumArcadeRacesY = 6;

int nSaveSlot = 999;

std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}

int nArcadePlatinumTargets[nNumArcadeRacesX][nNumArcadeRacesY];

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
	uint8_t playerPortrait;
	bool bArcadePlatinums[nNumArcadeRacesX][nNumArcadeRacesY];

	static inline bool bOverrideAllArcadeScores = false;

	tCustomSaveStructure() {
		memset(this,0,sizeof(*this));
	}
	void Load(int saveSlot, bool overrideArcadeScores) {
		// override all scores on the first load since swapping profiles doesn't properly clear it
		if (overrideArcadeScores) bOverrideAllArcadeScores = true;

		memset(this,0,sizeof(*this));
		memset(nArcadePlatinumTargets,0,sizeof(nArcadePlatinumTargets));
		playerPortrait = 12; // none

		auto file = std::ifstream(GetCustomSavePath(saveSlot), std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
		if (playerName[31]) playerName[31] = 0;
	}
	void Save() {
		auto file = std::ofstream(GetCustomSavePath(nSaveSlot), std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));
	}

	void CalculateArcadePlacement(PlayerProfile* profile, int x, int y) {
		auto pRace = &profile->aArcadeClasses[x].races[y];
		auto score = pRace->score;
		int placement = 255;
		if (score > pRace->targetScores[2]) placement = 3;
		if (score > pRace->targetScores[1]) placement = 2;
		if (score > pRace->targetScores[0]) placement = 1;
		bArcadePlatinums[x][y] = nArcadePlatinumTargets[x][y] > 0 && score > nArcadePlatinumTargets[x][y];
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