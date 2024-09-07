tGameSetting aGameSettings[] = {
		{ "Version", "Settings", (void*)0x849300, 0, 0, 1e+06 },
		{ "Game", "ImperialUnits", (void*)0x849494, 1, 0, 0 },
		{ "Game", "OverlayGauges", (void*)0x849498, 1, 0, 0 },
		{ "Game", "IngameMap", (void*)0x84949C, 1, 0, 0 },
		{ "Game", "Ragdoll", (void*)0x8494A0, 1, 0, 0 },
		{ "Game", "DefaultPlayerName", (void*)0x8494DC, 3, 0, 0 },
		{ "Game", "Camera", (void*)0x8494B8, 0, 0, 9 },
		{ "Game", "Hud", (void*)0x8494CC, 0, 0, 4 },
		{ "Game", "PlayerModel", &nPlayerModel, 0, 0, 13 },
		{ "Game", "MPPortrait", &nAvatarID, 0, 0, 12 },
		{ "Game", "ArrowColor", &nArrowColor, 0, 0, 13 },
		//{ "Game", "ShowBonus", (void*)0x8494D4, 1, 0, 0 },
		{ "Game", "ShowTutorials", (void*)0x8494D8, 1, 0, 0 },
		{ "Game", "CareerHandlingDamage", &nCareerHandlingDamage, 0, 0, 3 },
		{ "Game", "Soundtrack", &nIngameSoundtrack, 0, 0, nNumPlaylists-1 },
		{ "Game", "DerbySoundtrack", &nIngameDerbySoundtrack, 0, 0, nNumPlaylists-1 },
		{ "Game", "FO1Soundtrack", &nIngameFO1Soundtrack, 0, 0, nNumPlaylists-1 },
		{ "Game", "MenuSoundtrack", &nMenuSoundtrack, 0, 0, nNumPlaylistsMenu-1 },
		{ "Game", "OpponentCount", &nOpponentCountType, 0, 0, 3 },
		{ "Game", "OpponentStrength", &nAIFudgeDisabled, 0, 0, 2 },
		{ "Game", "HUDType", &nHUDType, 0, 0, 1 },
		//{ "Game", "FragDerbyCrashRewards", &nFragDerbyCrashRewards, 1, 0, 0 },
		{ "Game", "SlideControlDisabled", &nSlideControlDisabled, 1, 0, 0 },
		{ "Game", "PlayerFlag", &nPlayerFlag, 0, 0, 12 },
		{ "Game", "PlayerFlag2", &nPlayerFlag2, 1, 0, 0 },
		{ "Game", "HighCarCam", &nHighCarCam, 1, 0, 0 },
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
		{ "Audio", "InterfaceMusicVolume", (void*)0x849548, 0, 0, 100 },
		{ "Audio", "InterfaceSfxVolume", (void*)0x849550, 0, 0, 100 },
		{ "Audio", "IngameMusicVolume", (void*)0x849548, 0, 0, 100 },
		{ "Audio", "IngameSfxVolume", (void*)0x849550, 0, 0, 100 },
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
		NyaHookLib::Patch(addr, &aGameSettings[0].category);
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
		NyaHookLib::Patch(addr, &aGameSettings[0].name);
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
		NyaHookLib::Patch(addr, &aGameSettings[0].value);
	}
	NyaHookLib::Patch(0x45885C, &aGameSettings[0].type);
	NyaHookLib::Patch(0x4588AE, &aGameSettings[0].minValue);
	NyaHookLib::Patch(0x458976, &aGameSettings[0].minValue);
	NyaHookLib::Patch(0x4588A8, &aGameSettings[0].maxValue);
	NyaHookLib::Patch(0x458957, &aGameSettings[0].maxValue);
}