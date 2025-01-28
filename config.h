enum eHandlingMode {
	HANDLING_NORMAL,
	HANDLING_NORMAL_FO2DOWNFORCE,
	HANDLING_PROFESSIONAL,
	HANDLING_BETA,
	HANDLING_NORMAL_LEGACY,
	NUM_HANDLING_MODES
};

uint32_t nCareerHandlingDamage = 0;
uint32_t nMenuSoundtrack = 0;
uint32_t nIngameSoundtrack = 0;
uint32_t nIngameDerbySoundtrack = 0;
uint32_t nIngameFragDerbySoundtrack = 0;
uint32_t nIngameArcadeRaceSoundtrack = 0;
uint32_t nIngameFO1Soundtrack = 3;
uint32_t nIngameTTSoundtrack = 0;
uint32_t nIngameRTSoundtrack = 0;
uint32_t nIngameStuntSoundtrack = 0;
uint32_t nIngameStuntShowSoundtrack = 0;
uint32_t nIngameDriftSoundtrack = 0;
uint32_t nOpponentCountTypeCareer = 1; // 11
uint32_t nOpponentCountTypeArcade = 1; // numaiprofiles
uint32_t nOpponentCountTypeRally = 2; // numaiprofiles
uint32_t nOpponentCountTypeSingleEvent = 2; // numaiprofiles
uint32_t nPlayerModel = 0;
uint32_t nPlayerModelSkinID = 0;
uint32_t nAvatarID = 12;
uint32_t nArrowColor = 0;
uint32_t nChatColor = 0;
uint32_t nHUDType = 0;
uint32_t nAIFudgeDisabled = 1;
//uint32_t nFragDerbyCrashRewards = 1;
uint32_t nMPInterpolation = 1;
uint32_t nInterpMinRange = 50;
uint32_t nUseRelativePosition = 1;
uint32_t nDefaultTickRate = 1;
uint32_t nAllowCheats = 0;
uint32_t nJoinNotifs = 0;
uint32_t nHandlingMode = HANDLING_NORMAL;
uint32_t nPlayerFlag = 0;
uint32_t nPlayerFlag2 = 0;
uint32_t nWindowedMode = 0;
uint32_t nHighCarCam = 0;
uint32_t nWidescreenMenu = 1;
uint32_t nWidescreenSafeZone = 1;
uint32_t nExplosionEffects = 1;
uint32_t nAirControlType = 0;
uint32_t nAirControlFlipType = 0;
uint32_t nLoadingSkip = 2;
uint32_t nPacenoteVolume = 90;
uint32_t nPacenoteType = 2;
uint32_t nPacenoteVisualType = 2;
uint32_t nShowSuperAuthors = 0;
uint32_t nRallyMusicVolume = 45;
uint32_t nDisplaySplits = 1;
uint32_t nSplitType = 1;

uint32_t nNumAIProfiles = 11;
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

bool bIsTimeTrial = false;
bool bIsStuntMode = false;
bool bIsSpeedtrap = false;
bool bIsDriftEvent = false;
bool bIsLapKnockout = false;
bool bIsInMultiplayer = false;
bool bIsCareerRally = false;