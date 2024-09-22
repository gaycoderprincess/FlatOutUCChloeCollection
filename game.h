enum eTrack {
	TRACK_FOREST1A			= 1,
	TRACK_FOREST1B			= 2,
	TRACK_FOREST1C			= 3,
	TRACK_FOREST2A			= 4,
	TRACK_FOREST2B			= 5,
	TRACK_FOREST2C			= 6,
	TRACK_CITY1A			= 10,
	TRACK_CITY1B			= 11,
	TRACK_CITY1C			= 12,
	TRACK_CITY2A			= 13,
	TRACK_CITY2B			= 14,
	TRACK_CITY2C			= 15,
	TRACK_CANAL1A			= 16,
	TRACK_CANAL1B			= 17,
	TRACK_CANAL1C			= 18,
	TRACK_DESERT1A			= 22,
	TRACK_DESERT1B			= 23,
	TRACK_DESERT1C			= 24,
	TRACK_FIELDS1A			= 28,
	TRACK_FIELDS1B			= 29,
	TRACK_FIELDS1C			= 30,
	TRACK_FIELDS2A			= 31,
	TRACK_FIELDS2B			= 32,
	TRACK_FIELDS2C			= 33,
	TRACK_RACING1A			= 40,
	TRACK_RACING1B			= 41,
	TRACK_RACING1C			= 42,
	TRACK_RACING2A			= 43,
	TRACK_RACING2B			= 44,
	TRACK_RACING2C			= 45,
	TRACK_DERBY1A			= 52,
	TRACK_DERBY2A			= 53,
	TRACK_DERBY3A			= 54,
	TRACK_DERBY4A			= 55,
	TRACK_DERBY5A			= 56,
	TRACK_DERBY6A			= 57,
	TRACK_TEST				= 58,
	TRACK_ARENA1			= 59,
	TRACK_ARENA2			= 60,
	TRACK_ARENA3			= 61,
	TRACK_ARENA4			= 62,
	TRACK_ARENA5			= 63,
	TRACK_ARENA6			= 64,
	TRACK_NASCAR1A			= 65,
	TRACK_NASCAR1B			= 66,
	TRACK_NASCAR1C			= 67,
	TRACK_GARAGETESTTRACK 	= 68,
	TRACK_CURLING			= 73,
	TRACK_HIGHJUMP			= 70,
	TRACK_STONESKIPPING 	= 74,
	TRACK_RINGOFFIRE		= 75,
	TRACK_DARTS				= 83,
	TRACK_ROYALFLUSH		= 81,
	TRACK_SKIJUMP			= 72,
	TRACK_FIELDGOAL			= 80,
	TRACK_BOWLING			= 71,
	TRACK_BASKETBALL		= 82,
	TRACK_BASEBALL			= 84,
	TRACK_SOCCER			= 85,
	TRACK_FO1TOWN2A			= 86,
	TRACK_FO1TOWN2B			= 87,
	TRACK_FO1TOWN2C			= 88,
	TRACK_FO1PIT1A			= 89,
	TRACK_FO1PIT1B			= 90,
	TRACK_FO1PIT1C			= 91,
	TRACK_FO1PIT2A			= 92,
	TRACK_FO1PIT2B			= 93,
	TRACK_FO1PIT2C			= 94,
	TRACK_FO1PIT3A			= 95,
	TRACK_FO1PIT3B			= 96,
	TRACK_FO1PIT3C			= 97,
	TRACK_FO1TOWN3A			= 98,
	TRACK_FO1TOWN3B			= 99,
	TRACK_FO1TOWN3C			= 100,
	TRACK_FO1FOREST1A		= 101,
	TRACK_FO1FOREST1B		= 102,
	TRACK_FO1FOREST1C		= 103,
	TRACK_FO1FOREST2A		= 104,
	TRACK_FO1FOREST2B		= 105,
	TRACK_FO1FOREST2C		= 106,
	TRACK_FO1FOREST3A		= 107,
	TRACK_FO1FOREST3B		= 108,
	TRACK_FO1FOREST3C		= 109,
	TRACK_FO1WINTER1A		= 110,
	TRACK_FO1WINTER1B		= 111,
	TRACK_FO1WINTER1C		= 112,
	TRACK_FO1WINTER2A		= 113,
	TRACK_FO1WINTER2B		= 114,
	TRACK_FO1WINTER2C		= 115,
	TRACK_FO1RACING1A		= 116,
	TRACK_FO1RACING1B		= 117,
	TRACK_FO1RACING1C		= 118,
	TRACK_FO1RACING2A		= 119,
	TRACK_FO1RACING2B		= 120,
	TRACK_FO1RACING2C		= 121,
	TRACK_RETRODEMO1A		= 122,
	TRACK_RETRODEMO1B		= 123,
	TRACK_RETRODEMO2A		= 124,
	TRACK_RETRODEMO2B		= 125,
	TRACK_TOUGHTRUCKS1		= 126,
	TRACK_TOUGHTRUCKS2		= 127,
	TRACK_TOUGHTRUCKS3		= 128,
	TRACK_TOUGHTRUCKS4		= 129,
	TRACK_TOUGHTRUCKS5		= 130,
	TRACK_TOUGHTRUCKS6		= 131,
	TRACK_TOUGHTRUCKS7		= 132,
	TRACK_TOUGHTRUCKS8		= 133,
	TRACK_TOUGHTRUCKS9		= 134,
	TRACK_TOUGHTRUCKS10		= 135,
	TRACK_TOUGHTRUCKS11		= 136,
	TRACK_TOUGHTRUCKS12		= 137,
	TRACK_TOUGHTRUCKS13		= 138,
	TRACK_TOUGHTRUCKS14		= 139,
	TRACK_TOUGHTRUCKS15		= 140,
	TRACK_TOUGHTRUCKS16		= 141,
	TRACK_TOUGHTRUCKS17		= 142,
	TRACK_TOUGHTRUCKS18		= 143,
	TRACK_TOUGHTRUCKS19		= 144,
	TRACK_TOUGHTRUCKS20		= 145,
	TRACK_TOUGHTRUCKS21		= 146,
	TRACK_TOUGHTRUCKS22		= 147,
};

