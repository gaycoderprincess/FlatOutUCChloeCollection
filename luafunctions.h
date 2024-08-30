int nArcadeCareerCarSkin = 1;
int nForceAllAICarsNextRace = -1;
int nForceAICountNextRace = -1;
uint32_t nCurrentMenuCar = 0;
std::vector<int> aCustomCarUnlockList;

uintptr_t GetCarDataPath_call = 0x4C6340;
int __attribute__((naked)) __fastcall GetCarDataPath(int dbCar, bool isMenuCar) {
	__asm__ (
		"mov eax, ecx\n\t"
		"push edx\n\t"
		"call %0\n\t"
		"pop edx\n\t"
		"ret\n\t"
			:
			:  "m" (GetCarDataPath_call)
	);
}

uintptr_t GetScriptParameter_call = 0x5DB370;
int __attribute__((naked)) __fastcall GetScriptParameter(void* a1) {
	__asm__ (
		"mov eax, ecx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (GetScriptParameter_call)
	);
}

int GetNumSkinsForCurrentCar(void* a1) {
	int tmp[2];
	auto path = "data/cars/car_" + std::to_string(GetCarDataPath(nCurrentMenuCar, false)) + "/skin";
	int i;
	for (i = 0; i < 255; i++) {
		auto file = (path + std::to_string(i+1) + ".dds");
		if (!BFSManager_DoesFileExist(*(void**)0x846688, file.c_str(), tmp)) {
			break;
		}
	}
	lua_pushnumber(a1, i);
	return 1;
}

int GetCarSkinAuthor(void* a1) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	std::wstring author = config["car" + std::to_string((int)luaL_checknumber(a1, 1))]["skin" + std::to_string((int)luaL_checknumber(a1, 2))].value_or(L"");
	if (!author.empty()) author = L"Skin Author: " + author;
	lua_pushlstring(a1, author.c_str(), (author.length() + 1) * 2);
	return 1;
}

int GetOpponentCount() {
	int count = 11;
	switch (nOpponentCountType) {
		case 0:
			count = 7;
			break;
		case 1:
			count = 11;
			break;
		case 2:
			count = nNumAIProfiles;
			break;
		case 3:
			count = 31;
			break;
		default:
			break;
	}
	return count;
}

int ShouldDisplayCareerOpponentsWarning(void* a1) {
	lua_pushboolean(a1, false);
	//lua_pushboolean(a1, GetOpponentCount() >= 16);
	return 1;
}

int HasCCWelcomeScreenDisplayed(void* a1) {
	lua_pushboolean(a1, gCustomSave.bWelcomeScreenDisplayed);
	return 1;
}

int SetCCWelcomeScreenDisplayed(void* a1) {
	gCustomSave.bWelcomeScreenDisplayed = true;
	gCustomSave.Save();
	return 0;
}

float UpdateMenuCar(void* a1, int a2) {
	nCurrentMenuCar = luaL_checknumber(a1, a2);
	return nCurrentMenuCar;
}

int GetUnlockIDForCustomCar(int id, bool warn) {
	static auto config = toml::parse_file("Config/CarUnlocks.toml");
	int replacementId = config["main"]["car" + std::to_string(id)].value_or(-1);
	if (replacementId < 0) {
		if (warn) MessageBoxA(nullptr, ("Failed to find unlock data for car " + std::to_string(id) + "!").c_str(), "nya?!~", MB_ICONERROR);
		return 0;
	}
	return replacementId;
}

void GenerateUnlockList() {
	aCustomCarUnlockList.clear();

	static auto config = toml::parse_file("Config/CarUnlocks.toml");
	for (int i = 0; i < pGame->NumUnlockCar; i++) {
		for (int j = 0; j < 255; j++) {
			if (j == pGame->UnlockCar[i]) continue; // don't duplicate
			if (config["main"]["car" + std::to_string(j)].value_or(-1) == pGame->UnlockCar[i]) {
				aCustomCarUnlockList.push_back(j);
			}
		}
	}
}

int GetNumUnlockCustomCar(void* a1) {
	GenerateUnlockList();
	lua_pushnumber(a1, aCustomCarUnlockList.size());
	return 1;
}

int GetUnlockCustomCar(void* a1) {
	GenerateUnlockList();
	lua_pushnumber(a1, aCustomCarUnlockList[luaL_checknumber(a1, 1)-1]);
	return 1;
}

int SetArcadeCareerCarSkin(void* a1) {
	GenerateUnlockList();
	nArcadeCareerCarSkin = luaL_checknumber(a1, 1);
	return 0;
}

