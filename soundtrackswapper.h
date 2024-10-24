char aPlaylistTitleBasePath[64] = "data/music/";
char aPlaylistIngameBasePath[64] = "data/music/";
char aPlaylistTitlePath[64] = "data/music/playlist_title.bed";
char aPlaylistIngamePath[64] = "data/music/playlist_ingame.bed";

struct tPlaylist {
	std::string filename;
	std::wstring name;
};
std::vector<tPlaylist> aPlaylists;
std::vector<tPlaylist> aMenuPlaylists;

auto LoadSoundtrack = (void(*)(int))0x41D870;
void SetSoundtrack() {
	static int nLastMenuSoundtrack = -1;

	snprintf(aPlaylistTitlePath, 64, "%s%s.bed", aPlaylistTitleBasePath, aMenuPlaylists[nMenuSoundtrack].filename.c_str());

	static int nLastSoundtrackId = -1;
	if (pGameFlow && (pGameFlow->nGameState == GAME_STATE_RACE || nLastSoundtrackId < 0)) {
		int soundtrackId = nIngameSoundtrack;
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			if (DoesTrackValueExist(pGameFlow->nLevelId, "UseFO1Soundtrack")) soundtrackId = nIngameFO1Soundtrack;
			if (DoesTrackValueExist(pGameFlow->nLevelId, "UseToughTrucksSoundtrack")) soundtrackId = nIngameTTSoundtrack;
			if (DoesTrackValueExist(pGameFlow->nLevelId, "UseRallyTrophySoundtrack")) soundtrackId = nIngameRTSoundtrack;
			if (pGameFlow->nGameRules == GR_DERBY || pGameFlow->nDerbyType != DERBY_NONE) soundtrackId = nIngameDerbySoundtrack;
			if (bIsStuntMode) soundtrackId = nIngameStuntShowSoundtrack;
		}

		snprintf(aPlaylistIngamePath, 64, "%s%s.bed", aPlaylistIngameBasePath, aPlaylists[soundtrackId].filename.c_str());

		if (soundtrackId != nLastSoundtrackId) {
			LoadSoundtrack(1);
			nLastSoundtrackId = soundtrackId;
		}
	}

	if (nLastMenuSoundtrack != nMenuSoundtrack) {
		LoadSoundtrack(0);

		if (nLastMenuSoundtrack >= 0) {
			auto data = tEventData(EVENT_MUSIC_STOP);
			pEventManager->SendEvent(&data);
			data = tEventData(EVENT_MUSIC_PLAY_TITLE);
			pEventManager->SendEvent(&data);
		}

		nLastMenuSoundtrack = nMenuSoundtrack;
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
	int defaultMenu = config["main"]["default_menu"].value_or(1) - 1;
	int defaultIngame = config["main"]["default_race"].value_or(1) - 1;
	int defaultFO1 = config["main"]["default_fo1_race"].value_or(1) - 1;
	int defaultTT = config["main"]["default_tt_race"].value_or(1) - 1;
	int defaultRT = config["main"]["default_rt_race"].value_or(1) - 1;
	int defaultDerby = config["main"]["default_derby"].value_or(1) - 1;
	int defaultStuntShow = config["main"]["default_stuntshow"].value_or(1) - 1;
	for (int i = 0; i < numPlaylists; i++) {
		tPlaylist playlist;
		playlist.name = config[std::format("playlist{}", i+1)]["name"].value_or(L"");
		playlist.filename = config[std::format("playlist{}", i+1)]["file"].value_or("");
		if (playlist.name.empty()) continue;
		if (playlist.filename.empty()) continue;
		aPlaylists.push_back(playlist);
	}
	for (int i = 0; i < numMenuPlaylists; i++) {
		tPlaylist playlist;
		playlist.name = config[std::format("menuplaylist{}", i+1)]["name"].value_or(L"");
		playlist.filename = config[std::format("menuplaylist{}", i+1)]["file"].value_or("");
		if (playlist.name.empty()) continue;
		if (playlist.filename.empty()) continue;
		aMenuPlaylists.push_back(playlist);
	}
	if (defaultMenu < 0 || defaultMenu >= aMenuPlaylists.size()) defaultMenu = 0;
	if (defaultIngame < 0 || defaultIngame >= aPlaylists.size()) defaultIngame = 0;
	if (defaultFO1 < 0 || defaultFO1 >= aPlaylists.size()) defaultFO1 = 0;
	if (defaultTT < 0 || defaultTT >= aPlaylists.size()) defaultTT = 0;
	if (defaultRT < 0 || defaultRT >= aPlaylists.size()) defaultRT = 0;
	if (defaultDerby < 0 || defaultDerby >= aPlaylists.size()) defaultDerby = 0;
	if (defaultStuntShow < 0 || defaultStuntShow >= aPlaylists.size()) defaultStuntShow = 0;
	nMenuSoundtrack = defaultMenu;
	nIngameSoundtrack = defaultIngame;
	nIngameFO1Soundtrack = defaultFO1;
	nIngameTTSoundtrack = defaultTT;
	nIngameRTSoundtrack = defaultRT;
	nIngameDerbySoundtrack = defaultDerby;
	nIngameStuntShowSoundtrack = defaultStuntShow;

	for (auto& setting : aNewGameSettings) {
		if (setting.value == &nMenuSoundtrack) setting.maxValue = aMenuPlaylists.size()-1;
		if (setting.value == &nIngameSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameFO1Soundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameTTSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameRTSoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameDerbySoundtrack) setting.maxValue = aPlaylists.size()-1;
		if (setting.value == &nIngameStuntShowSoundtrack) setting.maxValue = aPlaylists.size()-1;
	}

	NyaHookLib::Patch(0x41D2C3 + 1, aPlaylistTitlePath);
	NyaHookLib::Patch(0x41D9B8 + 1, aPlaylistTitlePath);
	NyaHookLib::Patch(0x41D2E5 + 1, aPlaylistIngamePath);
	NyaHookLib::Patch(0x41D938 + 1, aPlaylistIngamePath);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41FE1D, &SoundtrackSwapper);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55BA6F, &SoundtrackSwapper2);
}