enum eCrashBonusType {
	INGAME_CRASHBONUS_CRASHFLYBY = 908,
	INGAME_CRASHBONUS_SUPERFLIP = 909,
	INGAME_CRASHBONUS_SLAM = 910,
	INGAME_CRASHBONUS_POWERHIT = 911,
	INGAME_CRASHBONUS_BLASTOUT = 912,
	INGAME_CRASHBONUS_RAGDOLLED = 913,
	INGAME_CRASHBONUS_WRECKED = 914,
};

enum eGameMode {
	GM_NONE = 0,
	GM_CAREER = 1,
	GM_SINGLE_RACE = 2,
	GM_INSTANT_ACTION = 3,
	GM_CRASHTEST_DUMMY = 4,
	GM_ONLINE_MULTIPLAYER = 5,
	GM_PARTY = 6,
	GM_DEVELOPER = 7,
	GM_TOURNAMENT = 8,
	GM_SPLITSCREEN = 9,
	GM_TEST = 10,
	GM_ARCADE_CAREER = 11,
};

enum eGameRules {
	GR_DEFAULT = 0,
	GR_RACE = 1,
	GR_STUNT = 2,
	GR_DERBY = 3,
	GR_HUNTER_PREY = 4,
	GR_ARCADE_ADVENTURE = 5,
	GR_PONGRACE = 6,
	GR_TIMEATTACK = 7,
	GR_BEAT_THE_BOMB = 8,
	GR_ARCADE_RACE = 9,
	GR_TIME_TRIAL = 10,
	GR_TEST = 11,
	GR_TONYHAWK = 12,
};

enum eDerbyType {
	DERBY_NONE = 0,
	DERBY_LMS = 1,
	DERBY_WRECKING = 2,
	DERBY_FRAG = 3,
};

enum eDBValueType {
	DBVALUE_CHAR = 1,
	DBVALUE_STRING = 2,
	DBVALUE_BOOL = 5,
	DBVALUE_INT = 6,
	DBVALUE_FLOAT = 7,
	DBVALUE_RGBA = 8,
	DBVALUE_VECTOR2 = 9,
	DBVALUE_VECTOR3 = 10,
	DBVALUE_VECTOR4 = 11,
	DBVALUE_NODE = 12,
	DBVALUE_MAX_COUNT
};

