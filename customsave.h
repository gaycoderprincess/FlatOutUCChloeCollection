const int nNumArcadeRacesX = 6;
const int nNumArcadeRacesY = 6;

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
	uint8_t _0[0x3C8];
	struct {
		ArcadeRaceStats* races;
		uint32_t _4;
	} aArcadeClasses[0];
};

struct tCustomSaveStructure {
	wchar_t playerName[32];
	bool bWelcomeScreenDisplayed;
	struct {
		uint32_t score;
		uint32_t placement;
	} aArcadeRaces[nNumArcadeRacesX][nNumArcadeRacesY];

	tCustomSaveStructure() {
		memset(this,0,sizeof(*this));
	}
	void Load() {
		auto file = std::ifstream("Savegame/customsave.sav", std::ios::in | std::ios::binary);
		if (!file.is_open()) return;

		file.read((char*)this, sizeof(*this));
		if (playerName[31]) playerName[31] = 0;
	}
	void Save() {
		auto savePath = (char*)0x8255B0;
		auto file = std::ofstream(savePath + (std::string)"/Savegame/customsave.sav", std::ios::out | std::ios::binary);
		if (!file.is_open()) return;

		file.write((char*)this, sizeof(*this));
	}

	auto GetArcadeRace(int x, int y) {
		return &aArcadeRaces[x-1][y];
	}
	void UpdateArcadeRace(PlayerProfile* profile) {
		bool customSaveModified = false;

		int numClasses = nNumArcadeRacesX;
		int numRaces = nNumArcadeRacesY;
		for (int x = 1; x < numClasses + 1; x++) {
			for (int y = 0; y < numRaces; y++) {
				auto vanillaSave = &profile->aArcadeClasses[x].races[y];
				auto customSave = GetArcadeRace(x, y);
				if (customSave->score > vanillaSave->score) {
					vanillaSave->score = customSave->score;
					vanillaSave->placement = customSave->placement;
				}
				if (vanillaSave->score > customSave->score) {
					customSave->score = vanillaSave->score;
					customSave->placement = vanillaSave->placement;
					customSaveModified = true;
				}
			}
		}

		if (customSaveModified) {
			Save();
		}
	}
} gCustomSave;

void InitCustomSave() {
	gCustomSave.Load();
	// always set playername, required for some mp stuff
	NyaHookLib::Patch<uint8_t>(0x4879E7, 0xEB);
	NyaHookLib::Patch(0x487A2B + 1, &gCustomSave.playerName);
	// set default name
	if (!gCustomSave.playerName[0] || !wcscmp(gCustomSave.playerName, L"PLAYER")) {
		wcscpy_s(gCustomSave.playerName, 32, L"PLAYER");
		gCustomSave.bWelcomeScreenDisplayed = false;
	}
}