int nArcadeCareerCarSkin = 1;
int nForceAllAICarsNextRace = -1;
int nForceAllAICarsNextRaceDuo[2] = {-1, -1};
int nForceAICountNextRace = -1;
uint32_t nCurrentMenuCar = 0;
std::vector<int> aCustomCarUnlockList;

auto GetStringWide(const std::string& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(string);
}

auto GetStringNarrow(const std::wstring& string) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(string);
}

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

int ChloeSkins_GetNumSkinsForCurrentCar(void* a1) {
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

int ChloeSkins_GetSkinAuthor(void* a1) {
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

int ChloeCollection_HasWelcomeScreenDisplayed(void* a1) {
	lua_pushboolean(a1, gCustomSave.bWelcomeScreenDisplayed);
	return 1;
}

int ChloeCollection_SetWelcomeScreenDisplayed(void* a1) {
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

int ChloeUnlocks_GetNumUnlockCustomCar(void* a1) {
	GenerateUnlockList();
	lua_pushnumber(a1, aCustomCarUnlockList.size());
	return 1;
}

int ChloeUnlocks_GetUnlockCustomCar(void* a1) {
	GenerateUnlockList();
	lua_pushnumber(a1, aCustomCarUnlockList[luaL_checknumber(a1, 1)-1]);
	return 1;
}

int ChloeArcade_SetCarSkin(void* a1) {
	GenerateUnlockList();
	nArcadeCareerCarSkin = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeArcade_ForceAllAICars(void* a1) {
	nForceAllAICarsNextRace = luaL_checknumber(a1, 1);

	// using player skins
	NyaHookLib::Patch<uint8_t>(0x43407E, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0xEB);
	return 0;
}

int ChloeArcade_ForceAllAICarsDuo(void* a1) {
	nForceAllAICarsNextRaceDuo[0] = luaL_checknumber(a1, 1);
	nForceAllAICarsNextRaceDuo[1] = luaL_checknumber(a1, 2);

	// using player skins
	NyaHookLib::Patch<uint8_t>(0x43407E, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0xEB);
	return 0;
}

int ChloeArcade_ForceAICarCount(void* a1) {
	nForceAICountNextRace = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeArcade_SetLenientMultipliers(void* a1) {
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
	for (int x = 0; x < numClasses; x++) {
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
	lua_pushboolean(a1, !bUnlockAllArcadeEvents && GetTotalArcadeScore(pProfile) < pProfile->aArcadeClasses[cls - 1].races[race - 1].unlockScore);
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
	lua_pushnumber(a1, pProfile->aArcadeClasses[cls - 1].races[race - 1].placement);
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
	lua_pushnumber(a1, pProfile->aArcadeClasses[cls - 1].races[race - 1].score);
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

int ChloeCollection_GetFragDerbyRewardAmount(void* a1) {
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
int ChloeCollection_SetHandlingMode(void* a1) {
	SetSlideControl(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeInput_OpenInputWindow(void* a1) {
	sInputWindowTitle = GetStringNarrow(lua_tolstring(a1, 1, nullptr));
	nInputEntryLength = luaL_checknumber(a1, 2);
	if (nInputEntryLength < 1) nInputEntryLength = 1;
	if (nInputEntryLength > nMaxInputEntry) nInputEntryLength = nMaxInputEntry;
	SetInputWindowOpen(true);
	return 0;
}

int ChloeInput_CloseInputWindow(void* a1) {
	SetInputWindowOpen(false);
	return 0;
}

int ChloeInput_GetInputText(void* a1) {
	if (sInputWindowLastEntry.empty()) return 0;

	auto str = GetStringWide(sInputWindowLastEntry);
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	sInputWindowLastEntry = "";
	return 1;
}

int ChloeInput_IsInputWindowCanceled(void* a1) {
	lua_pushboolean(a1, !bInputWindowOpen && sInputWindowLastEntry.empty());
	return 1;
}

int ChloeProfiles_DoesProfileExist(void* a1) {
	lua_pushboolean(a1, std::filesystem::exists(GetProfilePath((luaL_checknumber(a1, 1)))));
	return 1;
}

int ChloeProfiles_IsProfileValid(void* a1) {
	lua_pushboolean(a1, IsProfileValid(luaL_checknumber(a1, 1)));
	return 1;
}

int ChloeProfiles_GetProfileName(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	auto str = (std::wstring)GetProfileName(id);
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeProfiles_GetProfileCar(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCar(id));
	return 1;
}

int ChloeProfiles_GetProfileClass(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileClass(id));
	return 1;
}

int ChloeProfiles_GetProfileCupsCompleted(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCupsCompleted(id));
	return 1;
}

int ChloeProfiles_GetProfileCarsUnlocked(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCarsUnlocked(id));
	return 1;
}

int ChloeProfiles_GetProfileProgress(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileProgress(id));
	return 1;
}

int ChloeProfiles_GetProfilePortrait(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfilePortrait(id));
	return 1;
}

int ChloeProfiles_GetProfileMoney(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileMoney(id));
	return 1;
}

int ChloeProfiles_SetProfileSlot(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (id < 1) return 0;
	nSaveSlot = id;
	return 0;
}

int ChloeProfiles_HasLoaded(void* a1) {
	lua_pushboolean(a1, bHasProfileLoaded);
	bHasProfileLoaded = false;
	return 1;
}

int ChloeProfiles_WasLoadSuccessful(void* a1) {
	lua_pushboolean(a1, bWasProfileLoadSuccessful);
	return 1;
}

int ChloeProfiles_DeleteProfile(void* a1) {
	DeleteProfile(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeProfiles_LoadPlayerNameFromProfile(void* a1) {
	gCustomSave.Load(nSaveSlot, true);
	wcscpy_s(gCustomSave.playerName, 32, GetProfileName(nSaveSlot));
	if (!gCustomSave.playerName[0]) wcscpy_s(gCustomSave.playerName, 32, L"PLAYER");
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_SetNumCupsPassed(void* a1) {
	gCustomSave.numCupsPassed = luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_SetNumCarsUnlocked(void* a1) {
	gCustomSave.numCarsUnlocked = luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_SetGameProgress(void* a1) {
	gCustomSave.gameProgress = luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_SetProfilePortrait(void* a1) {
	gCustomSave.playerPortrait = luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_GetNumArcadeEventsPassed(void* a1) {
	int numRacesPassed = 0;
	for (int x = 0; x < nNumArcadeRacesX; x++) {
		for (int y = 0; y < nNumArcadeRacesY; y++) {
			auto pos = gCustomSave.aArcadeRaces[x][y].placement;
			if (pos == 1) numRacesPassed++; // arcade events should only count if golded
		}
	}
	lua_pushnumber(a1, numRacesPassed);
	return 1;
}

int ChloeProfiles_GetNumArcadeEvents(void* a1) {
	lua_pushnumber(a1, nNumArcadeRacesX * nNumArcadeRacesY);
	return 1;
}

int ChloeArcade_EnablePlatinumGoal(void* a1) {
	bArcadePlatinumEnabled = true;
	return 0;
}

int ChloeArcade_DisablePlatinumGoal(void* a1) {
	bArcadePlatinumEnabled = false;
	return 0;
}

int ChloeArcade_SetCurrentEventId(void* a1) {
	nArcadePlatinumCurrentLevelX = luaL_checknumber(a1, 1) - 1;
	nArcadePlatinumCurrentLevelY = luaL_checknumber(a1, 2) - 1;
	return 0;
}

int ChloeArcade_HasPlatinumOnEvent(void* a1) {
	int x = luaL_checknumber(a1, 1) - 1;
	int y = luaL_checknumber(a1, 2) - 1;
	lua_pushboolean(a1, gCustomSave.bArcadePlatinums[x][y]);
	return 1;
}

int ChloeArcade_SetPlatinumTargetForLevel(void* a1) {
	int x = luaL_checknumber(a1, 1) - 1;
	int y = luaL_checknumber(a1, 2) - 1;
	nArcadePlatinumTargets[x][y] = luaL_checknumber(a1, 3);
	return 0;
}

int ChloeOST_GetSoundtrackName(void* a1) {
	auto name = aPlaylists[(int)luaL_checknumber(a1, 1)].name;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloeOST_GetMenuSoundtrackName(void* a1) {
	auto name = aMenuPlaylists[(int)luaL_checknumber(a1, 1)].name;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloeOST_GetNumSoundtracks(void* a1) {
	lua_pushnumber(a1, aPlaylists.size());
	return 1;
}

int ChloeOST_GetNumMenuSoundtracks(void* a1) {
	lua_pushnumber(a1, aMenuPlaylists.size());
	return 1;
}

int ChloeCollection_CheckCheatCode(void* a1) {
	auto str = lua_tolstring(a1, 1, nullptr);
	if (!wcscmp(str, L"pressplay")) {
		bUnlockAllArcadeEvents = !bUnlockAllArcadeEvents;
		lua_pushboolean(a1, true);
	}
	else lua_pushboolean(a1, false);
	return 1;
}

void ApplyAIExtenderPatches();
int ChloeCollection_ReinitHooks(void* a1) {
	ApplyAIExtenderPatches();
	return 0;
}

auto lua_pushcfunction_hooked = (void(*)(void*, void*, int))0x633750;
void CustomLUAFunctions(void* a1, void* a2, int a3) {
	lua_pushcfunction(a1, (void*)&ChloeArcade_EnablePlatinumGoal, 0);
	lua_setfield(a1, -10002, "ChloeArcade_EnablePlatinumGoal");
	lua_pushcfunction(a1, (void*)&ChloeArcade_DisablePlatinumGoal, 0);
	lua_setfield(a1, -10002, "ChloeArcade_DisablePlatinumGoal");
	lua_pushcfunction(a1, (void*)&ChloeArcade_SetCurrentEventId, 0);
	lua_setfield(a1, -10002, "ChloeArcade_SetCurrentEventId");
	lua_pushcfunction(a1, (void*)&ChloeArcade_HasPlatinumOnEvent, 0);
	lua_setfield(a1, -10002, "ChloeArcade_HasPlatinumOnEvent");
	lua_pushcfunction(a1, (void*)&ChloeArcade_SetPlatinumTargetForLevel, 0);
	lua_setfield(a1, -10002, "ChloeArcade_SetPlatinumTargetForLevel");
	lua_pushcfunction(a1, (void*)&ChloeSkins_GetNumSkinsForCurrentCar, 0);
	lua_setfield(a1, -10002, "ChloeSkins_GetNumSkinsForCurrentCar");
	lua_pushcfunction(a1, (void*)&ChloeSkins_GetSkinAuthor, 0);
	lua_setfield(a1, -10002, "ChloeSkins_GetSkinAuthor");
	lua_pushcfunction(a1, (void*)&ChloeCollection_HasWelcomeScreenDisplayed, 0);
	lua_setfield(a1, -10002, "ChloeCollection_HasWelcomeScreenDisplayed");
	lua_pushcfunction(a1, (void*)&ChloeCollection_SetWelcomeScreenDisplayed, 0);
	lua_setfield(a1, -10002, "ChloeCollection_SetWelcomeScreenDisplayed");
	lua_pushcfunction(a1, (void*)&ChloeUnlocks_GetNumUnlockCustomCar, 0);
	lua_setfield(a1, -10002, "ChloeUnlocks_GetNumUnlockCustomCar");
	lua_pushcfunction(a1, (void*)&ChloeUnlocks_GetUnlockCustomCar, 0);
	lua_setfield(a1, -10002, "ChloeUnlocks_GetUnlockCustomCar");
	lua_pushcfunction(a1, (void*)&ChloeArcade_SetCarSkin, 0);
	lua_setfield(a1, -10002, "ChloeArcade_SetCarSkin");
	lua_pushcfunction(a1, (void*)&ChloeArcade_ForceAllAICars, 0);
	lua_setfield(a1, -10002, "ChloeArcade_ForceAllAICars");
	lua_pushcfunction(a1, (void*)&ChloeArcade_ForceAllAICarsDuo, 0);
	lua_setfield(a1, -10002, "ChloeArcade_ForceAllAICarsDuo");
	lua_pushcfunction(a1, (void*)&ChloeArcade_ForceAICarCount, 0);
	lua_setfield(a1, -10002, "ChloeArcade_ForceAICarCount");
	lua_pushcfunction(a1, (void*)&ChloeArcade_SetLenientMultipliers, 0);
	lua_setfield(a1, -10002, "ChloeArcade_SetLenientMultipliers");
	lua_pushcfunction(a1, (void*)&GetCustomPlayerModelType, 0);
	lua_setfield(a1, -10002, "GetCustomPlayerModelType");
	lua_pushcfunction(a1, (void*)&GetCustomPlayerModelSkinID, 0);
	lua_setfield(a1, -10002, "GetCustomPlayerModelSkinID");
	lua_pushcfunction(a1, (void*)&ChloeCollection_GetFragDerbyRewardAmount, 0);
	lua_setfield(a1, -10002, "ChloeCollection_GetFragDerbyRewardAmount");
	lua_pushcfunction(a1, (void*)&ChloeCollection_SetHandlingMode, 0);
	lua_setfield(a1, -10002, "ChloeCollection_SetHandlingMode");
	lua_pushcfunction(a1, (void*)&ChloeInput_OpenInputWindow, 0);
	lua_setfield(a1, -10002, "ChloeInput_OpenInputWindow");
	lua_pushcfunction(a1, (void*)&ChloeInput_CloseInputWindow, 0);
	lua_setfield(a1, -10002, "ChloeInput_CloseInputWindow");
	lua_pushcfunction(a1, (void*)&ChloeInput_GetInputText, 0);
	lua_setfield(a1, -10002, "ChloeInput_GetInputText");
	lua_pushcfunction(a1, (void*)&ChloeInput_IsInputWindowCanceled, 0);
	lua_setfield(a1, -10002, "ChloeInput_IsInputWindowCanceled");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_DoesProfileExist, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_DoesProfileExist");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_IsProfileValid, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_IsProfileValid");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileName, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileName");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileCar, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileCar");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileClass, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileClass");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileCupsCompleted, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileCupsCompleted");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileCarsUnlocked, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileCarsUnlocked");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileProgress, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileProgress");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfilePortrait, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfilePortrait");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetProfileMoney, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetProfileMoney");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_SetProfileSlot, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_SetProfileSlot");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_HasLoaded, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_HasLoaded");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_WasLoadSuccessful, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_WasLoadSuccessful");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_DeleteProfile, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_DeleteProfile");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_LoadPlayerNameFromProfile, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_LoadPlayerNameFromProfile");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_SetNumCupsPassed, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_SetNumCupsPassed");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_SetNumCarsUnlocked, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_SetNumCarsUnlocked");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_SetGameProgress, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_SetGameProgress");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_SetProfilePortrait, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_SetProfilePortrait");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetNumArcadeEventsPassed, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetNumArcadeEventsPassed");
	lua_pushcfunction(a1, (void*)&ChloeProfiles_GetNumArcadeEvents, 0);
	lua_setfield(a1, -10002, "ChloeProfiles_GetNumArcadeEvents");
	lua_pushcfunction(a1, (void*)&ChloeOST_GetSoundtrackName, 0);
	lua_setfield(a1, -10002, "ChloeOST_GetSoundtrackName");
	lua_pushcfunction(a1, (void*)&ChloeOST_GetMenuSoundtrackName, 0);
	lua_setfield(a1, -10002, "ChloeOST_GetMenuSoundtrackName");
	lua_pushcfunction(a1, (void*)&ChloeOST_GetNumSoundtracks, 0);
	lua_setfield(a1, -10002, "ChloeOST_GetNumSoundtracks");
	lua_pushcfunction(a1, (void*)&ChloeOST_GetNumMenuSoundtracks, 0);
	lua_setfield(a1, -10002, "ChloeOST_GetNumMenuSoundtracks");
	lua_pushcfunction(a1, (void*)&ChloeCollection_CheckCheatCode, 0);
	lua_setfield(a1, -10002, "ChloeCollection_CheckCheatCode");
	lua_pushcfunction(a1, (void*)&ChloeCollection_ReinitHooks, 0);
	lua_setfield(a1, -10002, "ChloeCollection_ReinitHooks");

	static auto sVersionString = "Chloe's Collection v1.26 - Custom Soundtrack Edition";
	lua_setglobal(a1, "ChloeCollectionVersion");
	lua_setglobal(a1, sVersionString);
	lua_settable(a1, -10002);
	return lua_pushcfunction_hooked(a1, a2, a3);
}

int DebugConsolePrint(void* a1) {
	static auto file = std::ofstream("scriptlog.txt");
	file << (const char*)lua_tolstring(a1, 1, nullptr);
	file << "\n";
	file.flush();
	return 0;
}

void ApplyLUAPatches() {
	luaL_checknumber = (float(*)(void*, int))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4D653A, &UpdateMenuCar);
	lua_pushcfunction_hooked = (void(*)(void*, void*, int))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4C9829, &CustomLUAFunctions);
	NyaHookLib::Patch(0x715C50, &IsCarLocked);
	NyaHookLib::Patch(0x715B0C, &GetArcadeLevelPosition);
	NyaHookLib::Patch(0x715B24, &IsArcadeLevelLocked);
	NyaHookLib::Patch(0x715B3C, &GetArcadeLevelScore);
	NyaHookLib::Patch(0x715B54, &GetArcadeTotalScore);
	//NyaHookLib::Patch(0x462615 + 1, &DebugConsolePrint);
}