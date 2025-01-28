tGameSetting aNewGameSettings[] = {
		{ "Version", "Settings", (void*)0x849300, 0, 0, 1e+06 },
		{ "Game", "ImperialUnits", (void*)0x849494, 1, 0, 0 },
		{ "Game", "OverlayGauges", (void*)0x849498, 1, 0, 0 },
		{ "Game", "IngameMap", (void*)0x84949C, 1, 0, 0 },
		{ "Game", "Ragdoll", (void*)0x8494A0, 1, 0, 0 },
		{ "Game", "DefaultPlayerName", (void*)0x8494DC, 3, 0, 0 },
		{ "Game", "Camera", (void*)0x8494B8, 0, 0, 9 },
		{ "Game", "Hud", (void*)0x8494CC, 0, 0, 4 },
		{ "Game", "PlayerModel", &nPlayerModel, 0, 0, 24 },
		{ "Game", "MPPortrait", &nAvatarID, 0, 0, 12 },
		{ "Game", "ArrowColor", &nArrowColor, 0, 0, 13 },
		{ "Game", "ChatColor", &nChatColor, 0, 0, 100 },
		//{ "Game", "ShowBonus", (void*)0x8494D4, 1, 0, 0 },
		{ "Game", "ShowTutorials", (void*)0x8494D8, 1, 0, 0 },
		{ "Game", "CareerHandlingDamage", &nCareerHandlingDamage, 1, 0, 0 },
		{ "Game", "Soundtrack", &nIngameSoundtrack, 0, 0, 1 },
		{ "Game", "DerbySoundtrack", &nIngameDerbySoundtrack, 0, 0, 1 },
		{ "Game", "FragDerbySoundtrack", &nIngameFragDerbySoundtrack, 0, 0, 1 },
		{ "Game", "ArcadeRaceSoundtrack", &nIngameArcadeRaceSoundtrack, 0, 0, 1 },
		{ "Game", "FO1Soundtrack", &nIngameFO1Soundtrack, 0, 0, 1 },
		{ "Game", "TTSoundtrack", &nIngameTTSoundtrack, 0, 0, 1 },
		{ "Game", "RallySoundtrack", &nIngameRTSoundtrack, 0, 0, 1 },
		{ "Game", "StuntSoundtrack", &nIngameStuntSoundtrack, 0, 0, 1 },
		{ "Game", "StuntModeSoundtrack", &nIngameStuntShowSoundtrack, 0, 0, 1 },
		{ "Game", "DriftSoundtrack", &nIngameDriftSoundtrack, 0, 0, 1 },
		{ "Game", "MenuSoundtrack", &nMenuSoundtrack, 0, 0, 1 },
		{ "Game", "OpponentCountCareer", &nOpponentCountTypeCareer, 0, 0, 1 },
		{ "Game", "OpponentCountCarnage", &nOpponentCountTypeArcade, 0, 0, 1 },
		{ "Game", "OpponentCountRally", &nOpponentCountTypeRally, 0, 0, 2 },
		{ "Game", "OpponentCountSingleEvent", &nOpponentCountTypeSingleEvent, 0, 0, 3 },
		{ "Game", "OpponentStrength", &nAIFudgeDisabled, 0, 0, 2 },
		{ "Game", "HUDType", &nHUDType, 0, 0, 1 },
		//{ "Game", "FragDerbyCrashRewards", &nFragDerbyCrashRewards, 1, 0, 0 },
		{ "Game", "HandlingMode", &nHandlingMode, 0, 0, NUM_HANDLING_MODES-1 },
		{ "Game", "PlayerFlag", &nPlayerFlag, 0, 0, 14 },
		{ "Game", "PlayerFlag2", &nPlayerFlag2, 0, 0, 2 },
		{ "Game", "HighCarCam", &nHighCarCam, 1, 0, 0 },
		{ "Game", "WidescreenMenu", &nWidescreenMenu, 1, 0, 0 },
		{ "Game", "WidescreenSafeZone", &nWidescreenSafeZone, 0, 0, 4 },
		{ "Game", "LoadingSkip", &nLoadingSkip, 0, 0, 2 },
		{ "Game", "ShowSuperAuthors", &nShowSuperAuthors, 0, 0, 2 },
		{ "Game", "DisplaySplits", &nDisplaySplits, 0, 0, 3 },
		{ "Game", "SplitType", &nSplitType, 0, 0, 2 },
		{ "Control", "ControllerGuid", (void*)0x845D98, 3, 0, 0 },
		{ "Control", "Controller", (void*)0x845DBC, 0, 0, 2 },
		{ "Control", "ForceFeedback", (void*)0x845DC0, 1, 0, 0 },
		{ "Control", "ForceMagnitude", (void*)0x845DCC, 0, 0, 100 },
		{ "Control", "ControllerSensitivity", (void*)0x845DD0, 0, 0, 100 },
		{ "Control", "ControllerDeadzone", (void*)0x845DD4, 0, 0, 100 },
		{ "Control", "ControllerSaturation", (void*)0x845DD8, 0, 0, 100 },
		{ "Control", "ControllerLayout", (void*)0x845DDC, 0, 0, 10 },
		{ "Control", "Transmission", (void*)0x845DC8, 1, 0, 0 },
		{ "Control", "UsePedals", (void*)0x845DC4, 1, 0, 0 },
		{ "Control", "DigitalCenteringSpeed", (void*)0x845E3C, 2, 0, 100 },
		{ "Control", "DigitalSteeringMaxSpeed", (void*)0x845E40, 2, 0, 100 },
		{ "Control", "DigitalSteeringMinSpeed", (void*)0x845E44, 2, 0, 100 },
		{ "Control", "AirControlType", &nAirControlType, 0, 0, 1 },
		{ "Control", "AirControlFlipType", &nAirControlFlipType, 0, 0, 1 },
		{ "Visual", "Windowed", &nWindowedMode, 1, 0, 0 },
		{ "Visual", "AlphaBlend", (void*)0x8494F0, 1, 0, 0 },
		{ "Visual", "SunFlare", (void*)0x8494F8, 1, 0, 0 },
		{ "Visual", "TrackDetail", (void*)0x849504, 0, 0, 2 },
		{ "Visual", "PS2MotionBlurIngame", (void*)0x849508, 0, 0, 255 },
		{ "Visual", "Brightness", (void*)0x84950C, 0, 0, 100 },
		{ "Visual", "TextureQuality", (void*)0x849500, 0, 0, 2 },
		{ "Visual", "Visibility", (void*)0x849510, 0, 0, 100 },
		{ "Visual", "QualityPreset", (void*)0x849514, 0, 0, 3 },
		{ "Visual", "ReflectionQuality", (void*)0x849518, 0, 0, 2 },
		{ "Visual", "ParticleQuality", (void*)0x84951C, 0, 0, 2 },
		{ "Visual", "ShadowQuality", (void*)0x849520, 0, 0, 2 },
		{ "Visual", "FoliageQuality", (void*)0x849524, 0, 0, 2 },
		{ "Visual", "CustomPresetTextureQuality", (void*)0x849528, 0, 0, 2 },
		{ "Visual", "CustomPresetVisibility", (void*)0x84952C, 0, 0, 100 },
		{ "Visual", "CustomPresetReflectionQuality", (void*)0x849530, 0, 0, 2 },
		{ "Visual", "CustomPresetParticleQuality", (void*)0x849534, 0, 0, 2 },
		{ "Visual", "CustomPresetShadowQuality", (void*)0x849538, 0, 0, 2 },
		{ "Visual", "CustomPresetFoliageQuality", (void*)0x84953C, 0, 0, 2 },
		{ "Visual", "MotionBlur", (void*)0x7139AC, 1, 0, 0 },
		{ "Visual", "ExplosionEffects", &nExplosionEffects, 0, 0, 2 },
		{ "Audio", "InterfaceMusicVolume", (void*)0x849548, 0, 0, 100 },
		{ "Audio", "InterfaceSfxVolume", (void*)0x849550, 0, 0, 100 },
		{ "Audio", "IngameMusicVolume", (void*)0x849548, 0, 0, 100 },
		{ "Audio", "IngameSfxVolume", (void*)0x849550, 0, 0, 100 },
		{ "Audio", "RallyMusicVolume", &nRallyMusicVolume, 0, 0, 100 },
		{ "Audio", "PacenoteVolume", &nPacenoteVolume, 0, 0, 100 },
		{ "Audio", "PacenoteType", &nPacenoteType, 0, 0, 2 },
		{ "Audio", "PacenoteVisualType", &nPacenoteVisualType, 0, 0, 2 },
		{ "Audio", "DopplerEffects", (void*)0x849554, 1, 0, 0 },
		{ "Audio", "ChannelMode", (void*)0x84955C, 0, 0, 2 },
		{ "Network", "Port", (void*)0x849560, 0, 0, 65536 },
		{ "Network", "BroadcastPort", (void*)0x849564, 0, 0, 65536 },
		{ "Network", "MutePlayerVoice", (void*)0x849568, 1, 0, 0 },
		{ "Network", "MuteVoiceIngame", (void*)0x84956C, 1, 0, 0 },
		{ "Network", "VoiceOutputVolume", (void*)0x849570, 0, 0, 100 },
		{ "Network", "VoiceTalkingLevel", (void*)0x849574, 2, 0, 90 },
		{ "Network", "VoiceJitterMaxDelay", (void*)0x84957C, 0, 0, 100 },
		{ "Network", "VoiceJitterMaxVariation", (void*)0x849580, 0, 0, 100 },
		{ "Network", "VoiceJitterInitialVariation", (void*)0x849578, 0, 0, 100 },
		{ "Network", "Interpolation", &nMPInterpolation, 0, 0, 1 },
		{ "Network", "InterpMinRange", &nInterpMinRange, 0, 0, 100 },
		{ "Network", "UseRelativePosition", &nUseRelativePosition, 0, 0, 1 },
		{ "Network", "DefaultTickRate", &nDefaultTickRate, 0, 0, 4 },
		{ "Network", "AllowCheats", &nAllowCheats, 1, 0, 0 },
		{ "Network", "JoinNotifs", &nJoinNotifs, 1, 0, 0 },
		{ nullptr, nullptr, nullptr, 0, 0, 0 },
};

