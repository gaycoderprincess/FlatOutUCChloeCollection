void DrawDebugMenuViewerOption(const std::string& name, const std::string& desc = "") {
	if (DrawMenuOption(name, desc, false, false, true)) {
		ChloeMenuLib::SetEnterHint("");
	}
}

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption(std::format("Force Alternate Ray Carter - {}", bForceRayAltProfile), "", false, false)) {
		bForceRayAltProfile = !bForceRayAltProfile;
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