int SetArcadeCareerEventForceAllAICars(void* a1) {
	nForceAllAICarsNextRace = luaL_checknumber(a1, 1);

	// using player skins
	NyaHookLib::Patch<uint8_t>(0x43407E, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0xEB);
	return 0;
}

int SetArcadeCareerEventForceAICarCount(void* a1) {
	nForceAICountNextRace = luaL_checknumber(a1, 1);
	return 0;
}

int SetArcadeCareerEventLenientMultipliers(void* a1) {
	bool use = luaL_checknumber(a1, 1) != 0.0;
	SetArcadeRaceMultiplierPointer(use ? fArcadeRacePositionMultiplierLenient : fArcadeRacePositionMultiplier);
	return 0;
}

int IsCarLocked(void* a1) {
	luaL_checktype(a1, 1, 7);
	auto ppData = (Garage**)luaL_checkudata(a1, 1, "Garage");
	if (!ppData) {
		luaL_typerror(a1, 1, "Garage");
	}
	auto pGarage = *ppData;

	uint32_t data[2];
	data[0] = (uint32_t)a1;
	data[1] = 2;
	auto id = GetScriptParameter(data);
	if (id > 48) { // 46 and 47 are Bonecracker and Grinder
		id = GetUnlockIDForCustomCar(id, true);
	}
		// and allow the config to change vanilla cars too if desired
	else if (auto replacementId = GetUnlockIDForCustomCar(id, false)) {
		id = replacementId;
	}
	lua_pushboolean(a1, pGarage->aCarUnlocks[id].bIsLocked != 0);
	return 1;
}

uint32_t GetTotalArcadeScore(PlayerProfile* profile) {
	uint32_t score = 0;
	int numClasses = nNumArcadeRacesX;
	int numRaces = nNumArcadeRacesY;
	for (int x = 1; x < numClasses + 1; x++) {
		for (int y = 0; y < numRaces; y++) {
			score += profile->aArcadeClasses[x].races[y].score;
		}
	}
	return score;
}

int IsArcadeLevelLocked(void* a1) {
	luaL_checktype(a1, 1, 7);
	auto ppData = (PlayerProfile**)luaL_checkudata(a1, 1, "PlayerProfile");
	if (!ppData) {
		luaL_typerror(a1, 1, "PlayerProfile");
	}
	auto pProfile = *ppData;
	gCustomSave.UpdateArcadeRace(pProfile);
	auto cls = (int)luaL_checknumber(a1, 2);
	auto race = (int)luaL_checknumber(a1, 3);
	lua_pushboolean(a1, GetTotalArcadeScore(pProfile) < pProfile->aArcadeClasses[cls].races[race - 1].unlockScore);
	return 1;
}

int GetArcadeLevelPosition(void* a1) {
	luaL_checktype(a1, 1, 7);
	auto ppData = (PlayerProfile**)luaL_checkudata(a1, 1, "PlayerProfile");
	if (!ppData) {
		luaL_typerror(a1, 1, "PlayerProfile");
	}
	auto pProfile = *ppData;
	gCustomSave.UpdateArcadeRace(pProfile);
	auto cls = (int)luaL_checknumber(a1, 2);
	auto race = (int)luaL_checknumber(a1, 3);
	auto placement = pProfile->aArcadeClasses[cls].races[race - 1].placement;
	lua_pushnumber(a1, placement > 32 ? 255 : placement);
	return 1;
}

int GetArcadeLevelScore(void* a1) {
	luaL_checktype(a1, 1, 7);
	auto ppData = (PlayerProfile**)luaL_checkudata(a1, 1, "PlayerProfile");
	if (!ppData) {
		luaL_typerror(a1, 1, "PlayerProfile");
	}
	auto pProfile = *ppData;
	gCustomSave.UpdateArcadeRace(pProfile);
	auto cls = (int)luaL_checknumber(a1, 2);
	auto race = (int)luaL_checknumber(a1, 3);
	lua_pushnumber(a1, pProfile->aArcadeClasses[cls].races[race - 1].score);
	return 1;
}

int GetArcadeTotalScore(void* a1) {
	luaL_checktype(a1, 1, 7);
	auto ppData = (PlayerProfile**)luaL_checkudata(a1, 1, "PlayerProfile");
	if (!ppData) {
		luaL_typerror(a1, 1, "PlayerProfile");
	}
	auto pProfile = *ppData;
	gCustomSave.UpdateArcadeRace(pProfile);
	lua_pushnumber(a1, GetTotalArcadeScore(pProfile));
	return 1;
}

int GetCustomPlayerModelType(void* a1) {
	lua_pushnumber(a1, GetPlayerModelType());
	return 1;
}