class GameController {
public:

	virtual void _vf0() = 0;
	virtual void Initialize() = 0;
	virtual void _vf2() = 0;
	virtual void _vf3() = 0;
	virtual void _vf4() = 0;
	virtual void _vf5() = 0;
	virtual void _vf6() = 0;
	virtual void _vf7() = 0;
	virtual void _vf8() = 0;
	virtual void _vf9() = 0;
	virtual void _vf10() = 0;
	virtual void _vf11() = 0;
	virtual void _vf12() = 0;
	virtual void SetControllerLayout(int id) = 0;
};

class LiteDb {
public:

	virtual void _vf0() = 0;
	virtual void _vf1() = 0;
	virtual void _vf2() = 0;
	virtual void _vf3() = 0;
	virtual void _vf4() = 0;
	virtual void _vf5() = 0;
	virtual void _vf6() = 0;
	virtual void _vf7() = 0;
	virtual void _vf8() = 0;
	virtual void _vf9() = 0;
	virtual LiteDb* GetTable(const char* name) = 0;
	virtual void _vf11() = 0;
	virtual void _vf12() = 0;
	virtual void _vf13() = 0;
	virtual void _vf14() = 0;
	virtual void _vf15() = 0;
	virtual void _vf16() = 0;
	virtual void _vf17() = 0;
	virtual void _vf18() = 0;
	virtual void _vf19() = 0;
	virtual void _vf20() = 0;
	virtual void _vf21() = 0;
	virtual void _vf22() = 0;
	virtual void _vf23() = 0;
	virtual void _vf24() = 0;
	virtual void _vf25() = 0;
	virtual void _vf26() = 0;
	virtual void _vf27() = 0;
	virtual void _vf28() = 0;
	virtual void _vf29() = 0;
	virtual void _vf30() = 0;
	virtual void _vf31() = 0;
	virtual void _vf32() = 0;
	virtual void _vf33() = 0;
	virtual void _vf34() = 0;
	virtual void* GetPropertyPointer(const char* name) = 0;
	virtual int GetPropertyAsBool(const char* name, int offset) = 0;
	virtual int GetPropertyAsInt(const char* name, int offset) = 0;
	virtual char GetPropertyAsChar(const char* name, int offset) = 0;
	virtual float GetPropertyAsFloat(const char* name, int offset) = 0;
	virtual uint32_t GetPropertyAsRGBA(const char* name, int offset) = 0;
	virtual void* GetPropertyAsVector2(void* out, const char* name, int offset) = 0;
	virtual void* GetPropertyAsVector3(void* out, const char* name, int offset) = 0;
	virtual void* GetPropertyAsVector4(void* out, const char* name, int offset) = 0;
	virtual LiteDb* GetPropertyAsNode(void* out, const char* name, int offset) = 0;
	virtual LiteDb* GetPropertyAsNodePtr(const char* name, int offset) = 0;
	virtual const char* GetPropertyAsString(const char* name) = 0;
};
auto GetLiteDB = (LiteDb*(*)())0x5A5EB0;

class Tire {
public:
	uint8_t _0[0x344];
	uint32_t bOnGround; // +344
	uint8_t _348[0x68];
};
static_assert(sizeof(Tire) == 0x3B0);

class Player;
class Car {
public:
	uint8_t _0[0x1C0];
	float mMatrix[4*4]; // +1C0
	uint8_t _200[0x80];
	float qQuaternion[4]; // +280
	float vVelocity[3]; // +290
	uint8_t _29C[0x4];
	float vAngVelocity[3]; // +2A0
	uint8_t _2AC[0x340];
	float fNitro; // +5EC
	uint8_t _5F0[0x440];
	Tire aTires[4]; // +A30
	uint8_t _18F0[0x3D8];
	uint32_t nIsSkinCharred; // +1CC8
	uint8_t _1CCC[0x74];
	float fMass; // +1D40
	uint8_t _1D44[0x54];
	float vDriverLoc[3]; // +1D98
	uint8_t _1DA4[0x16C];
	float fGasPedal; // +1F10
	float fBrakePedal; // +1F14
	float fNitroButton; // +1F18
	float fHandbrake; // +1F1C
	float fSteerAngle; // +1F20
	uint8_t _1F24[0x166C];
	int nIsRagdolled; // +3590
	uint8_t _3594[0x1108];
	Player* pPlayer; // +469C
	uint8_t _46A0[0x3418];
	float fDamage; // +7AB8
	uint8_t _7ABC[0xC];
	uint32_t nIsWrecked; // +7AC8

