const char* aPlaylistBasePath = "data/music/";
char aPlaylistTitlePath[MAX_PATH] = "data/music/playlist_title.bed";
char aPlaylistIngamePath[MAX_PATH] = "data/music/playlist_ingame.bed";
char aPlaylistStuntPath[MAX_PATH] = "data/music/playlist_stunt.bed";

struct tPlaylist {
	std::string filename;
	std::wstring name;

	FO2Vector<MusicInterface::tSong> gamePlaylist;
	int gamePlaylistCurrent = -2;
	int gamePlaylistNext = 0;

	bool ShouldDisableStuntDisplay() {
		if (!gamePlaylist.GetSize()) return false;

		if (!strcmp(gamePlaylist[0].sTitle.Get(), "Henri")) return true;
		return false;
	}
};
std::vector<tPlaylist> aPlaylists;
std::vector<tPlaylist> aMenuPlaylists;
std::vector<tPlaylist> aStuntPlaylists;
std::vector<tPlaylist> aTrackPlaylists;

tPlaylist* GetPlaylistByFilename(const std::string& name) {
	for (auto& playlist : aPlaylists) {
		if (playlist.filename == name) return &playlist;
	}
	for (auto& playlist : aMenuPlaylists) {
		if (playlist.filename == name) return &playlist;
	}
	for (auto& playlist : aStuntPlaylists) {
		if (playlist.filename == name) return &playlist;
	}
	for (auto& playlist : aTrackPlaylists) {
		if (playlist.filename == name) return &playlist;
	}
	return nullptr;
}

void ApplyIngamePlaylistNoRandomizePatch(bool apply) {
	NyaHookLib::Patch<uint16_t>(0x41E4D4, apply ? 0x27EB : 0x1A75);
}

tPlaylist gRallyPlaylists[5];
tPlaylist gCarnageModernPlaylist;
tPlaylist* pCurrentPlaylist[3] = {};

void LoadSoundtrackWithBackup(int id, tPlaylist* playlist) {
	FO2Vector<MusicInterface::tSong>* songs = nullptr;
	int* current = nullptr;
	int* next = nullptr;
	switch (id) {
		case 0:
			songs = &MusicInterface::gPlaylistTitle;
			current = &MusicInterface::gPlaylistTitleCurrent;
			next = &MusicInterface::gPlaylistTitleNext;
			snprintf(aPlaylistTitlePath, MAX_PATH, "%s%s.bed", aPlaylistBasePath, playlist->filename.c_str());
			break;
		case 1:
			songs = &MusicInterface::gPlaylistIngame;
			current = &MusicInterface::gPlaylistIngameCurrent;
			next = &MusicInterface::gPlaylistIngameNext;
			snprintf(aPlaylistIngamePath, MAX_PATH, "%s%s.bed", aPlaylistBasePath, playlist->filename.c_str());
			break;
		case 2:
			songs = &MusicInterface::gPlaylistStunt;
			current = &MusicInterface::gPlaylistStuntCurrent;
			next = &MusicInterface::gPlaylistStuntNext;
			snprintf(aPlaylistStuntPath, MAX_PATH, "%s%s.bed", aPlaylistBasePath, playlist->filename.c_str());
			break;
		default:
			break;
	}

	if (auto old = pCurrentPlaylist[id]) {
		old->gamePlaylistCurrent = *current;
		old->gamePlaylistNext = *next;
	}

	auto bak1 = MusicInterface::gPlaylistTitleCurrent;
	auto bak2 = MusicInterface::gPlaylistTitleNext;
	auto bak3 = MusicInterface::gPlaylistIngameCurrent;
	auto bak4 = MusicInterface::gPlaylistIngameNext;
	auto bak5 = MusicInterface::gPlaylistStuntCurrent;
	auto bak6 = MusicInterface::gPlaylistStuntNext;

	if (!playlist->gamePlaylist.begin) {
		songs->begin = nullptr;
		songs->end = nullptr;
		songs->capacity = nullptr;

		MusicInterface::LoadPlaylist(id);

		playlist->gamePlaylist = *songs;
		playlist->gamePlaylistCurrent = *current;
		playlist->gamePlaylistNext = *next;
 	}
	else {
		*songs = playlist->gamePlaylist;
		*current = playlist->gamePlaylistCurrent;
		*next = playlist->gamePlaylistNext;
	}

	if (id != 0) {
		auto numTitle = MusicInterface::gPlaylistTitle.GetSize();
		MusicInterface::gPlaylistTitleCurrent = bak1;
		MusicInterface::gPlaylistTitleNext = bak2;

		if (numTitle > 0) {
			if (MusicInterface::gPlaylistTitleCurrent >= 0) MusicInterface::gPlaylistTitleCurrent %= numTitle;
			MusicInterface::gPlaylistTitleNext %= numTitle;
		}
	}

	if (id != 1) {
		auto numIngame = MusicInterface::gPlaylistIngame.GetSize();
		MusicInterface::gPlaylistIngameCurrent = bak3;
		MusicInterface::gPlaylistIngameNext = bak4;

		if (numIngame > 0) {
			if (MusicInterface::gPlaylistIngameCurrent >= 0) MusicInterface::gPlaylistIngameCurrent %= numIngame;
			MusicInterface::gPlaylistIngameNext %= numIngame;
		}
	}

	if (id != 2) {
		auto numStunt = MusicInterface::gPlaylistStunt.GetSize();
		MusicInterface::gPlaylistStuntCurrent = bak5;
		MusicInterface::gPlaylistStuntNext = bak6;

		if (numStunt > 0) {
			if (MusicInterface::gPlaylistStuntCurrent >= 0) MusicInterface::gPlaylistStuntCurrent %= numStunt;
			MusicInterface::gPlaylistStuntNext %= numStunt;
		}
	}

	pCurrentPlaylist[id] = playlist;

	if (id == 1) {
		ApplyIngamePlaylistNoRandomizePatch(playlist->gamePlaylist.GetSize() <= 1);
	}
}