int GetCustomPlayerModelSkinID(void* a1) {
	lua_pushnumber(a1, GetPlayerModelSkinID());
	return 1;
}

int GetFragDerbyRewardAmount(void* a1) {
	int type = luaL_checknumber(a1, 1);
	switch (type) {
		case 1:
			lua_pushnumber(a1, nFragDerbyRewardSlam);
			break;
		case 2:
			lua_pushnumber(a1, nFragDerbyRewardSuperFlip);
			break;
		case 3:
			lua_pushnumber(a1, nFragDerbyRewardPowerHit);
			break;
		case 4:
			lua_pushnumber(a1, nFragDerbyRewardBlastOut);
			break;
		default:
			MessageBoxA(nullptr, std::format("LUA error: Invalid parameter given to GetFragDerbyRewardAmount ({})", type).c_str(), "nya?!~", MB_ICONERROR);
			exit(0);
			break;
	}
	return 1;
}

void SetSlideControl(bool disabled);
int SetHandlingMode(void* a1) {
	SetSlideControl(luaL_checknumber(a1, 1));
	return 1;
}

auto lua_pushcfunction_hooked = (void(*)(void*, void*, int))0x633750;
void CustomLUAFunctions(void* a1, void* a2, int a3) {
	lua_pushcfunction(a1, (void*)&GetNumSkinsForCurrentCar, 0);
	lua_setfield(a1, -10002, "GetNumSkinsForCurrentCar");
	lua_pushcfunction(a1, (void*)&GetCarSkinAuthor, 0);
	lua_setfield(a1, -10002, "GetCarSkinAuthor");
	lua_pushcfunction(a1, (void*)&HasCCWelcomeScreenDisplayed, 0);
	lua_setfield(a1, -10002, "HasCCWelcomeScreenDisplayed");
	lua_pushcfunction(a1, (void*)&SetCCWelcomeScreenDisplayed, 0);
	lua_setfield(a1, -10002, "SetCCWelcomeScreenDisplayed");
	lua_pushcfunction(a1, (void*)&ShouldDisplayCareerOpponentsWarning, 0);
	lua_setfield(a1, -10002, "ShouldDisplayCareerOpponentsWarning");
	lua_pushcfunction(a1, (void*)&GetNumUnlockCustomCar, 0);
	lua_setfield(a1, -10002, "GetNumUnlockCustomCar");
	lua_pushcfunction(a1, (void*)&GetUnlockCustomCar, 0);
	lua_setfield(a1, -10002, "GetUnlockCustomCar");
	lua_pushcfunction(a1, (void*)&SetArcadeCareerCarSkin, 0);
	lua_setfield(a1, -10002, "SetArcadeCareerCarSkin");
	lua_pushcfunction(a1, (void*)&SetArcadeCareerEventForceAllAICars, 0);
	lua_setfield(a1, -10002, "SetArcadeCareerEventForceAllAICars");
	lua_pushcfunction(a1, (void*)&SetArcadeCareerEventForceAICarCount, 0);
	lua_setfield(a1, -10002, "SetArcadeCareerEventForceAICarCount");
	lua_pushcfunction(a1, (void*)&SetArcadeCareerEventLenientMultipliers, 0);
	lua_setfield(a1, -10002, "SetArcadeCareerEventLenientMultipliers");
	lua_pushcfunction(a1, (void*)&GetCustomPlayerModelType, 0);
	lua_setfield(a1, -10002, "GetCustomPlayerModelType");
	lua_pushcfunction(a1, (void*)&GetCustomPlayerModelSkinID, 0);
	lua_setfield(a1, -10002, "GetCustomPlayerModelSkinID");
	lua_pushcfunction(a1, (void*)&GetFragDerbyRewardAmount, 0);
	lua_setfield(a1, -10002, "GetFragDerbyRewardAmount");
	lua_pushcfunction(a1, (void*)&SetHandlingMode, 0);
	lua_setfield(a1, -10002, "SetHandlingMode");
	return lua_pushcfunction_hooked(a1, a2, a3);
}

void ApplyLUAPatches() {
	luaL_checknumber = (float(*)(void*, int))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4D653A, &UpdateMenuCar);
	lua_pushcfunction_hooked = (void(*)(void*, void*, int))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4C9829, &CustomLUAFunctions);
	NyaHookLib::Patch(0x715C50, &IsCarLocked);
	NyaHookLib::Patch(0x715B0C, &GetArcadeLevelPosition);
	NyaHookLib::Patch(0x715B24, &IsArcadeLevelLocked);
	NyaHookLib::Patch(0x715B3C, &GetArcadeLevelScore);
	NyaHookLib::Patch(0x715B54, &GetArcadeTotalScore);
}