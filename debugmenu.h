void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption(std::format("Force Alternate Ray Carter - {}", bForceRayAltProfile), "", false, false)) {
		bForceRayAltProfile = !bForceRayAltProfile;
	}

	DrawMenuOption("Game State:", "", true);
	DrawMenuOption(std::format("Ray Carter State - {}", bRayAltProfileState ? "Alternate" : "Normal"), bRayAltProfileState ? "Ray is locked in" : "Ray is taking it easy", false, false);
	auto prerace = GetLiteDB()->GetTable("GameFlow.PreRace");
	DrawMenuOption(std::format("Career Class - {}", prerace->GetPropertyAsInt("Class", 0)), "", false, false);
	DrawMenuOption(std::format("Career Cup - {}", prerace->GetPropertyAsInt("Cup", 0)), "", false, false);

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}