void SetSoundtrack() {
	if (!MusicInterface::bMusicLoaded) return;

	NyaHookLib::Patch<uint8_t>(0x41E264, pGameFlow->nGameRules == GR_STUNT && aStuntPlaylists[nIngameStuntSoundtrack].ShouldDisableStuntDisplay() ? 0xEB : 0x74);

	static tPlaylist* pLastSoundtrack = nullptr;
	static tPlaylist* pLastMenuSoundtrack = nullptr;
	static tPlaylist* pLastStuntSoundtrack = nullptr;

	// preload all soundtracks
	static bool bOnce = true;
	if (bOnce) {
		// per track playlists
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!DoesTrackValueExist(i, "MusicPlaylist")) continue;

			auto name = GetTrackValueString(i, "MusicPlaylist");
			if (!GetPlaylistByFilename(name)) {
				tPlaylist playlist;
				playlist.name = L"DUMMY";
				playlist.filename = name;
				aTrackPlaylists.push_back(playlist);
				LoadSoundtrackWithBackup(1, &aTrackPlaylists[aTrackPlaylists.size()-1]);
				pCurrentPlaylist[1] = nullptr;
			}
		}
		// regular playlists
		for (auto& playlist : aPlaylists) {
			LoadSoundtrackWithBackup(1, &playlist);
			pCurrentPlaylist[1] = nullptr;
		}
		if (!gCarnageModernPlaylist.filename.empty()) {
			LoadSoundtrackWithBackup(1, &gCarnageModernPlaylist);
			pCurrentPlaylist[1] = nullptr;
		}
		for (auto& playlist : gRallyPlaylists) {
			if (!playlist.filename.empty()) {
				LoadSoundtrackWithBackup(1, &playlist);
				pCurrentPlaylist[1] = nullptr;
			}
		}
		for (auto& playlist : aMenuPlaylists) {
			LoadSoundtrackWithBackup(0, &playlist);
			pCurrentPlaylist[0] = nullptr;
		}
		for (auto& playlist : aStuntPlaylists) {
			LoadSoundtrackWithBackup(2, &playlist);
			pCurrentPlaylist[2] = nullptr;
		}
		pLastSoundtrack = nullptr;
		pLastMenuSoundtrack = nullptr;
		pLastStuntSoundtrack = nullptr;
		bOnce = false;
	}

	if (pGameFlow && (pGameFlow->nGameState == GAME_STATE_RACE || !pLastSoundtrack)) {
		bool isCarnageRace = false;
		bool isRally = false;
		int rallyId = 0;

		int soundtrackId = nIngameSoundtrack;
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "UseFO1Soundtrack")) soundtrackId = nIngameFO1Soundtrack;
			if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "UseToughTrucksSoundtrack")) soundtrackId = nIngameTTSoundtrack;
			if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "UseRallyTrophySoundtrack")) {
				soundtrackId = nIngameRTSoundtrack;
				isRally = true;
				rallyId = GetTrackValueNumber(pGameFlow->PreRace.nLevel, "RallyTrophySoundtrackID")-1;
				if (rallyId < 0) rallyId = 0;
				if (rallyId > 4) rallyId = 4;
			}
			if (pGameFlow->nGameRules == GR_DERBY || pGameFlow->nDerbyType != DERBY_NONE) {
				soundtrackId = pGameFlow->nDerbyType == DERBY_FRAG ? nIngameFragDerbySoundtrack : nIngameDerbySoundtrack;
			}
			if (!bIsStuntMode && !bIsSpeedtrap && !bIsDriftEvent && pGameFlow->nGameRules == GR_ARCADE_RACE) {
				soundtrackId = nIngameArcadeRaceSoundtrack;
				isCarnageRace = true;
			}
			if (bIsStuntMode) soundtrackId = nIngameStuntShowSoundtrack;
			if (bIsDriftEvent) soundtrackId = nIngameDriftSoundtrack;
		}

		auto playlist = &aPlaylists[soundtrackId];
		if (isCarnageRace && playlist->filename == "playlist_ingamemodern") playlist = &gCarnageModernPlaylist;
		if (isRally && playlist->filename == "playlist_ingamerally") playlist = &gRallyPlaylists[rallyId];

		if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->PreRace.nLevel, "MusicPlaylist")) {
			if (auto trackPlaylist = GetPlaylistByFilename(GetTrackValueString(pGameFlow->PreRace.nLevel, "MusicPlaylist"))) {
				playlist = trackPlaylist;
			}
		}

		if (playlist != pLastSoundtrack) {
			LoadSoundtrackWithBackup(1, playlist);
			pLastSoundtrack = playlist;
		}
	}

	auto menuPlaylist = &aMenuPlaylists[nMenuSoundtrack];
	if (pLastMenuSoundtrack != menuPlaylist) {
		LoadSoundtrackWithBackup(0, menuPlaylist);

		if (pLastMenuSoundtrack) {
			auto data = tEventData(EVENT_MUSIC_STOP);
			pEventManager->SendEvent(&data);
			data = tEventData(EVENT_MUSIC_PLAY_TITLE);
			pEventManager->SendEvent(&data);
		}

		pLastMenuSoundtrack = menuPlaylist;
	}

	auto stuntPlaylist = &aStuntPlaylists[nIngameStuntSoundtrack];
	if (pLastStuntSoundtrack != stuntPlaylist) {
		LoadSoundtrackWithBackup(2, stuntPlaylist);
		pLastStuntSoundtrack = stuntPlaylist;
	}
}

