void DrawDebugMenuViewerOption(const std::string& name, const std::string& desc = "") {
	if (DrawMenuOption(name, desc, false, false, true)) {
		ChloeMenuLib::SetEnterHint("");
	}
}

// copied from lua
std::string GetTimeString(double time_in_sec) {
	int64_t h = std::floor(time_in_sec / 3600.0);
	int64_t m = std::floor(((int)time_in_sec % 3600) / 60.0);
	int64_t s = (int)time_in_sec % 60;
	return std::format("{}:{:02}:{:02}", h, m, s);
}

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption(std::format("Force Alternate Ray Carter - {}", bForceRayAltProfile), "", false, false)) {
		bForceRayAltProfile = !bForceRayAltProfile;
	}

	if (DrawMenuOption("Playtime Stats")) {
		ChloeMenuLib::BeginMenu();

		for (int i = 0; i < NUM_PLAYTIME_TYPES; i++) {
			DrawDebugMenuViewerOption(std::format("{} - {}", aPlaytimeTypeNames[i], GetTimeString(gCustomSave.playtime[i])));
		}

		ChloeMenuLib::EndMenu();
	}

	DrawMenuOption("Game State:", "", true);

	auto prerace = GetLiteDB()->GetTable("GameFlow.PreRace");
	DrawDebugMenuViewerOption(std::format("Ray Carter State - {}", bRayAltProfileState ? "Alternate" : "Normal"), bRayAltProfileState ? "Ray is locked in" : "Ray is taking it easy");
	DrawDebugMenuViewerOption(std::format("Career Class - {}", prerace->GetPropertyAsInt("Class", 0)));
	DrawDebugMenuViewerOption(std::format("Career Cup - {}", prerace->GetPropertyAsInt("Cup", 0)));

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}