	static inline auto CalculatePerformance = (void(*)(float* out, float torque, float torqueRPM, float power, float powerRPM, float zeroRPM, float redlineRPM))0x45C750;
};

enum ePlayerType {
	PLAYER_TYPE_NONE = 0,
	PLAYER_TYPE_LOCAL = 1,
	PLAYER_TYPE_NETWORK = 2,
	PLAYER_TYPE_AI = 4,
};

class Player {
public:
	uint8_t _4[0x290];
	Car* pCar; // +294
	uint32_t nCarId; // +298
	uint8_t _29C[0x8];
	uint32_t nPlayerModel; // +2A4
	uint8_t _2A8[0x1C];
	uint32_t nPlayerId; // +2C4
	uint32_t nPlayerType; // +2C8
	uint8_t _2CC[0x174];
	uint32_t nGhosting; // +440
	uint8_t _444[0x78];
	uint32_t nTimeInAir; // +4BC
	uint8_t _4C0[0x3E0];
	uint32_t nSteeringKeyboardLeft; // +8A0
	uint32_t nSteeringKeyboardRight; // +8A4
	uint8_t _8A8[0x14];
	uint32_t nIsUsingKeyboard; // +8BC
	uint8_t _8C0[0x8];
	float fSteeringController; // +8C8
	float fOutputSteerAngle; // +8CC
	float fGasPedal; // +8D0
	float fBrakePedal; // +8D4
	uint8_t _8D8[0x28];
	float fTimeInAirForBonus; // +900
	uint8_t _904[0xD8];
	float fLookAheadMin; // +9DC
	float fLookAheadMax; // +9E0
	float fLookAheadModifier; // +9E4

	virtual void _vf0() = 0;
	virtual void _vf1() = 0;
	virtual void _vf2() = 0;
	virtual void _vf3() = 0;
	virtual void _vf4() = 0;
	virtual void _vf5() = 0;
	virtual void _vf6() = 0;
	virtual void _vf7() = 0;
	virtual void _vf8() = 0;
	virtual void _vf9() = 0;
	virtual void _vf10() = 0;
	virtual void _vf11() = 0;
	virtual void _vf12() = 0;
	virtual void _vf13() = 0;
	virtual void _vf14() = 0;
	virtual void _vf15() = 0;
	virtual void _vf16() = 0;
	virtual void _vf17() = 0;
	virtual void TriggerEvent(int* properties) = 0;
};

class PlayerScoreArcadeRace {
public:
	uint8_t _0[0x4];
	uint32_t nPlayerId; // +4
	uint32_t nUnknownScoringRelated; // +8
	uint8_t _C[0x74];
	int32_t nTimeLeft; // 80
	uint8_t _84[0xC];
	float fScore; // 90
};

class PlayerScoreDerby {
public:
	uint8_t _0[0x4];
	uint32_t nPlayerId; // 4
	uint8_t _8[0x31];
	uint8_t bKnockedOut; // 39 can be both wrecked or out of time, not set in dm derby
	uint8_t _3A[0x2];
	uint32_t nContactTimerCurrentTick; // 3C
	uint32_t nContactTimerLastHit; // 40
	uint8_t _44[0x10];
	uint32_t nScore1; // 54
	uint32_t nScore2; // 58
	uint8_t _5C[0x10];
	uint32_t nLives; // 6C
	uint8_t _70[0x14];
	float fScoreMultiplier;
};

class ScoreManager {
public:
	uint8_t _0[0x8];
	void** pScoresStart; // +8
	void** pScoresEnd; // +C
	uint8_t _10[0x34];
	uint32_t nSurvivorId; // +44
};
auto& pScoreManager = *(ScoreManager**)0x846514;