void WriteSettingsToTestFile() {
	auto file = std::ofstream("test.cpp");
	auto setting = (tGameSetting*)0x71B518;
	while (setting->category) {
		file << std::format("{{ \"{}\", \"{}\", (void*)0x{:X}, {}, {}, {} }},\n", setting->category, setting->name, (uintptr_t)setting->value, setting->type, setting->minValue, setting->maxValue);
		setting++;
	}
}

void SetNewDefaultOptions() {
	SetDefaultOptions();

	// halve music & sfx volume by default
	*(int*)0x849550 = 25; // sfx volume
	*(int*)0x849548 = 30; // music volume
}

auto InitXInputController = (void(__thiscall*)(GameController*))0x5BA340;
void __fastcall SetControllerLayout(GameController* pThis) {
	InitXInputController(pThis);
	pThis->SetControllerLayout((*(int*)0x845DDC) + 1);
}

void ApplyCustomSettingsPatches() {
	uintptr_t aCategoryAddresses[] = {
			0x45882C,
			0x45888D,
			0x458933,
			0x458969,
			0x4589AA,
			0x458A18,
			0x459135,
			0x45915E,
			0x4591C8,
			0x4591EC,
			0x45932F,
			0x45935D,
			0x4593C9,
			0x4593EC,
			0x459680,
			0x459688,
			0x459750,
			0x459758,
	};
	for (auto& addr : aCategoryAddresses) {
		NyaHookLib::Patch(addr, &aNewGameSettings[0].category);
	}
	uintptr_t aNameAddresses[] = {
			0x458842,
			0x458887,
			0x458923,
			0x458963,
			0x4589A4,
			0x459143,
			0x459342,
	};
	for (auto& addr : aNameAddresses) {
		NyaHookLib::Patch(addr, &aNewGameSettings[0].name);
	}
	uintptr_t aValueAddresses[] = {
			0x45884F,
			0x458872,
			0x458910,
			0x45895D,
			0x45899E,
			0x459151,
			0x459350,
	};
	for (auto& addr : aValueAddresses) {
		NyaHookLib::Patch(addr, &aNewGameSettings[0].value);
	}
	NyaHookLib::Patch(0x45885C, &aNewGameSettings[0].type);
	NyaHookLib::Patch(0x4588AE, &aNewGameSettings[0].minValue);
	NyaHookLib::Patch(0x458976, &aNewGameSettings[0].minValue);
	NyaHookLib::Patch(0x4588A8, &aNewGameSettings[0].maxValue);
	NyaHookLib::Patch(0x458957, &aNewGameSettings[0].maxValue);

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4582AB, &SetNewDefaultOptions);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4D12B0, &SetNewDefaultOptions);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55B1F5, &SetNewDefaultOptions);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55B618, &SetNewDefaultOptions);

	//InitXInputController = (void(__thiscall*)(GameController*))(*(uintptr_t*)0x6F4040);
	//NyaHookLib::Patch(0x6F4040, &SetControllerLayout);
}