auto SoundtrackSwapper_call = (void(*)())0x41D2C0;
void SoundtrackSwapper() {
	SetSoundtrack();
	SoundtrackSwapper_call();
}

auto SoundtrackSwapper2_call = (void(__thiscall*)())0x41FF10;
void SoundtrackSwapper2() {
	SetSoundtrack();
	SoundtrackSwapper2_call();
}

void ApplySoundtrackPatches() {
	static auto config = toml::parse_file("Config/Music.toml");
	int numPlaylists = config["main"]["playlist_count"].value_or(1);
	int numMenuPlaylists = config["main"]["menuplaylist_count"].value_or(1);
	int numStuntPlaylists = config["main"]["stuntplaylist_count"].value_or(1);
	int defaultMenu = config["main"]["default_menu"].value_or(1) - 1;
	int defaultIngame = config["main"]["default_race"].value_or(1) - 1;
	int defaultFO1 = config["main"]["default_fo1_race"].value_or(1) - 1;
	int defaultTT = config["main"]["default_tt_race"].value_or(1) - 1;
	int defaultRT = config["main"]["default_rt_race"].value_or(1) - 1;
	int defaultDerby = config["main"]["default_derby"].value_or(1) - 1;
	int defaultFragDerby = config["main"]["default_frag_derby"].value_or(1) - 1;
	int defaultArcade = config["main"]["default_arcade_race"].value_or(1) - 1;
	int defaultStunt = config["main"]["default_stunt"].value_or(1) - 1;
	int defaultStuntShow = config["main"]["default_stuntshow"].value_or(1) - 1;
	int defaultDrift = config["main"]["default_drift"].value_or(1) - 1;
	for (int i = 0; i < numPlaylists; i++) {
		tPlaylist playlist;
		playlist.name = config[std::format("playlist{}", i+1)]["name"].value_or(L"");
		playlist.filename = config[std::format("playlist{}", i+1)]["file"].value_or("");
		if (playlist.name.empty()) continue;
		if (playlist.filename.empty()) continue;
		aPlaylists.push_back(playlist);
		if (playlist.filename == "playlist_ingamemodern") {
			gCarnageModernPlaylist = playlist;
			gCarnageModernPlaylist.filename = "playlist_ingamemodern2";
		}
		if (playlist.filename == "playlist_ingamerally") {
			for (int j = 0; j < 5; j++) {
				gRallyPlaylists[j] = playlist;
				gRallyPlaylists[j].filename = std::format("playlist_ingamerally{}",j+1);
			}
		}
	}
	for (int i = 0; i < numMenuPlaylists; i++) {
		tPlaylist playlist;
		playlist.name = config[std::format("menuplaylist{}", i+1)]["name"].value_or(L"");
		playlist.filename = config[std::format("menuplaylist{}", i+1)]["file"].value_or("");
		if (playlist.name.empty()) continue;
		if (playlist.filename.empty()) continue;
		aMenuPlaylists.push_back(playlist);
	}
	for (int i = 0; i < numStuntPlaylists; i++) {
		tPlaylist playlist;
		playlist.name = config[std::format("stuntplaylist{}", i+1)]["name"].value_or(L"");
		playlist.filename = config[std::format("stuntplaylist{}", i+1)]["file"].value_or("");
		if (playlist.name.empty()) continue;
		if (playlist.filename.empty()) continue;
		aStuntPlaylists.push_back(playlist);
	}
	if (defaultMenu < 0 || defaultMenu >= aMenuPlaylists.size()) defaultMenu = 0;
	if (defaultStunt < 0 || defaultStunt >= aStuntPlaylists.size()) defaultStunt = 0;
	if (defaultIngame < 0 || defaultIngame >= aPlaylists.size()) defaultIngame = 0;
	if (defaultFO1 < 0 || defaultFO1 >= aPlaylists.size()) defaultFO1 = 0;
	if (defaultTT < 0 || defaultTT >= aPlaylists.size()) defaultTT = 0;
	if (defaultRT < 0 || defaultRT >= aPlaylists.size()) defaultRT = 0;
	if (defaultDerby < 0 || defaultDerby >= aPlaylists.size()) defaultDerby = 0;
	if (defaultFragDerby < 0 || defaultFragDerby >= aPlaylists.size()) defaultFragDerby = 0;
	if (defaultArcade < 0 || defaultArcade >= aPlaylists.size()) defaultArcade = 0;
	if (defaultStuntShow < 0 || defaultStuntShow >= aPlaylists.size()) defaultStuntShow = 0;
	if (defaultDrift < 0 || defaultDrift >= aPlaylists.size()) defaultDrift = 0;
	nMenuSoundtrack = defaultMenu;
	nIngameSoundtrack = defaultIngame;
	nIngameFO1Soundtrack = defaultFO1;
	nIngameTTSoundtrack = defaultTT;
	nIngameRTSoundtrack = defaultRT;
	nIngameDerbySoundtrack = defaultDerby;
	nIngameFragDerbySoundtrack = defaultFragDerby;
	nIngameArcadeRaceSoundtrack = defaultArcade;
	nIngameStuntSoundtrack = defaultStunt;
	nIngameStuntShowSoundtrack = defaultStuntShow;
	nIngameDriftSoundtrack = defaultDrift;

	for (auto& setting : aNewGameSettings) {
		if (setting.value == &nMenuSoundtrack) setting.maxValue = aMenuPlaylists.size()-1;
		if (setting.value == &nIngameStuntSoundtrack) setting.maxValue = aStuntPlaylists.size()-1;
		if (setting.value == &nIngameSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameFO1Soundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameTTSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameRTSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameDerbySoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameFragDerbySoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameArcadeRaceSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameStuntShowSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameDriftSoundtrack) setting.maxValue = aPlaylists.size()-1;
	}

	NyaHookLib::Patch(0x41D2C3 + 1, aPlaylistTitlePath);
	NyaHookLib::Patch(0x41D9B8 + 1, aPlaylistTitlePath);
	NyaHookLib::Patch(0x41D2E5 + 1, aPlaylistIngamePath);
	NyaHookLib::Patch(0x41D938 + 1, aPlaylistIngamePath);
	NyaHookLib::Patch(0x41D8B2 + 1, aPlaylistStuntPath);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41FE1D, &SoundtrackSwapper);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55BA6F, &SoundtrackSwapper2);
}