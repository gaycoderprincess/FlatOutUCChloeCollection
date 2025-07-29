int nArcadeCareerCarSkin = 1;
int nArcadeCareerCarVariant = 0;
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

int GetCarMatchup(int id) {
	int dataId = GetCarDataID(id);
	static auto config = toml::parse_file("Config/CarMatchups.toml");

	auto name = GetCarName(id);
	auto matchupName = config["named"][name].value_or("");
	if (matchupName[0]) {
		auto carByName = GetCarByName(matchupName);
		if (carByName >= 0 && carByName > id) {
			return carByName;
		}
	}

	for (int i = 0; i < 1024; i++) {
		if (config["main"]["car_" + std::to_string(i)].value_or(-1) == dataId) {
			int dbId = GetCarDBID(i);
			if (dbId >= 0) {
				return dbId;
			}
		}
	}
	return -1;
}

int GetCarInverseMatchup(int id) {
	int dataId = GetCarDataID(id);
	static auto config = toml::parse_file("Config/CarMatchups.toml");
	auto matchup = config["main"]["car_" + std::to_string(dataId)].value_or(-1);
	if (matchup >= 0) {
		return GetCarDBID(matchup);
	}
	else {
		auto name = GetCarName(id);
		auto matchupName = config["named"][name].value_or("");
		if (matchupName[0]) {
			auto carByName = GetCarByName(matchupName);
			if (carByName >= 0 && carByName < id) {
				return carByName;
			}
		}
	}
	return -1;
}

int ChloeSkins_GetNumSkinsForCurrentCar(void* a1) {
	lua_pushnumber(a1, GetNumSkinsForCar(nCurrentMenuCar));
	return 1;
}

int ChloeSkins_GetNumSkinsForCar(void* a1) {
	lua_pushnumber(a1, GetNumSkinsForCar(luaL_checknumber(a1, 1)));
	return 1;
}

int ChloeSkins_GetSkinAuthor(void* a1) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	std::wstring author = config["car_" + std::to_string(GetCarDataID((int)luaL_checknumber(a1, 1)))]["skin" + std::to_string((int)luaL_checknumber(a1, 2))].value_or(L"");
	if (!author.empty()) author = L"Skin Author: " + author;
	lua_pushlstring(a1, author.c_str(), (author.length() + 1) * 2);
	return 1;
}

int ChloeSkins_IsSkinCustom(void* a1) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	int carId = (int)luaL_checknumber(a1, 1);
	int skinId = (int)luaL_checknumber(a1, 2);
	bool wrapAround = luaL_checknumber(a1, 3);
	int numSkins = GetNumSkinsForCar(carId);
	if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
		lua_pushboolean(a1, false);
		return 1;
	}
	// wrap around
	while (skinId < 1) {
		skinId += numSkins;
	}
	while (skinId > numSkins) {
		skinId -= numSkins;
	}
	std::wstring author = config["car_" + std::to_string(GetCarDataID(carId))]["skin" + std::to_string(skinId)].value_or(L"");
	lua_pushboolean(a1, !author.empty());
	return 1;
}

