const int nNumArcadeRacesX = 6;
const int nNumArcadeRacesY = 6;

int nSaveSlot = 999;

class ArcadeRaceStats {
public:
	uint32_t score;
	uint32_t placement; // 255 if locked, 254 if unlocked, 1, 2, 3 otherwise
	uint8_t _8[0xC];
	uint32_t unlockScore;
	uint8_t _18[0x58];
};
static_assert(sizeof(ArcadeRaceStats) == 0x70);

class PlayerProfile {
public:
	uint8_t _0[0x3D0];
	struct {
		ArcadeRaceStats* races;
		uint32_t _4;
	} aArcadeClasses[0];
};

std::string GetCustomSavePath(int id) {
	return std::format("Savegame/customsave{:03}.sav", id);
}

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

	static inline bool bOverrideAllArcadeScores = false;

	tCustomSaveStructure() {
		memset(this,0,sizeof(*this));
	}
	void Load(int saveSlot, bool overrideArcadeScores) {
		// override all scores on the first load since swapping profiles doesn't properly clear it
		if (overrideArcadeScores) bOverrideAllArcadeScores = true;

		memset(this,0,sizeof(*this));
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

	void UpdateArcadeRace(PlayerProfile* profile) {
		bool customSaveModified = false;

		int numClasses = nNumArcadeRacesX;
		int numRaces = nNumArcadeRacesY;
		for (int x = 0; x < numClasses; x++) {
			for (int y = 0; y < numRaces; y++) {
				auto vanillaSave = &profile->aArcadeClasses[x].races[y];
				auto customSave = &aArcadeRaces[x][y];
				if (customSave->score > vanillaSave->score || bOverrideAllArcadeScores) {
					vanillaSave->score = customSave->score;
					vanillaSave->placement = customSave->placement;
				}
				else if (vanillaSave->score > customSave->score) {
					customSave->score = vanillaSave->score;
					customSave->placement = vanillaSave->placement;
					customSaveModified = true;
				}
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