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
};

enum eDerbyType {
	DERBY_NONE = 0,
	DERBY_LMS = 1,
	DERBY_WRECKING = 2,
	DERBY_FRAG = 3,
};

class Player {
public:
	uint8_t _4[0x2C0];
	uint32_t nPlayerId; // +2C4
	
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
	} aArcadeClasses[0];
};

class Game {
public:
	uint8_t _0[0x4B0];
	int nGameMode;
	int nGameRules;
	uint8_t _4B8[0x64];
	int nDerbyType; // +51C
	uint8_t _520[0xFCC];
	uint32_t NumUnlockCar; // +14EC
	uint32_t UnlockCar[16]; // +14F0
	uint8_t _1530[0x1108];
	uint32_t nArcadeTargets[3]; // +2638
};
auto& pGame = *(Game**)0x9298FAC;

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
	}; // +34
	uint8_t _38[0x14];
	float fScaleX; // +4C
	float fScaleY; // +50

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
auto BFSManager_DoesFileExist = (bool(__stdcall*)(void*, const char*, int*))0x5B7170;

int lua_setglobal(void* a1, const char *a2) {
	if (a2) return lua_pushlstring(a1, (const wchar_t*)a2, strlen(a2));
	return lua_pushnil(a1);
}