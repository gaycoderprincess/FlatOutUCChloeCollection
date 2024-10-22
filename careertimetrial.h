bool bIsCareerTimeTrial = false;
int nCareerTimeTrialCar = 0;

void ApplyCareerTimeTrialPatches(bool apply) {
	bIsCareerTimeTrial = true;
	NyaHookLib::Patch<uint8_t>(0x46937D, apply ? 0xEB : 0x74); // never use career active car
	NyaHookLib::Patch<uint64_t>(0x469BFE, apply ? 0xF883909090909090 : 0xF8830000020A840F); // use UpgradeLevel
}

void ProcessCareerTimeTrial() {
	if (!bIsCareerTimeTrial) return;
	pGameFlow->nInstantActionCar = nCareerTimeTrialCar;

	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		bIsCareerTimeTrial = false;
	}
}