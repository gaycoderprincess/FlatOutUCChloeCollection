char aPlaylistTitleBasePath[64] = "data/music/playlist_title";
char aPlaylistIngameBasePath[64] = "data/music/playlist_ingame";
char aPlaylistTitlePath[64] = "data/music/playlist_title.bed";
char aPlaylistIngamePath[64] = "data/music/playlist_ingame.bed";

const char* aPlaylists[] = {
		"",
		"fo2",
		"fouc",
		"fo1",
};
const int nNumPlaylists = sizeof(aPlaylists) / sizeof(aPlaylists[0]);

const char* aPlaylistsMenu[] = {
		"",
		"fo2",
		"fouc",
};
const int nNumPlaylistsMenu = sizeof(aPlaylistsMenu) / sizeof(aPlaylistsMenu[0]);

auto LoadSoundtrack = (void(*)(int))0x41D870;
void SetSoundtrack() {
	static int nLastMenuSoundtrack = -1;

	snprintf(aPlaylistTitlePath, 64, "%s%s.bed", aPlaylistTitleBasePath, aPlaylistsMenu[nMenuSoundtrack]);

	if (auto game = pGame) {
		static int nLastSoundtrackId = -1;

		int soundtrackId = nIngameSoundtrack;
		if (game->nLevelId >= TRACK_FO1PIT1A) soundtrackId = nIngameFO1Soundtrack;
		if (game->nGameRules == GR_DERBY || game->nDerbyType != DERBY_NONE) soundtrackId = nIngameDerbySoundtrack;

		snprintf(aPlaylistIngamePath, 64, "%s%s.bed", aPlaylistIngameBasePath, aPlaylists[soundtrackId]);

		if (soundtrackId != nLastSoundtrackId) {
			LoadSoundtrack(1);
			nLastSoundtrackId = soundtrackId;
		}
	}

	if (nLastMenuSoundtrack != nMenuSoundtrack) {
		LoadSoundtrack(0);
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
	NyaHookLib::Patch(0x41D2C3 + 1, aPlaylistTitlePath);
	NyaHookLib::Patch(0x41D9B8 + 1, aPlaylistTitlePath);
	NyaHookLib::Patch(0x41D2E5 + 1, aPlaylistIngamePath);
	NyaHookLib::Patch(0x41D938 + 1, aPlaylistIngamePath);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x41FE1D, &SoundtrackSwapper);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55BA6F, &SoundtrackSwapper2);
}