int ChloeSkins_GetSkinName(void* a1) {
	static auto config = toml::parse_file("Config/CarSkins.toml");
	int carId = (int)luaL_checknumber(a1, 1);
	int skinId = (int)luaL_checknumber(a1, 2);
	bool wrapAround = luaL_checknumber(a1, 3);
	int numSkins = GetNumSkinsForCar(carId);
	if (!wrapAround && (skinId < 1 || skinId > numSkins)) {
		std::wstring string = L"---";
		lua_pushlstring(a1, string.c_str(), (string.length() + 1) * 2);
		return 1;
	}
	// wrap around
	while (skinId < 1) {
		skinId += numSkins;
	}
	while (skinId > numSkins) {
		skinId -= numSkins;
	}
	std::wstring string = config["car_" + std::to_string(GetCarDataID(carId))]["skin" + std::to_string(skinId) + "name"].value_or(L"");
	if (string.empty()) string = L"Skin " + std::to_wstring(skinId);
	//std::wstring author = config["car_" + std::to_string(GetCarDataID(carId))]["skin" + std::to_string(skinId)].value_or(L"");
	//if (!author.empty()) string += L" - " + author;
	//if (!author.empty()) string = L"© " + string;
	lua_pushlstring(a1, string.c_str(), (string.length() + 1) * 2);
	return 1;
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

int ChloeCollection_GetCarDataID(void* a1) {
	lua_pushnumber(a1, GetCarDataID(luaL_checknumber(a1, 1)));
	return 1;
}

int ChloeCollection_GetCarDBID(void* a1) {
	lua_pushnumber(a1, GetCarDBID(luaL_checknumber(a1, 1)));
	return 1;
}

int ChloeCollection_GetCarByName(void* a1) {
	lua_pushnumber(a1, GetCarByName((const char*)lua_tolstring(a1, 1, nullptr)));
	return 1;
}

float UpdateMenuCar(void* a1, int a2) {
	nCurrentMenuCar = luaL_checknumber(a1, a2);
	return nCurrentMenuCar;
}

int GetUnlockIDForCustomCar(int id, bool warn) {
	static auto config = toml::parse_file("Config/CarUnlocks.toml");
	int replacementId = config["db_override"]["car" + std::to_string(id)].value_or(-1);
	if (replacementId < 0) {
		replacementId = config["main"]["car_" + std::to_string(GetCarDataID(id))].value_or(-1);
		if (replacementId >= 0) {
			replacementId = GetCarDBID(replacementId);
		}
	}
	if (replacementId < 0) {
		if (warn) MessageBoxA(nullptr, ("Failed to find unlock data for car " + std::to_string(id) + "!").c_str(), "nya?!~", MB_ICONERROR);
		return 0;
	}
	return replacementId;
}

int IsCarAlwaysUnlocked(int id) {
	static auto config = toml::parse_file("Config/CarUnlocks.toml");
	return config["always_unlocked"]["car_" + std::to_string(GetCarDataID(id))].value_or(false);
}

void GenerateUnlockList() {
	aCustomCarUnlockList.clear();

	static auto config = toml::parse_file("Config/CarUnlocks.toml");
	for (int i = 0; i < pGameFlow->Awards.nNumUnlockCar; i++) {
		auto unlockCar = pGameFlow->Awards.aUnlockCar[i];
		auto unlockCarName = GetCarName(unlockCar);
		auto unlockCarDataID = GetCarDataID(unlockCar);

		int numCars = GetNumCars();
		for (int j = 0; j < numCars; j++) {
			if (j == unlockCar) continue; // don't duplicate

			auto matchupName = (std::string)config["named"][GetCarName(j)].value_or("");
			if (matchupName[0] && matchupName == unlockCarName) {
				if (GetCarInverseMatchup(j) >= 0) continue; // don't add fo2 variants
				aCustomCarUnlockList.push_back(j);
			}
		}

		for (int j = 0; j < 512; j++) {
			if (config["main"]["car_" + std::to_string(j)].value_or(-1) == unlockCarDataID) {
				int dbId = GetCarDBID(j);
				if (dbId < 0) continue;
				if (dbId == unlockCar) continue; // don't duplicate
				if (GetCarInverseMatchup(dbId) >= 0) continue; // don't add fo2 variants
				aCustomCarUnlockList.push_back(dbId);
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

int ChloeUnlocks_GetUnlockCriteriaLabel(void* a1) {
	static auto config = toml::parse_file("Config/CarUnlockText.toml");
	auto dbId = luaL_checknumber(a1, 1);
	auto id = GetCarDataID(dbId);
	auto str = config["main"][std::format("car_{}",id)].value_or(L"");
	if (str.empty()) {
		str = config["main"][std::format("car_{}",GetCarDataID(GetUnlockIDForCustomCar(dbId, false)))].value_or(L"");
	}
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeArcade_SetCarSkin(void* a1) {
	nArcadeCareerCarSkin = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeArcade_SetCarVariant(void* a1) {
	nArcadeCareerCarVariant = luaL_checknumber(a1, 1);
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

uint32_t GetTotalArcadeScore(PlayerProfile* profile) {
	uint32_t score = 0;
	for (int x = 0; x < nNumArcadeRacesX; x++) {
		for (int y = 0; y < nNumArcadeRacesY; y++) {
			score += profile->aArcadeClasses[x].races[y].nScore;
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
	lua_pushboolean(a1, !bUnlockAllArcadeEvents && GetTotalArcadeScore(pProfile) < pProfile->aArcadeClasses[cls - 1].races[race - 1].nPointsToUnlock);
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
	lua_pushnumber(a1, pProfile->aArcadeClasses[cls - 1].races[race - 1].nPlacement);
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
	lua_pushnumber(a1, pProfile->aArcadeClasses[cls - 1].races[race - 1].nScore);
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

int ChloeCollection_GetCustomPlayerModelType(void* a1) {
	lua_pushnumber(a1, GetPlayerModelType());
	return 1;
}

int ChloeCollection_GetCustomPlayerModelSkinID(void* a1) {
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

int ChloeCollection_SetHandlingMode(void* a1) {
	nMultiplayerHandlingMode = luaL_checknumber(a1, 1);
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

int ChloeProfiles_GetProfileCarsUnlockedNoBonus(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileCarsUnlockedNoBonus(id));
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

int ChloeProfiles_GetProfilePlayerType(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfilePlayerType(id));
	return 1;
}

int ChloeProfiles_GetProfileFlag(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (!IsProfileValid(id)) return 0;
	lua_pushnumber(a1, GetProfileFlag(id));
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
	gCustomSave.ApplyPlayerSettings();
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

int ChloeProfiles_SetNumCarsUnlockedNoBonus(void* a1) {
	gCustomSave.numCarsUnlockedNoBonus = luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_SetGameProgress(void* a1) {
	gCustomSave.gameProgress = luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeProfiles_GetNumArcadeEventsPassed(void* a1) {
	int numRacesPassed = 0;
	for (int x = 0; x < nNumArcadeRacesX; x++) {
		for (int y = 0; y < nNumArcadeRacesY; y++) {
			if (gCustomSave.aArcadeRaces[x][y].placement == 1) numRacesPassed++; // arcade events should only count if golded
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
	lua_pushboolean(a1, nArcadePlatinumTargets[x][y] > 0 && gCustomSave.aArcadeRaces[x][y].score >= nArcadePlatinumTargets[x][y]);
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

int ChloeOST_GetStuntSoundtrackName(void* a1) {
	auto name = aStuntPlaylists[(int)luaL_checknumber(a1, 1)].name;
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

int ChloeOST_GetNumStuntSoundtracks(void* a1) {
	lua_pushnumber(a1, aStuntPlaylists.size());
	return 1;
}

int ChloePacenotes_GetSpeechName(void* a1) {
	auto name = aPacenoteSpeechTypes[(int)luaL_checknumber(a1, 1)].name;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloePacenotes_GetVisualName(void* a1) {
	auto name = aPacenoteVisualTypes[(int)luaL_checknumber(a1, 1)].name;
	if (name.empty()) return 0;
	lua_pushlstring(a1, name.c_str(), (name.length() + 1) * 2);
	return 1;
}

int ChloePacenotes_GetNumSpeechTypes(void* a1) {
	lua_pushnumber(a1, aPacenoteSpeechTypes.size());
	return 1;
}

int ChloePacenotes_GetNumVisualTypes(void* a1) {
	lua_pushnumber(a1, aPacenoteVisualTypes.size());
	return 1;
}

bool DoesCheatMatchAnyCarOrTrack(const std::string& str) {
	if (str == "temp350") {
		bPropCarsUnlocked = !bPropCarsUnlocked;
		return true;
	}

	for (int i = 0; i < GetNumCars(); i++) {
		auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", i).c_str());
		if (table->DoesPropertyExist("CheatCode")) {
			auto cheat = (std::string)table->GetPropertyAsString("CheatCode");
			std::transform(cheat.begin(), cheat.end(), cheat.begin(), [](unsigned char c){ return std::tolower(c); });
			if (cheat == str) {
				return true;
			}
		}
	}
	for (int i = 1; i < GetNumTracks() + 1; i++) {
		if (!DoesTrackExist(i)) continue;
		if (!DoesTrackValueExist(i, "CheatCode")) continue;
		auto cheat = (std::string)GetTrackValueString(i, "CheatCode");
		std::transform(cheat.begin(), cheat.end(), cheat.begin(), [](unsigned char c){ return std::tolower(c); });
		if (cheat == str) {
			return true;
		}
	}
	return false;
}

int ChloeCollection_CheckCheatCode(void* a1) {
	auto str = GetStringNarrow(lua_tolstring(a1, 1, nullptr));
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
	if (DoesCheatMatchAnyCarOrTrack(str)) {
		bool isEntered = false;
		for (auto& cheat : aCarCheatsEntered) {
			if (cheat == str) {
				isEntered = true;
				break;
			}
		}
		if (!isEntered) {
			aCarCheatsEntered.push_back(str);
			gCustomSave.Save();
		}
		lua_pushboolean(a1, true);
	}
	else if (str == "pressplay") {
		bUnlockAllArcadeEvents = !bUnlockAllArcadeEvents;
		lua_pushboolean(a1, true);
	}
	else if (str == "fuckmyphysicsupfam") {
		bNoDownforceCheat = !bNoDownforceCheat;
		lua_pushboolean(a1, true);
	}
	else lua_pushboolean(a1, false);

	// if prop cars got locked by this entry, remove temp350 from the savefile
	if (!bPropCarsUnlocked) {
		for (auto& cheat : aCarCheatsEntered) {
			if (cheat == "temp350") {
				aCarCheatsEntered.erase(aCarCheatsEntered.begin()+ (&cheat - &aCarCheatsEntered[0]));
				gCustomSave.Save();
				break;
			}
		}
	}

	return 1;
}

int ChloeCollection_SaveSettings(void* a1) {
	gCustomSave.Save();
	return 1;
}

int ChloeCollection_SetSpeedtrapMode(void* a1) {
	SpeedtrapMode::ApplyPatches(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_SetSpeedtrapSimpleUI(void* a1) {
	SpeedtrapMode::bSimpleUI = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetDriftMode(void* a1) {
	DriftMode::ApplyPatches(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_SetLapKnockoutMode(void* a1) {
	LapKnockoutMode::bTimedMode = false;
	LapKnockoutMode::ApplyPatches(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_SetTimedKnockoutMode(void* a1) {
	LapKnockoutMode::bTimedMode = true;
	LapKnockoutMode::ApplyPatches(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_SetDriftSimpleUI(void* a1) {
	DriftMode::bSimpleUI = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetDriftTime(void* a1) {
	DriftMode::nTimeLimit = (int)luaL_checknumber(a1, 1) * 60 * 1000;
	return 0;
}

int ChloeCollection_SetStuntMode(void* a1) {
	StuntMode::ApplyPatches(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_SetStuntTime(void* a1) {
	StuntMode::nTimeLimit = (int)luaL_checknumber(a1, 1) * 60 * 1000;
	return 0;
}

int ChloeCollection_SetStuntSimpleUI(void* a1) {
	StuntMode::bSimpleUI = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetStuntHandling(void* a1) {
	StuntMode::bStuntHandling = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_GetStuntHandling(void* a1) {
	lua_pushnumber(a1, StuntMode::bStuntHandling);
	return 1;
}

int ChloeCollection_SetAirControlMode(void* a1) {
	StuntMode::nAirControlMode = (int)luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_WasLastRaceStuntMode(void* a1) {
	lua_pushboolean(a1, bIsStuntMode);
	return 1;
}

int ChloeCollection_WasLastRaceSpeedtrap(void* a1) {
	lua_pushboolean(a1, bIsSpeedtrap);
	return 1;
}

int ChloeCollection_WasLastRaceDrift(void* a1) {
	lua_pushboolean(a1, bIsDriftEvent);
	return 1;
}

int ChloeCollection_WasLastRaceLapKnockout(void* a1) {
	lua_pushboolean(a1, bIsLapKnockout);
	return 1;
}

void ApplyAIExtenderPatches();
int ChloeCollection_ReinitHooks(void* a1) {
	ApplyAIExtenderPatches();
	StuntMode::ApplyPatches(false);
	SpeedtrapMode::ApplyPatches(false);
	DriftMode::ApplyPatches(false);
	LapKnockoutMode::ApplyPatches(false);
	CareerTimeTrial::ApplyPatches(false);
	bIsCareerRally = false;
	return 0;
}

double GetWidescreenLeft(float aspectCorrection = f43AspectCorrection) {
	return -(aspectCorrection - 640) * 0.5;
}

double GetWidescreenSafeLeft() {
	auto minLeft = GetWidescreenLeft(480 * GetSafeAspect());
	auto left = GetWidescreenLeft();
	if (minLeft > left) left = minLeft;
	return left;
}

double GetWidescreenSafeRight() {
	auto x = f43AspectCorrection;
	auto max = (480 * GetSafeAspect());
	if (x > max) x = max;
	return x;
}

int ChloeWidescreen_GetAspect(void* a1) {
	RecalculateAspectRatio();

	if (nWidescreenMenu) {
		lua_pushnumber(a1, fSpacingFixAmount43);
	}
	else {
		lua_pushnumber(a1, 1);
	}
	return 1;
}

int ChloeWidescreen_GetCenter(void* a1) {
	RecalculateAspectRatio();

	if (nWidescreenMenu) {
		lua_pushnumber(a1, f43AspectCorrectionCenter);
	}
	else {
		lua_pushnumber(a1, 320);
	}
	return 1;
}

int ChloeWidescreen_GetRight(void* a1) {
	RecalculateAspectRatio();

	if (nWidescreenMenu) {
		lua_pushnumber(a1, f43AspectCorrection);
	}
	else {
		lua_pushnumber(a1, 640);
	}
	return 1;
}

int ChloeWidescreen_GetSafeRight(void* a1) {
	RecalculateAspectRatio();

	if (nWidescreenMenu) {
		lua_pushnumber(a1, GetWidescreenSafeRight());
	}
	else {
		lua_pushnumber(a1, 640);
	}
	return 1;
}

int ChloeWidescreen_GetLeft(void* a1) {
	RecalculateAspectRatio();

	if (nWidescreenMenu) {
		lua_pushnumber(a1, GetWidescreenLeft());
	}
	else {
		lua_pushnumber(a1, 0);
	}
	return 1;
}

int ChloeWidescreen_GetSafeLeft(void* a1) {
	RecalculateAspectRatio();

	if (nWidescreenMenu) {
		lua_pushnumber(a1, GetWidescreenSafeLeft());
	}
	else {
		lua_pushnumber(a1, 0);
	}
	return 1;
}

int ChloeWidescreen_LeftJustify(void* a1) {
	RecalculateAspectRatio();

	auto f = luaL_checknumber(a1, 1);
	if (nWidescreenMenu) {
		f += GetWidescreenLeft();
	}
	lua_pushnumber(a1, f);
	return 1;
}

int ChloeWidescreen_SafeLeftJustify(void* a1) {
	RecalculateAspectRatio();

	auto f = luaL_checknumber(a1, 1);
	if (nWidescreenMenu) {
		f += GetWidescreenSafeLeft();
	}
	lua_pushnumber(a1, f);
	return 1;
}

int ChloeWidescreen_RightJustify(void* a1) {
	RecalculateAspectRatio();

	auto f = luaL_checknumber(a1, 1);
	if (nWidescreenMenu) {
		f -= 640.0 * 0.5;
		f += f43AspectCorrection * 0.5;
	}
	lua_pushnumber(a1, f);
	return 1;
}

int ChloeWidescreen_SafeRightJustify(void* a1) {
	RecalculateAspectRatio();

	auto f = luaL_checknumber(a1, 1);
	if (nWidescreenMenu) {
		f -= 640.0 * 0.5;
		f += GetWidescreenSafeRight() * 0.5;
	}
	lua_pushnumber(a1, f);
	return 1;
}

int ChloeWidescreen_HasSafeZone(void* a1) {
	lua_pushboolean(a1, std::abs(GetWidescreenLeft() - GetWidescreenSafeLeft()) > 0.05);
	return 1;
}

int ChloeWidescreen_WasWidescreenToggled(void* a1) {
	static auto bLastWidescreen = nWidescreenMenu;
	lua_pushboolean(a1, bLastWidescreen != nWidescreenMenu);
	bLastWidescreen = nWidescreenMenu;
	return 1;
}

int ChloeTuning_GetCarNitroModifier(void* a1) {
	lua_pushnumber(a1, GetCurrentCarTuning()->fNitroModifier * 100);
	return 1;
}

int ChloeTuning_SetCarNitroModifier(void* a1) {
	GetCurrentCarTuning()->fNitroModifier = luaL_checknumber(a1, 1) / 100.0;
	return 0;
}

int ChloeTuning_GetCarBrakeBias(void* a1) {
	lua_pushnumber(a1, GetCurrentCarTuning()->fBrakeBias * 100);
	return 1;
}

int ChloeTuning_SetCarBrakeBias(void* a1) {
	GetCurrentCarTuning()->fBrakeBias = luaL_checknumber(a1, 1) / 100.0;
	return 0;
}

int ChloeTuning_GetCarEngineModifier(void* a1) {
	lua_pushnumber(a1, GetCurrentCarTuning()->fEngineModifier * 100);
	return 1;
}

int ChloeTuning_SetCarEngineModifier(void* a1) {
	GetCurrentCarTuning()->fEngineModifier = luaL_checknumber(a1, 1) / 100.0;
	return 0;
}

int ChloeTuning_GetCarSuspensionStiffness(void* a1) {
	lua_pushnumber(a1, GetCurrentCarTuning()->fSuspensionStiffness * 100);
	return 1;
}

int ChloeTuning_SetCarSuspensionStiffness(void* a1) {
	GetCurrentCarTuning()->fSuspensionStiffness = luaL_checknumber(a1, 1) / 100.0;
	return 0;
}

int ChloeTuning_SetEnabledForMultiplayer(void* a1) {
	bEnableCarTuningForMultiplayer = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeTuning_SetActiveCarForMultiplayer(void* a1) {
	nCarTuningCurrectCarForMultiplayer = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetIsInMultiplayer(void* a1) {
	bIsInMultiplayer = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_GetRandom(void* a1) {
	int r = rand() % (int)luaL_checknumber(a1, 1);
	lua_pushnumber(a1, r);
	return 1;
}

int ChloeCollection_GetAIName(void* a1) {
	auto str = (std::wstring)GetAIName(luaL_checknumber(a1, 1));
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeCollection_GetCarMatchup(void* a1) {
	auto matchup = GetCarMatchup(luaL_checknumber(a1, 1));
	if (matchup >= 0) {
		lua_pushnumber(a1, matchup);
		return 1;
	}
	return 0;
}

int ChloeCollection_GetCarMatchupInverse(void* a1) {
	auto matchup = GetCarInverseMatchup(luaL_checknumber(a1, 1));
	if (matchup >= 0) {
		lua_pushnumber(a1, matchup);
		return 1;
	}
	return 0;
}

int ChloeCollection_GetCarTier(void* a1) {
	auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", (int)luaL_checknumber(a1, 1)).c_str());
	char tier[MAX_PATH];
	auto upgrades = table->GetPropertyAsNodePath(tier, "Upgrades", 0);
	auto tierStr = (std::string)tier;
	if (tierStr.find("Level1") != std::string::npos) {
		lua_pushnumber(a1, 1);
	}
	else  if (tierStr.find("Level2") != std::string::npos) {
		lua_pushnumber(a1, 2);
	}
	else if (tierStr.find("Level3") != std::string::npos) {
		lua_pushnumber(a1, 3);
	}
	else {
		lua_pushnumber(a1, 1);
	}
	return 1;
}

int ChloeSPStats_GetPlaytimeOfType(void* a1) {
	int id = luaL_checknumber(a1, 1);
	if (id < 0 || id >= NUM_PLAYTIME_TYPES_NEW) {
		return 0;
	}
	lua_pushnumber(a1, gCustomSave.playtimeNew[id]);
	return 1;
}

int ChloeCollection_SetIsTimeTrial(void* a1) {
	bIsTimeTrial = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetCareerTimeTrial(void* a1) {
	CareerTimeTrial::ApplyPatches(luaL_checknumber(a1, 1));
	return 0;
}

int ChloeCollection_SetCareerTimeTrialCar(void* a1) {
	CareerTimeTrial::nCar = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetCareerTimeTrialUpgrades(void* a1) {
	CareerTimeTrial::bUpgrades = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetCareerTimeTrialStartPosition(void* a1) {
	CareerTimeTrial::nStartPosition = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_SetCareerTimeTrialEventId(void* a1) {
	CareerTimeTrial::nEventClass = luaL_checknumber(a1, 1)-1;
	CareerTimeTrial::nEventId = luaL_checknumber(a1, 2)-1;
	return 0;
}

int ChloeCollection_SetCareerTimeTrialMedal(void* a1) {
	CareerTimeTrial::nMedalTimes[(int)luaL_checknumber(a1,1)]=luaL_checknumber(a1,2);
	return 0;
}

int ChloeCollection_SetCareerTimeTrialResult(void* a1) {
	CareerTimeTrial::SetMedal(luaL_checknumber(a1, 1));
	gCustomSave.Save();
	return 0;
}

int ChloeCollection_GetCareerTimeTrialResult(void* a1) {
	int x = ((int)luaL_checknumber(a1, 1))-1;
	int y = ((int)luaL_checknumber(a1, 2))-1;
	auto& event = gCustomSave.aCareerEvents[x][y];
	lua_pushnumber(a1, event.medal);

	//WriteLog(std::format("Getting results of event {} {}", x, y));
	//WriteLog(std::format("PB time {}, earned medal {}", event.pbTime, event.medal));
	return 1;
}

int ChloeCollection_GetCareerTimeTrialBestTime(void* a1) {
	auto& event = gCustomSave.aCareerEvents[((int)luaL_checknumber(a1, 1))-1][((int)luaL_checknumber(a1, 2))-1];
	lua_pushnumber(a1, event.pbTime);
	return 1;
}

int ChloeCollection_GetCarCustomMenuBG(void* a1) {
	auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", (int)luaL_checknumber(a1, 1)).c_str());
	if (table->DoesPropertyExist("MenuBG")) {
		lua_pushnumber(a1, table->GetPropertyAsInt("MenuBG", 0));
		return 1;
	}
	return 0;
}

int ChloeCollection_ArePropCarsUnlocked(void* a1) {
	lua_pushboolean(a1, bPropCarsUnlocked);
	return 1;
}

int ChloeCollection_IsCarLockedByCheatCode(void* a1) {
	auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", (int)luaL_checknumber(a1, 1)).c_str());
	if (table->DoesPropertyExist("CheatCode")) {
		auto str = (std::string)table->GetPropertyAsString("CheatCode");
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
		for (auto& cheat : aCarCheatsEntered) {
			if (cheat == str) {
				lua_pushboolean(a1, false);
				return 1;
			}
		}
		lua_pushboolean(a1, true);
		return 1;
	}
	lua_pushboolean(a1, false);
	return 1;
}

int ChloeCollection_IsTrackLockedByCheatCode(void* a1) {
	int id = (int)luaL_checknumber(a1, 1);
	if (DoesTrackValueExist(id, "CheatCode")) {
		auto str = (std::string)GetTrackValueString(id, "CheatCode");
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		for (auto &cheat: aCarCheatsEntered) {
			if (cheat == str) {
				lua_pushboolean(a1, false);
				return 1;
			}
		}
		lua_pushboolean(a1, true);
		return 1;
	}
	lua_pushboolean(a1, false);
	return 1;
}

int ChloeCollection_DoesTrackHaveCheatCode(void* a1) {
	if (DoesTrackValueExist(luaL_checknumber(a1, 1), "CheatCode")) {
		lua_pushboolean(a1, true);
		return 1;
	}
	lua_pushboolean(a1, false);
	return 1;
}

int ChloeCollection_GetAICarID(void* a1) {
	auto aiProfile = GetLiteDB()->GetTable("FlatOut2.Profiles")->GetPropertyAsNode("Profile", luaL_checknumber(a1, 1));
	auto classProfile = aiProfile->GetTable(std::format("Class[{}]", (int)luaL_checknumber(a1, 2)).c_str());
	auto car = classProfile->GetPropertyAsInt("CarNum", 0);
	lua_pushnumber(a1, GetCarDBID(car));
	return 1;
}

int ChloeCollection_DoesCarHaveCheatCode(void* a1) {
	auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", (int)luaL_checknumber(a1, 1)).c_str());
	if (table->DoesPropertyExist("CheatCode")) {
		lua_pushboolean(a1, true);
		return 1;
	}
	lua_pushboolean(a1, false);
	return 1;
}

int ChloeRally_AddCash(void* a1) {
	gCustomSave.nRallyCash += luaL_checknumber(a1, 1);
	gCustomSave.Save();
	return 0;
}

int ChloeRally_SetCash(void* a1) {
	gCustomSave.nRallyCash = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeRally_GetCash(void* a1) {
	if (gCustomSave.nRallyCash >= 100000) {
		AwardAchievement(GetAchievement("CASH_AWARD"));
	}
	lua_pushnumber(a1, gCustomSave.nRallyCash);
	return 1;
}

int ChloeRally_EnoughMoney(void* a1) {
	lua_pushboolean(a1, gCustomSave.nRallyCash >= (int)luaL_checknumber(a1, 1));
	return 1;
}

int ChloeRally_GetCupPosition(void* a1) {
	int classId = luaL_checknumber(a1, 1)-1;
	int cupId = luaL_checknumber(a1, 2)-1;
	lua_pushnumber(a1, gCustomSave.aRallyCareer[classId][cupId].nEventPosition);
	return 1;
}

int ChloeRally_IsCupLocked(void* a1) {
	int classId = luaL_checknumber(a1, 1)-1;
	int cupId = luaL_checknumber(a1, 2)-1;
	lua_pushboolean(a1, cupId != 0 && !gCustomSave.aRallyCareer[classId][cupId].bEventUnlocked);
	return 1;
}

int ChloeRally_IsEventLocked(void* a1) {
	int classId = luaL_checknumber(a1, 1)-1;
	int cupId = luaL_checknumber(a1, 2)-1;
	lua_pushboolean(a1, cupId != 0 && !gCustomSave.aRallyCareerEvents[classId][cupId].bEventUnlocked);
	return 1;
}

int ChloeRally_IsClassLocked(void* a1) {
	int classId = luaL_checknumber(a1, 1)-1;
	lua_pushboolean(a1, classId != 0 && !gCustomSave.bRallyClassUnlocked[classId]);
	return 1;
}

int ChloeRally_SetClassUnlocked(void* a1) {
	int classId = luaL_checknumber(a1, 1)-1;
	gCustomSave.bRallyClassUnlocked[classId] = luaL_checknumber(a1, 2);
	return 0;
}

int ChloeRally_GetNextStage(void* a1) {
	lua_pushnumber(a1, gCustomSave.nRallyCupNextStage+1);
	return 1;
}

int ChloeRally_GetPlayerByPosition(void* a1) {
	gCustomSave.CalculateRallyPlayersByPosition();
	int playerId = luaL_checknumber(a1,1)-1;
	lua_pushnumber(a1, gCustomSave.aRallyPlayersByPosition[playerId]+1);
	return 1;
}

int ChloeRally_GetPlayerPosition(void* a1) {
	gCustomSave.CalculateRallyPlayersByPosition();
	int playerId = luaL_checknumber(a1,1)-1;
	lua_pushnumber(a1, gCustomSave.aRallyPlayerPosition[playerId]+1);
	return 1;
}

int ChloeRally_GetPlayerCupPoints(void* a1) {
	int playerId = luaL_checknumber(a1,1)-1;
	lua_pushnumber(a1, gCustomSave.nRallyCupPoints[playerId]);
	return 1;
}

int ChloeRally_GetPlayerCupPointsForStage(void* a1) {
	int stageId = luaL_checknumber(a1,1)-1;
	int playerId = luaL_checknumber(a1,2)-1;
	lua_pushnumber(a1, gCustomSave.nRallyCupStagePoints[stageId][playerId]);
	return 1;
}

int ChloeRally_GetCupStagePosition(void* a1) {
	int stageId = luaL_checknumber(a1,1)-1;
	lua_pushnumber(a1, gCustomSave.nRallyCupStagePosition[stageId]);
	return 1;
}

int ChloeRally_GetPlayerName(void* a1) {
	int playerId = luaL_checknumber(a1,1)-1;
	if (playerId == 0) {
		auto str = (std::wstring)gCustomSave.playerName;
		lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	}
	else {
		auto str = (std::wstring)GetAIName(playerId-1);
		lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	}
	return 1;
}

int ChloeRally_GetCurrentClass(void* a1) {
	lua_pushnumber(a1, gCustomSave.nRallyClass+1);
	return 1;
}

int ChloeRally_GetCurrentCup(void* a1) {
	lua_pushnumber(a1, gCustomSave.nRallyCup+1);
	return 1;
}

int ChloeRally_GetCurrentEvent(void* a1) {
	lua_pushnumber(a1, gCustomSave.nRallyEvent+1);
	return 1;
}

int ChloeRally_IsCupActive(void* a1) {
	lua_pushboolean(a1, gCustomSave.nRallyCup >= 0 && gCustomSave.nRallyCupNextStage > 0);
	return 1;
}

int ChloeRally_SetCurrentClass(void* a1) {
	gCustomSave.nRallyClass = luaL_checknumber(a1, 1)-1;
	gCustomSave.Save();
	return 0;
}

int ChloeRally_SetCurrentCup(void* a1) {
	gCustomSave.nRallyCup = luaL_checknumber(a1, 1)-1;
	gCustomSave.nRallyCupNextStage = 0;
	memset(gCustomSave.nRallyCupPoints, 0, sizeof(gCustomSave.nRallyCupPoints));
	memset(gCustomSave.nRallyCupStagePosition, 0, sizeof(gCustomSave.nRallyCupStagePosition));
	memset(gCustomSave.nRallyCupStagePoints, 0, sizeof(gCustomSave.nRallyCupStagePoints));
	// save when a cup ends
	if (gCustomSave.nRallyCup < 0) {
		gCustomSave.Save();
	}
	return 0;
}

int ChloeRally_SetCurrentEvent(void* a1) {
	gCustomSave.nRallyEvent = luaL_checknumber(a1, 1)-1;
	return 0;
}

int ChloeRally_GetCurrentCar(void* a1) {
	lua_pushnumber(a1, gCustomSave.nRallyCarId);
	return 1;
}

int ChloeRally_GetCurrentCarSkin(void* a1) {
	lua_pushnumber(a1, gCustomSave.nRallyCarSkinId);
	return 1;
}

int ChloeRally_SetCurrentCar(void* a1) {
	gCustomSave.nRallyCarId = luaL_checknumber(a1, 1);
	return 1;
}

int ChloeRally_SetCurrentCarSkin(void* a1) {
	gCustomSave.nRallyCarSkinId = luaL_checknumber(a1, 1);
	return 1;
}

int ChloeRally_IsCarPurchased(void* a1) {
	int car = luaL_checknumber(a1, 1);
	// auto-unlock the current car for backwards compat
	if (car == gCustomSave.nRallyCarId) {
		gCustomSave.aRallyCareerGarage[car].bPurchased = true;
		gCustomSave.aRallyCareerGarage[car].nSkinId = gCustomSave.nRallyCarSkinId;
	}
	lua_pushboolean(a1, gCustomSave.aRallyCareerGarage[car].bPurchased);
	return 1;
}

int ChloeRally_SetCarPurchased(void* a1) {
	int car = luaL_checknumber(a1, 1);
	gCustomSave.aRallyCareerGarage[car].bPurchased = lua_toboolean(a1, 2);
	gCustomSave.Save();
	return 0;
}

int ChloeRally_SetCarSkin(void* a1) {
	int car = luaL_checknumber(a1, 1);
	gCustomSave.aRallyCareerGarage[car].nSkinId = luaL_checknumber(a1, 2);
	return 0;
}

int ChloeRally_GetNumCarsPurchased(void* a1) {
	int count = 0;
	for (auto& car : gCustomSave.aRallyCareerGarage) {
		if (car.bPurchased) count++;
	}
	lua_pushnumber(a1, count);
	return 1;
}

int ChloeRally_GetCarSkin(void* a1) {
	int car = luaL_checknumber(a1, 1);
	lua_pushnumber(a1, gCustomSave.aRallyCareerGarage[car].nSkinId);
	return 1;
}

int ChloeRally_GetCupNumPlayers(void* a1) {
	lua_pushnumber(a1, nNumAIProfiles+1);
	return 1;
}

int ChloeRally_SetIsCareerRally(void* a1) {
	bIsCareerRally = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeRally_GetIsCareerRally(void* a1) {
	lua_pushboolean(a1, bIsCareerRally);
	return 1;
}

int ChloeRally_SetNumRacesInCup(void* a1) {
	CareerRally::nNumRacesInThisCup = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeRally_IsCupFinished(void* a1) {
	lua_pushboolean(a1, gCustomSave.nRallyCupNextStage >= CareerRally::nNumRacesInThisCup);
	return 1;
}

int ChloeRally_AdvanceCup(void* a1) {
	CareerRally::AdvanceCup();
	return 0;
}

int ChloeRally_AdvanceEvent(void* a1) {
	CareerRally::AdvanceEvent();
	return 0;
}

int ChloeRally_IsNewCupJustFinished(void* a1) {
	lua_pushboolean(a1, CareerRally::bNewCupJustFinished);
	CareerRally::bNewCupJustFinished = false;
	return 1;
}

int ChloeCollection_DoesTrackHaveFO2Variant(void* a1) {
	int level = luaL_checknumber(a1, 1);
	auto pathG = (std::string)GetTrackValueString(level, "StagePath") + "geometryfo2/track_geom.w32";
	auto pathC = (std::string)GetTrackValueString(level, "StagePath") + "geometryfo2/track_cdb2.gen";
	lua_pushboolean(a1, DoesFileExist(pathG.c_str(), 0) || DoesFileExist(pathC.c_str(), 0));
	return 1;
}

int ChloeCollection_SetTrackFO2Variant(void* a1) {
	bLoadFO2Track = luaL_checknumber(a1, 1);
	return 0;
}

int ChloeCollection_AwardAchievement(void* a1) {
	AwardAchievement(GetAchievement((const char*)lua_tolstring(a1, 1, nullptr)));
	return 0;
}

int ChloeCollection_GetAchievementName(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1, nullptr));
	if (!achievement) return 0;
	std::wstring str = GetStringWide(achievement->sName);
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeCollection_GetAchievementDescription(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1, nullptr));
	if (!achievement) return 0;
	auto desc = (std::string)achievement->sDescription;
	if (!achievement->bUnlocked && achievement->fMaxInternalProgress > 0) {
		desc += std::format(" ({:.0f}/{})", achievement->fInternalProgress, achievement->fMaxInternalProgress);
	}
	std::wstring str = GetStringWide(desc);
	lua_pushlstring(a1, str.c_str(), (str.length() + 1) * 2);
	return 1;
}

int ChloeCollection_GetAchievementProgression(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1, nullptr));
	if (!achievement) return 0;

	int progress = achievement->nProgress;
	if (progress < 0) progress = 0;
	if (progress > 100) progress = 100;

	if (achievement->bUnlocked) progress = 100;
	else if (progress == 100) progress = 99;

	lua_pushnumber(a1, progress);
	return 1;
}

int ChloeCollection_GetAchievementCompleted(void* a1) {
	auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1, nullptr));
	if (!achievement) return 0;
	lua_pushboolean(a1, achievement->bUnlocked);
	return 1;
}

int ChloeCollection_GetAchievementInCategory(void* a1) {
	auto achievements = Achievements::GetAchievementsInCategory(luaL_checknumber(a1, 1));
	std::sort(achievements.begin(), achievements.end(), [] (Achievements::CAchievement* a, Achievements::CAchievement* b) { return (std::string)a->sName < (std::string)b->sName; });
	int id = luaL_checknumber(a1, 2)-1;
	if (id < 0 || id >= achievements.size()) return 0;
	std::string str = achievements[id]->sIdentifier;
	lua_pushlstring(a1, (const wchar_t*)str.c_str(), (str.length() + 1));
	return 1;
}

int ChloeCollection_GetNumAchievementsInCategory(void* a1) {
	auto achievements = Achievements::GetAchievementsInCategory(luaL_checknumber(a1, 1));
	lua_pushnumber(a1, achievements.size());
	return 1;
}

int ChloeCollection_SetAchievementTracked(void* a1) {
	if (auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1, nullptr))) {
		achievement->bTracked = !achievement->bTracked;
	}
	return 0;
}

int ChloeCollection_GetAchievementTrackable(void* a1) {
	if (auto achievement = GetAchievement((const char*)lua_tolstring(a1, 1, nullptr))) {
		lua_pushboolean(a1, !achievement->bUnlocked && achievement->pTrackFunction != nullptr);
		return 1;
	}
	return 0;
}

int ChloeCollection_SetTrackWon(void* a1) {
	gCustomSave.tracksWon[(int)luaL_checknumber(a1, 1)] = true;
	return 1;
}

void RegisterLUAFunction(void* a1, void* function, const char* name) {
	lua_pushcfunction(a1, function, 0);
	lua_setfield(a1, -10002, name);
}

void RegisterLUAEnum(void* a1, int id, const char* name) {
	lua_setglobal(a1, name);
	lua_pushnumber(a1, id);
	lua_settable(a1, -10002);
}

void CustomLUAFunctions(void* a1) {
	RegisterLUAFunction(a1, (void*)&ChloeTuning_GetCarNitroModifier, "ChloeTuning_GetCarNitroModifier");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_SetCarNitroModifier, "ChloeTuning_SetCarNitroModifier");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_GetCarBrakeBias, "ChloeTuning_GetCarBrakeBias");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_SetCarBrakeBias, "ChloeTuning_SetCarBrakeBias");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_GetCarEngineModifier, "ChloeTuning_GetCarEngineModifier");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_SetCarEngineModifier, "ChloeTuning_SetCarEngineModifier");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_GetCarSuspensionStiffness, "ChloeTuning_GetCarSuspensionStiffness");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_SetCarSuspensionStiffness, "ChloeTuning_SetCarSuspensionStiffness");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_SetEnabledForMultiplayer, "ChloeTuning_SetEnabledForMultiplayer");
	RegisterLUAFunction(a1, (void*)&ChloeTuning_SetActiveCarForMultiplayer, "ChloeTuning_SetActiveCarForMultiplayer");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetAspect, "ChloeWidescreen_GetAspect");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_LeftJustify, "ChloeWidescreen_LeftJustify");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_SafeLeftJustify, "ChloeWidescreen_SafeLeftJustify");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_RightJustify, "ChloeWidescreen_RightJustify");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_SafeRightJustify, "ChloeWidescreen_SafeRightJustify");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetCenter, "ChloeWidescreen_GetCenter");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetLeft, "ChloeWidescreen_GetLeft");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetSafeLeft, "ChloeWidescreen_GetSafeLeft");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetRight, "ChloeWidescreen_GetRight");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_GetSafeRight, "ChloeWidescreen_GetSafeRight");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_HasSafeZone, "ChloeWidescreen_HasSafeZone");
	RegisterLUAFunction(a1, (void*)&ChloeWidescreen_WasWidescreenToggled, "ChloeWidescreen_WasWidescreenToggled");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_EnablePlatinumGoal, "ChloeArcade_EnablePlatinumGoal");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_DisablePlatinumGoal, "ChloeArcade_DisablePlatinumGoal");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_SetCurrentEventId, "ChloeArcade_SetCurrentEventId");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_HasPlatinumOnEvent, "ChloeArcade_HasPlatinumOnEvent");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_SetPlatinumTargetForLevel, "ChloeArcade_SetPlatinumTargetForLevel");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_SetCarSkin, "ChloeArcade_SetCarSkin");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_SetCarVariant, "ChloeArcade_SetCarVariant");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_ForceAllAICars, "ChloeArcade_ForceAllAICars");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_ForceAllAICarsDuo, "ChloeArcade_ForceAllAICarsDuo");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_ForceAICarCount, "ChloeArcade_ForceAICarCount");
	RegisterLUAFunction(a1, (void*)&ChloeArcade_SetLenientMultipliers, "ChloeArcade_SetLenientMultipliers");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_GetNumSkinsForCurrentCar, "ChloeSkins_GetNumSkinsForCurrentCar");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_GetNumSkinsForCar, "ChloeSkins_GetNumSkinsForCar");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_GetSkinAuthor, "ChloeSkins_GetSkinAuthor");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_GetSkinName, "ChloeSkins_GetSkinName");
	RegisterLUAFunction(a1, (void*)&ChloeSkins_IsSkinCustom, "ChloeSkins_IsSkinCustom");
	RegisterLUAFunction(a1, (void*)&ChloeUnlocks_GetNumUnlockCustomCar, "ChloeUnlocks_GetNumUnlockCustomCar");
	RegisterLUAFunction(a1, (void*)&ChloeUnlocks_GetUnlockCustomCar, "ChloeUnlocks_GetUnlockCustomCar");
	RegisterLUAFunction(a1, (void*)&ChloeUnlocks_GetUnlockCriteriaLabel, "ChloeUnlocks_GetUnlockCriteriaLabel");
	RegisterLUAFunction(a1, (void*)&ChloeInput_OpenInputWindow, "ChloeInput_OpenInputWindow");
	RegisterLUAFunction(a1, (void*)&ChloeInput_CloseInputWindow, "ChloeInput_CloseInputWindow");
	RegisterLUAFunction(a1, (void*)&ChloeInput_GetInputText, "ChloeInput_GetInputText");
	RegisterLUAFunction(a1, (void*)&ChloeInput_IsInputWindowCanceled, "ChloeInput_IsInputWindowCanceled");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_DoesProfileExist, "ChloeProfiles_DoesProfileExist");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_IsProfileValid, "ChloeProfiles_IsProfileValid");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileName, "ChloeProfiles_GetProfileName");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCar, "ChloeProfiles_GetProfileCar");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileClass, "ChloeProfiles_GetProfileClass");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCupsCompleted, "ChloeProfiles_GetProfileCupsCompleted");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCarsUnlocked, "ChloeProfiles_GetProfileCarsUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileCarsUnlockedNoBonus, "ChloeProfiles_GetProfileCarsUnlockedNoBonus");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileProgress, "ChloeProfiles_GetProfileProgress");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfilePortrait, "ChloeProfiles_GetProfilePortrait");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfilePlayerType, "ChloeProfiles_GetProfilePlayerType");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileFlag, "ChloeProfiles_GetProfileFlag");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetProfileMoney, "ChloeProfiles_GetProfileMoney");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_SetProfileSlot, "ChloeProfiles_SetProfileSlot");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_HasLoaded, "ChloeProfiles_HasLoaded");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_WasLoadSuccessful, "ChloeProfiles_WasLoadSuccessful");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_DeleteProfile, "ChloeProfiles_DeleteProfile");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_LoadPlayerNameFromProfile, "ChloeProfiles_LoadPlayerNameFromProfile");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_SetNumCupsPassed, "ChloeProfiles_SetNumCupsPassed");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_SetNumCarsUnlocked, "ChloeProfiles_SetNumCarsUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_SetNumCarsUnlockedNoBonus, "ChloeProfiles_SetNumCarsUnlockedNoBonus");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_SetGameProgress, "ChloeProfiles_SetGameProgress");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetNumArcadeEventsPassed, "ChloeProfiles_GetNumArcadeEventsPassed");
	RegisterLUAFunction(a1, (void*)&ChloeProfiles_GetNumArcadeEvents, "ChloeProfiles_GetNumArcadeEvents");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetSoundtrackName, "ChloeOST_GetSoundtrackName");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetMenuSoundtrackName, "ChloeOST_GetMenuSoundtrackName");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetStuntSoundtrackName, "ChloeOST_GetStuntSoundtrackName");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetNumSoundtracks, "ChloeOST_GetNumSoundtracks");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetNumMenuSoundtracks, "ChloeOST_GetNumMenuSoundtracks");
	RegisterLUAFunction(a1, (void*)&ChloeOST_GetNumStuntSoundtracks, "ChloeOST_GetNumStuntSoundtracks");
	RegisterLUAFunction(a1, (void*)&ChloePacenotes_GetSpeechName, "ChloePacenotes_GetSpeechName");
	RegisterLUAFunction(a1, (void*)&ChloePacenotes_GetVisualName, "ChloePacenotes_GetVisualName");
	RegisterLUAFunction(a1, (void*)&ChloePacenotes_GetNumSpeechTypes, "ChloePacenotes_GetNumSpeechTypes");
	RegisterLUAFunction(a1, (void*)&ChloePacenotes_GetNumVisualTypes, "ChloePacenotes_GetNumVisualTypes");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarDataID, "ChloeCollection_GetCarDataID");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarDBID, "ChloeCollection_GetCarDBID");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarByName, "ChloeCollection_GetCarByName");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_HasWelcomeScreenDisplayed, "ChloeCollection_HasWelcomeScreenDisplayed");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetWelcomeScreenDisplayed, "ChloeCollection_SetWelcomeScreenDisplayed");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCustomPlayerModelType, "ChloeCollection_GetCustomPlayerModelType");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCustomPlayerModelSkinID, "ChloeCollection_GetCustomPlayerModelSkinID");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetFragDerbyRewardAmount, "ChloeCollection_GetFragDerbyRewardAmount");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetHandlingMode, "ChloeCollection_SetHandlingMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_CheckCheatCode, "ChloeCollection_CheckCheatCode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SaveSettings, "ChloeCollection_SaveSettings");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetLapKnockoutMode, "ChloeCollection_SetLapKnockoutMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetTimedKnockoutMode, "ChloeCollection_SetTimedKnockoutMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetDriftMode, "ChloeCollection_SetDriftMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetDriftSimpleUI, "ChloeCollection_SetDriftSimpleUI");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetDriftTime, "ChloeCollection_SetDriftTime");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetSpeedtrapMode, "ChloeCollection_SetSpeedtrapMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetSpeedtrapSimpleUI, "ChloeCollection_SetSpeedtrapSimpleUI");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetStuntMode, "ChloeCollection_SetStuntMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetStuntTime, "ChloeCollection_SetStuntTime");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetStuntSimpleUI, "ChloeCollection_SetStuntSimpleUI");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetStuntHandling, "ChloeCollection_SetStuntHandling");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetStuntHandling, "ChloeCollection_GetStuntHandling");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetAirControlMode, "ChloeCollection_SetAirControlMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_WasLastRaceStuntMode, "ChloeCollection_WasLastRaceStuntMode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_WasLastRaceSpeedtrap, "ChloeCollection_WasLastRaceSpeedtrap");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_WasLastRaceDrift, "ChloeCollection_WasLastRaceDrift");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_WasLastRaceLapKnockout, "ChloeCollection_WasLastRaceLapKnockout");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_ReinitHooks, "ChloeCollection_ReinitHooks");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetIsInMultiplayer, "ChloeCollection_SetIsInMultiplayer");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetRandom, "ChloeCollection_GetRandom");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAIName, "ChloeCollection_GetAIName");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarMatchup, "ChloeCollection_GetCarMatchup");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarMatchupInverse, "ChloeCollection_GetCarMatchupInverse");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarTier, "ChloeCollection_GetCarTier");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrial, "ChloeCollection_SetCareerTimeTrial");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrialCar, "ChloeCollection_SetCareerTimeTrialCar");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrialUpgrades, "ChloeCollection_SetCareerTimeTrialUpgrades");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrialStartPosition, "ChloeCollection_SetCareerTimeTrialStartPosition");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetIsTimeTrial, "ChloeCollection_SetIsTimeTrial");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrialEventId, "ChloeCollection_SetCareerTimeTrialEventId");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrialMedal, "ChloeCollection_SetCareerTimeTrialMedal");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetCareerTimeTrialResult, "ChloeCollection_SetCareerTimeTrialResult");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCareerTimeTrialResult, "ChloeCollection_GetCareerTimeTrialResult");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCareerTimeTrialBestTime, "ChloeCollection_GetCareerTimeTrialBestTime");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetCarCustomMenuBG, "ChloeCollection_GetCarCustomMenuBG");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_ArePropCarsUnlocked, "ChloeCollection_ArePropCarsUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_IsCarLockedByCheatCode, "ChloeCollection_IsCarLockedByCheatCode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_IsTrackLockedByCheatCode, "ChloeCollection_IsTrackLockedByCheatCode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_DoesCarHaveCheatCode, "ChloeCollection_DoesCarHaveCheatCode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_DoesTrackHaveCheatCode, "ChloeCollection_DoesTrackHaveCheatCode");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAICarID, "ChloeCollection_GetAICarID");
	RegisterLUAFunction(a1, (void*)&ChloeSPStats_GetPlaytimeOfType, "ChloeSPStats_GetPlaytimeOfType");
	RegisterLUAFunction(a1, (void*)&ChloeRally_AddCash, "ChloeRally_AddCash");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCash, "ChloeRally_SetCash");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCash, "ChloeRally_GetCash");
	RegisterLUAFunction(a1, (void*)&ChloeRally_EnoughMoney, "ChloeRally_EnoughMoney");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCupPosition, "ChloeRally_GetCupPosition");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsCupLocked, "ChloeRally_IsCupLocked");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsEventLocked, "ChloeRally_IsEventLocked");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsClassLocked, "ChloeRally_IsClassLocked");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetClassUnlocked, "ChloeRally_SetClassUnlocked");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetNextStage, "ChloeRally_GetNextStage");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetPlayerByPosition, "ChloeRally_GetPlayerByPosition");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetPlayerPosition, "ChloeRally_GetPlayerPosition");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetPlayerCupPoints, "ChloeRally_GetPlayerCupPoints");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetPlayerCupPointsForStage, "ChloeRally_GetPlayerCupPointsForStage");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCurrentClass, "ChloeRally_GetCurrentClass");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCurrentCup, "ChloeRally_GetCurrentCup");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCurrentEvent, "ChloeRally_GetCurrentEvent");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsCupActive, "ChloeRally_IsCupActive");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCurrentClass, "ChloeRally_SetCurrentClass");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCurrentCup, "ChloeRally_SetCurrentCup");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCurrentEvent, "ChloeRally_SetCurrentEvent");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCurrentCar, "ChloeRally_GetCurrentCar");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCurrentCarSkin, "ChloeRally_GetCurrentCarSkin");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCurrentCar, "ChloeRally_SetCurrentCar");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCurrentCarSkin, "ChloeRally_SetCurrentCarSkin");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsCarPurchased, "ChloeRally_IsCarPurchased");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCarPurchased, "ChloeRally_SetCarPurchased");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetCarSkin, "ChloeRally_SetCarSkin");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetNumCarsPurchased, "ChloeRally_GetNumCarsPurchased");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCarSkin, "ChloeRally_GetCarSkin");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCupNumPlayers, "ChloeRally_GetCupNumPlayers");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetPlayerName, "ChloeRally_GetPlayerName");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetCupStagePosition, "ChloeRally_GetCupStagePosition");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetIsCareerRally, "ChloeRally_SetIsCareerRally");
	RegisterLUAFunction(a1, (void*)&ChloeRally_GetIsCareerRally, "ChloeRally_GetIsCareerRally");
	RegisterLUAFunction(a1, (void*)&ChloeRally_SetNumRacesInCup, "ChloeRally_SetNumRacesInCup");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsCupFinished, "ChloeRally_IsCupFinished");
	RegisterLUAFunction(a1, (void*)&ChloeRally_AdvanceCup, "ChloeRally_AdvanceCup");
	RegisterLUAFunction(a1, (void*)&ChloeRally_AdvanceEvent, "ChloeRally_AdvanceEvent");
	RegisterLUAFunction(a1, (void*)&ChloeRally_IsNewCupJustFinished, "ChloeRally_IsNewCupJustFinished");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_DoesTrackHaveFO2Variant, "ChloeCollection_DoesTrackHaveFO2Variant");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetTrackFO2Variant, "ChloeCollection_SetTrackFO2Variant");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_AwardAchievement, "ChloeCollection_AwardAchievement");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementName, "ChloeCollection_GetAchievementName");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementDescription, "ChloeCollection_GetAchievementDescription");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementProgression, "ChloeCollection_GetAchievementProgression");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementCompleted, "ChloeCollection_GetAchievementCompleted");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementInCategory, "ChloeCollection_GetAchievementInCategory");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetNumAchievementsInCategory, "ChloeCollection_GetNumAchievementsInCategory");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetAchievementTracked, "ChloeCollection_SetAchievementTracked");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_GetAchievementTrackable, "ChloeCollection_GetAchievementTrackable");
	RegisterLUAFunction(a1, (void*)&ChloeCollection_SetTrackWon, "ChloeCollection_SetTrackWon");

	RegisterLUAEnum(a1, Achievements::CAT_GENERAL, "ACHIEVEMENTS_GENERAL");
	RegisterLUAEnum(a1, Achievements::CAT_SINGLEPLAYER, "ACHIEVEMENTS_SINGLEPLAYER");
	RegisterLUAEnum(a1, Achievements::CAT_MULTIPLAYER, "ACHIEVEMENTS_MULTIPLAYER");
	RegisterLUAEnum(a1, Achievements::CAT_CAREER, "ACHIEVEMENTS_CAREER");
	RegisterLUAEnum(a1, Achievements::CAT_CARNAGE, "ACHIEVEMENTS_CARNAGE");
	RegisterLUAEnum(a1, Achievements::CAT_RALLY, "ACHIEVEMENTS_RALLY");
	RegisterLUAEnum(a1, Achievements::CAT_GAMEMODES, "ACHIEVEMENTS_GAMEMODES");
	RegisterLUAEnum(a1, Achievements::CAT_TRACKS, "ACHIEVEMENTS_TRACKS");
	RegisterLUAEnum(a1, Achievements::CAT_HIDDEN, "ACHIEVEMENTS_HIDDEN");

	RegisterLUAEnum(a1, HANDLING_NORMAL, "HANDLING_NORMAL");
	RegisterLUAEnum(a1, HANDLING_NORMAL_FO2DOWNFORCE, "HANDLING_NORMAL_FO2DOWNFORCE");
	RegisterLUAEnum(a1, HANDLING_PROFESSIONAL, "HANDLING_PROFESSIONAL");
	RegisterLUAEnum(a1, HANDLING_BETA, "HANDLING_BETA");
	RegisterLUAEnum(a1, HANDLING_NORMAL_LEGACY, "HANDLING_NORMAL_LEGACY");

	RegisterLUAEnum(a1, GR_TONYHAWK, "GR_TONYHAWK");
	RegisterLUAEnum(a1, GR_SPEEDTRAP, "GR_SPEEDTRAP");
	RegisterLUAEnum(a1, GR_DRIFT, "GR_DRIFT");
	RegisterLUAEnum(a1, GR_LAPKNOCKOUT, "GR_LAPKNOCKOUT");
	RegisterLUAEnum(a1, GR_KNOCKOUT, "GR_KNOCKOUT");

	RegisterLUAEnum(a1, PLAYTIME_TOTAL, "PLAYTIME_TOTAL");
	RegisterLUAEnum(a1, PLAYTIME_MENU, "PLAYTIME_MENU");
	RegisterLUAEnum(a1, PLAYTIME_INGAME, "PLAYTIME_INGAME");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_SINGLEPLAYER, "PLAYTIME_INGAME_SINGLEPLAYER");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_MULTIPLAYER, "PLAYTIME_INGAME_MULTIPLAYER");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_CAREER, "PLAYTIME_INGAME_CAREER");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_CARNAGE, "PLAYTIME_INGAME_CARNAGE");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_SINGLE, "PLAYTIME_INGAME_SINGLE");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_ALLRACE, "PLAYTIME_INGAME_ALLRACE");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_RACE, "PLAYTIME_INGAME_RACE");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_PONGRACE, "PLAYTIME_INGAME_PONGRACE");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_ARCADERACE, "PLAYTIME_INGAME_ARCADERACE");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_BEATTHEBOMB, "PLAYTIME_INGAME_BEATTHEBOMB");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_ALLDERBY, "PLAYTIME_INGAME_ALLDERBY");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_WRECKINGDERBY, "PLAYTIME_INGAME_WRECKINGDERBY");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_LMSDERBY, "PLAYTIME_INGAME_LMSDERBY");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_FRAGDERBY, "PLAYTIME_INGAME_FRAGDERBY");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_STUNT, "PLAYTIME_INGAME_STUNT");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_STUNTSHOW, "PLAYTIME_INGAME_STUNTSHOW");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_TIMETRIAL, "PLAYTIME_INGAME_TIMETRIAL");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_DRIFT, "PLAYTIME_INGAME_DRIFT");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_LAPKNOCKOUT, "PLAYTIME_INGAME_LAPKNOCKOUT");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_SPEEDTRAP, "PLAYTIME_INGAME_SPEEDTRAP");
	RegisterLUAEnum(a1, PLAYTIME_INGAME_RALLYMODE, "PLAYTIME_INGAME_RALLYMODE");
	RegisterLUAEnum(a1, NUM_PLAYTIME_TYPES_NEW, "NUM_PLAYTIME_TYPES");

	static auto sVersionString = "Chloe's Collection v1.73 - Achievements Edition";
	lua_setglobal(a1, "ChloeCollectionVersion");
	lua_setglobal(a1, sVersionString);
	lua_settable(a1, -10002);
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

	NyaFO2Hooks::PlaceScriptHook();
	NyaFO2Hooks::aScriptFuncs.push_back(CustomLUAFunctions);

	NyaHookLib::Patch(0x715B0C, &GetArcadeLevelPosition);
	NyaHookLib::Patch(0x715B24, &IsArcadeLevelLocked);
	NyaHookLib::Patch(0x715B3C, &GetArcadeLevelScore);
	NyaHookLib::Patch(0x715B54, &GetArcadeTotalScore);
	//NyaHookLib::Patch(0x462615 + 1, &DebugConsolePrint);
}