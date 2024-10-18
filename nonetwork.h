class tDummyStatisticsManager {
public:
	bool bExited = false;

	void Exit() {
		if (bExited) return;
		auto data = tEventData(2022);
		pEventManager->PostEvent(&data);
		bExited = true;
	}

	virtual void _dtor(char a2) {
		Exit();
		if ((a2 & 1) != 0) {
			((void(*)(void*))0x50BEC0)(this);
		}
	}
	virtual void _vf1() { }
	virtual void Process(int ms) {
		if (pGameFlow->nRaceState != RACE_STATE_WANTSTOQUIT) return;
		Exit();
	}
};

tDummyStatisticsManager* __stdcall InitDummyStatisticsManager(tDummyStatisticsManager* manager) {
	auto tmp = new tDummyStatisticsManager;
	memcpy(manager, tmp, sizeof(tDummyStatisticsManager));
	delete tmp;
	return manager;
}

void DummyInitOnlineHighscoresLUA() {
	*(const char**)0x92990F4 = "OnlineHighscores";
	*(int*)0x92990F8 = 0;
	*(int*)0x92994A0 = 0;
	*(int*)0x92994A4 = 0;
	*(int*)0x92994A8 = 0;
	*(int*)0x92994AC = 0;
	*(int*)0x92994B0 = 0;
}

void ApplyNoNetworkPatches() {
	// highscoremanager read at:
	// 00492ACD when finishing a lap
	// 00465305 and 004703A0 when quitting
	// 00468FE8 when loaded into menu
	// uploading race results is lang entry 246 (0xF6), called at 4AF733 for event 3015, from event 7025
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x465270, 0x46543C); // no highscore updating
	NyaHookLib::Patch<uint8_t>(0x4675E6, 0xEB); // no uploading race results
	NyaHookLib::Patch<uint8_t>(0x492B0F, 0xEB); // no new lap record popup

	// remove onlinehighscores
	NyaHookLib::Patch<uint8_t>(0x558C7E, 0xEB); // no onlinehighscores init
	//NyaHookLib::Patch<uint8_t>(0x54DDA0, 0xC3); // no onlinehighscores script init
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x54DDA0, &DummyInitOnlineHighscoresLUA);
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4A5ACF, 0x4A5B08); // no StatisticsManager init
	// quit to menu is at 004654C9, event 7E6, if +2738 is 11
	// set to 11 by event 0x2339, works fine already
	// event 7E6 is not fired, supposed to be at 005575D6
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4A5AFD, &InitDummyStatisticsManager);
}