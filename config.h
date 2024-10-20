uint32_t nCareerHandlingDamage = 1;
uint32_t nCareerRallyTrophyCars = 0;
uint32_t nMenuSoundtrack = 0;
uint32_t nIngameSoundtrack = 0;
uint32_t nIngameDerbySoundtrack = 0;
uint32_t nIngameFO1Soundtrack = 3;
uint32_t nIngameTTSoundtrack = 0;
uint32_t nIngameRTSoundtrack = 0;
uint32_t nIngameStuntShowSoundtrack = 0;
uint32_t nOpponentCountType = 2;
uint32_t nPlayerModel = 0;
uint32_t nPlayerModelSkinID = 0;
uint32_t nAvatarID = 12;
uint32_t nArrowColor = 0;
uint32_t nHUDType = 0;
uint32_t nAIFudgeDisabled = 0;
//uint32_t nFragDerbyCrashRewards = 1;
uint32_t nNumAIProfiles = 11;
uint32_t nMPInterpolation = 1;
uint32_t nInterpMinRange = 50;
uint32_t nUseRelativePosition = 1;
uint32_t nHandlingMode = 0;
uint32_t nPlayerFlag = 0;
uint32_t nPlayerFlag2 = 0;
uint32_t nWindowedMode = 0;
uint32_t nHighCarCam = 0;
uint32_t nWidescreenMenu = 1;
uint32_t nWidescreenSafeZone = 1;
uint32_t nExplosionEffects = 1;
uint32_t nAirControlType = 0;
uint32_t nAirControlFlipType = 0;
uint32_t nLoadingSkip = 0;
uint32_t nPacenoteVolume = 100;
uint32_t nPacenoteType = 0;
int nFragDerbyRewardSlam = 100;
int nFragDerbyRewardSuperFlip = 250;
int nFragDerbyRewardPowerHit = 250;
int nFragDerbyRewardBlastOut = 500;
float fArcadeRacePositionMultiplierLenient[32] = {
		5,
		5,
		5,
		5,
		5,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		3,
		3,
		3,
		3,
		3,
		3,
		2,
		2,
		2,
		2,
		2,
		2,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
};
float fArcadeRacePositionMultiplierStunt[32] = {
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
};
float fArcadeRacePositionMultiplier[32];

bool bIsStuntMode = false;
bool bIsInMultiplayer = false;