class ArcadeRaceStats {
public:
	uint32_t score; // +0
	uint32_t placement; // +4, 255 if locked, 254 if unlocked, 1, 2, 3 otherwise
	uint8_t _8[0xC];
	uint32_t unlockScore; // +14
	uint8_t _18[0x8];
	uint32_t targetScores[3]; // +20
	uint8_t _2C[0x44];
};
static_assert(sizeof(ArcadeRaceStats) == 0x70);

class PlayerProfile {
public:
	uint8_t _0[0x3D0];
	struct {
		ArcadeRaceStats* races;
		uint32_t numRaces;
	} aArcadeClasses[0]; // +3D0
	uint8_t _3D0[0xEEA];
	uint8_t nActiveCar; // +12BA
};

enum eGameState {
	GAME_STATE_NONE,
	GAME_STATE_MENU,
	GAME_STATE_RACE
};

class Game {
public:
	uint8_t _0[0x4A4];
	int nGameState;
	uint8_t _4A8[0x8];
	int nGameMode; // +4B0
	int nGameRules; // +4B4
	uint8_t _4B8[0x14];
	int nLevelId; // +4CC
	uint8_t _4D0[0x8];
	int nInstantActionCar; // +4D8
	int nInstantActionCarSkin; // +4DC
	uint8_t _4E0[0x3C];
	int nDerbyType; // +51C
	uint8_t _520[0xFCC];
	uint32_t NumUnlockCar; // +14EC
	uint32_t UnlockCar[16]; // +14F0
	uint8_t _1530[0x1108];
	uint32_t nArcadeTargets[3]; // +2638
	uint8_t _2644[0x80C];
	PlayerProfile Profile; // +2E50
};
auto& pGame = *(Game**)0x9298FAC;

auto& pLoadingScreen = *(void**)0x929902C;

class CarUnlockState {
public:
	uint32_t bIsLocked;
	uint32_t bIsPurchased;
	uint8_t _8[0x18];
};
static_assert(sizeof(CarUnlockState) == 0x20);

class Garage {
public:
	uint8_t _0[0x5C];
	CarUnlockState* aCarUnlocks;
};

class HUDElement {
public:
	uint8_t _0[0xE8];
	bool bVisible;
};

class Body {
public:
	float mMatrix[4*4]; // +1C0
	uint8_t _200[0x80];
	float qQuaternion[4]; // +280
	float vVelocity[3]; // +290
	uint8_t _29C[0x4];
	float vAngVelocity[3]; // +2A0
};

class Camera {
public:
	uint8_t _0[0xF0];
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;
	uint8_t _100[0xC];
	float fNearZ;
	float fFarZ;
	float fFOV;
};

class CameraManager {
public:
	uint8_t _0[0x5C];
	Body* pTarget;
};
auto& pCameraManager = *(CameraManager**)0x9298FC0;

struct tGameSetting {
	enum eValueType {
		VALUE_TYPE_INT,
		VALUE_TYPE_BOOL,
		VALUE_TYPE_FLOAT,
		VALUE_TYPE_STRING,
	};

	const char* category;
	const char* name;
	void* value;
	int type;
	float minValue;
	float maxValue;
};

class Font {
public:
	uint8_t _0[0xC];
	NyaDrawing::CNyaRGBA32 nColor; // +C
	uint8_t _10[0x2C];
	struct
	{
		bool bXCenterAlign : 1;
		bool bXRightAlign : 1;
	}; // +3C
	uint8_t _40[0x14];
	float fScaleX; // +54
	float fScaleY; // +58

	static inline auto GetFont = (Font*(__stdcall*)(void*, const char*))0x457D00;
	static inline auto Display = (void(*)(Font*, float, float, const wchar_t*, ...))0x5A8BE0;
};

void DrawStringFO2(tNyaStringData data, const wchar_t* string, const char* font) {
	auto pFont = Font::GetFont(*(void**)(0x9298FD8), font);
	pFont->fScaleX = data.size * nResX / 20.0;
	pFont->fScaleY = data.size * nResY / 20.0;
	pFont->bXRightAlign = data.XRightAlign;
	pFont->bXCenterAlign = data.XCenterAlign;
	pFont->nColor.r = data.b;
	pFont->nColor.g = data.g;
	pFont->nColor.b = data.r;
	pFont->nColor.a = data.a;
	pFont->fScaleX *= GetAspectRatioInv();
	if (data.YCenterAlign) {
		data.y -= data.size * 0.5;
	}
	Font::Display(pFont, data.x * nResX, data.y * nResY, string);
}

