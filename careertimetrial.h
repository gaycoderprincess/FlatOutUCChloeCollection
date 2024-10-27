bool bIsCareerTimeTrial = false;
int nCareerTimeTrialCar = 0;
bool bCareerTimeTrialUpgrades = false;
uint32_t nCareerTimeTrialMedalTimes[5];
uint32_t nCareerTimeTrialEventClass;
uint32_t nCareerTimeTrialEventId;
uint32_t nCareerTimeTrialStartPosition;
enum eCareerTimeTrialMedal {
	MEDAL_GOLD,
	MEDAL_SILVER,
	MEDAL_BRONZE,
	MEDAL_AUTHOR,
	MEDAL_SAUTHOR
};

void ApplyCareerTimeTrialPatches(bool apply) {
	bIsTimeTrial = apply;
	bIsCareerTimeTrial = apply;
	if (!apply) bSkipTuningThisRace = false;
	NyaHookLib::Patch<uint8_t>(0x46937D, apply ? 0xEB : 0x74); // never use career active car
	NyaHookLib::Patch<uint64_t>(0x469BFE, apply ? 0xF883909090909090 : 0xF8830000020A840F); // use UpgradeLevel
	NyaHookLib::Patch<uint8_t>(0x469CE1, apply && bCareerTimeTrialUpgrades ? 0xEB : 0x74); // use UpgradeLevel SingleRace
	NyaHookLib::Patch(0x4DC0FF + 1, apply ? "Data.Overlay.HUD.ChloeTimeTrial" : "Data.Overlay.HUD.Race"); // use UpgradeLevel SingleRace
}

void SetCareerTimeTrialPlayerColor(Player* ply) {
	if (!ply) return;

	auto tmp = *(NyaDrawing::CNyaRGBA32*)&ply->nArrowColor;
	auto color = NyaDrawing::CNyaRGBA32(tmp.b, tmp.g, tmp.r, tmp.a);
	switch (ply->nPlayerId) {
		// gold
		case 2:
			color.r = 236;
			color.g = 221;
			color.b = 16;
			break;
		// silver
		case 3:
			color.r = 186;
			color.g = 186;
			color.b = 186;
			break;
		// bronze
		case 4:
			color.r = 175;
			color.g = 100;
			color.b = 0;
			break;
		// author
		case 5:
			color.r = 30;
			color.g = 160;
			color.b = 0;
			break;
		// super author
		case 6:
			color.r = 219;
			color.g = 100;
			color.b = 193;
			break;
		default:
			break;
	}
	tmp = color;
	tmp.r = color.b;
	tmp.b = color.r;
	ply->nArrowColor = *(uint32_t*)&tmp;
}

void ProcessCareerTimeTrial() {
	if (bIsTimeTrial && pGameFlow->nGameState == GAME_STATE_RACE && GetPlayer(0)) {
		GetPlayer(0)->nPlayerStartPosition = bIsCareerTimeTrial ? nCareerTimeTrialStartPosition : 1;
	}

	if (!bIsCareerTimeTrial) return;
	if (pGameFlow->nGameMode != GM_CAREER) return;
	bSkipTuningThisRace = true;
	pGameFlow->nInstantActionCar = nCareerTimeTrialCar;
	pGameFlow->nInstantActionCarSkin = 1;

	if (pGameFlow->nGameState == GAME_STATE_RACE && GetPlayer(0)) {
		for (int i = 1; i < 32; i++) {
			SetCareerTimeTrialPlayerColor(GetPlayer(i));
		}
	}
}