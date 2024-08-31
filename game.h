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
};

class ScoreManager {
public:
	uint8_t _0[0x8];
	void** pScoresStart;
	void** pScoresEnd;
};
auto& pScoreManager = *(ScoreManager**)0x846514;

class Game {
public:
	uint8_t _0[0x4B0];
	int nGameMode;
	int nGameRules;
	uint8_t _4B8[0x64];
	int nDerbyType; // +51C
	uint8_t _520[0xFCC];
	uint32_t NumUnlockCar; // +14EC
	uint32_t UnlockCar[0];
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

auto sTextureFolder = (const char*)0x845B78;
auto sSharedTextureFolder = (const char*)0x845C80;

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
auto BFSManager_DoesFileExist = (bool(__stdcall*)(void*, const char*, int*))0x5B7170;