void DrawStringFO2(const tNyaStringData& data, const std::string& name) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	DrawStringFO2(data, converter.from_bytes(name).c_str(), "FontLarge");
}

uintptr_t SetTextureFolder_call = 0x5A6E20;
void __attribute__((naked)) __fastcall SetTextureFolder(const char* path) {
	__asm__ (
		"mov eax, ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (SetTextureFolder_call)
	);
}

uintptr_t SetSharedTextureFolder_call = 0x5A6E90;
void __attribute__((naked)) __fastcall SetSharedTextureFolder(const char* path) {
	__asm__ (
		"mov eax, ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (SetSharedTextureFolder_call)
	);
}

enum eGameEvent {
	EVENT_MUSIC_PLAY_TITLE = 4003,
	EVENT_MUSIC_PLAY_INGAME = 4004,
	EVENT_MUSIC_STOP = 4007,
	EVENT_RESPAWN_PLAYER = 6037,
};

uintptr_t PostEvent_call = 0x4611D0;
float __attribute__((naked)) __fastcall PostEvent(int* eventData) {
	__asm__ (
		"pushad\n\t"
		"mov edx, 0x9298FB4\n\t"
		"mov edx, [edx]\n\t"
		"mov eax, ecx\n\t"
		"call %0\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (PostEvent_call)
	);
}

uintptr_t SendEvent_call = 0x461250;
int __attribute__((naked)) __fastcall SendEvent(void* a1, int* eventData) {
	__asm__ (
		"pushad\n\t"
		"mov eax, edx\n\t"
		"push ecx\n\t"
		"call %0\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (SendEvent_call)
	);
}

uintptr_t AddArcadeRaceScore_call = 0x46F300;
void __attribute__((naked)) __fastcall AddArcadeRaceScore(const wchar_t* name, int category, Game* game, float score, int unknown) {
	__asm__ (
		"mov eax, ecx\n\t"
		"mov ecx, edx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (AddArcadeRaceScore_call)
	);
}

template<typename T>
T* GetPlayerScore(int playerId) {
	if (!pScoreManager) return nullptr;

	auto score = (T**)pScoreManager->pScoresStart;
	auto end = (T**)pScoreManager->pScoresEnd;
	while (score < end) {
		if ((*score)->nPlayerId + 1 == playerId) {
			return *score;
		}
		score++;
	}
	return nullptr;
}

auto& fPowerups_BombMaxDistance = *(float*)0x76510C;

auto sTextureFolder = (const char*)0x845B78;
auto sSharedTextureFolder = (const char*)0x845C80;

auto AddHUDKeyword = (void(*)(const char*, void*, void*))0x4ECB20;

auto luaL_checktype = (void(*)(void*, int, int))0x634C70;
auto luaL_checkudata = (void*(*)(void*, int, const char*))0x634BB0;
auto luaL_typerror = (void(*)(void*, int, const char*))0x634900;
auto lua_pushnumber = (int(*)(void*, float))0x633550;
auto lua_tolstring = (const wchar_t*(*)(void*, int, void*))0x6332B0;
auto luaL_checknumber = (float(*)(void*, int))0x634DD0;
auto lua_setfield = (void(*)(void*, int, const char*))0x633D20;
auto lua_pushcfunction = (void(*)(void*, void*, int))0x633750;
auto lua_pushboolean = (int(*)(void*, int))0x633870;
auto lua_pushlstring = (int(*)(void*, const wchar_t*, size_t))0x6335D0;
auto lua_pushnil = (int(*)(void*))0x633520;
auto lua_settable = (int(*)(void*, int))0x633CD0;
auto lua_setglobal = (int(*)(void*, const char*))0x633640;
auto BFSManager_DoesFileExist = (bool(__stdcall*)(void*, const char*, int*))0x5B7170;
auto SetDefaultOptions = (void(*)())0x458A80;