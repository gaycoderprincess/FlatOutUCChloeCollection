bool bIsCareerTimeTrial = false;
int nCareerTimeTrialCar = 0;
bool bCareerTimeTrialUpgrades = false;

void ApplyCareerTimeTrialPatches(bool apply) {
	bIsCareerTimeTrial = true;
	if (!apply) bSkipTuningThisRace = false;
	NyaHookLib::Patch<uint8_t>(0x46937D, apply ? 0xEB : 0x74); // never use career active car
	NyaHookLib::Patch<uint64_t>(0x469BFE, apply ? 0xF883909090909090 : 0xF8830000020A840F); // use UpgradeLevel
	NyaHookLib::Patch<uint8_t>(0x469CE1, apply && bCareerTimeTrialUpgrades ? 0xEB : 0x74); // use UpgradeLevel SingleRace
}

void ProcessCareerTimeTrial() {
	if (!bIsCareerTimeTrial) return;
	if (pGameFlow->nGameMode != GM_CAREER) return;
	bSkipTuningThisRace = true;
	pGameFlow->nInstantActionCar = nCareerTimeTrialCar;
	pGameFlow->nInstantActionCarSkin = 1;
}