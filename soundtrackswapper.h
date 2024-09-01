char aPlaylistTitleBasePath[64] = "data/music/playlist_title";
char aPlaylistIngameBasePath[64] = "data/music/playlist_ingame";
char aPlaylistTitlePath[64] = "data/music/playlist_title.bed";
char aPlaylistIngamePath[64] = "data/music/playlist_ingame.bed";

auto LoadSoundtrack = (void(*)(int))0x41D870;
void SetSoundtrack() {
	static int nLastSoundtrack = -1;

	const char *suffix = "";
	if (nSoundtrack == 1) suffix = "fo2";
	if (nSoundtrack == 2) suffix = "fouc";
	if (nSoundtrack == 3) suffix = "fo1";

	// no fo1 menu music because we can't have nice things
	if (nSoundtrack == 3) {
		snprintf(aPlaylistTitlePath, 64, "%s.bed", aPlaylistTitleBasePath);
	}
	else {
		snprintf(aPlaylistTitlePath, 64, "%s%s.bed", aPlaylistTitleBasePath, suffix);
	}
	snprintf(aPlaylistIngamePath, 64, "%s%s.bed", aPlaylistIngameBasePath, suffix);

	if (nLastSoundtrack != nSoundtrack) {
		LoadSoundtrack(0);
		LoadSoundtrack(1);
		nLastSoundtrack = nSoundtrack;
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