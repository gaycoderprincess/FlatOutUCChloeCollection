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

void PacenoteTypeEditor(int& out) {
	if (DrawMenuOption("None", "", false, false)) {
		out = -1;
		ChloeMenuLib::BackOut();
	}
	for (auto& note : aPacenoteSpeeches) {
		if (DrawMenuOption(note.speechName, "", false, false)) {
			out = &note - &aPacenoteSpeeches[0];
			ChloeMenuLib::BackOut();
		}
	}
}

void PacenoteEditor(tPacenote* note) {
	auto ply = GetPlayer(0);
	for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
		if (DrawMenuOption(std::format("Speech {} - {}", i + 1, note->GetSpeechName(i)))) {
			ChloeMenuLib::BeginMenu();
			PacenoteTypeEditor(note->data.types[i]);
			ChloeMenuLib::EndMenu();
		}
	}
	if (DrawMenuOption("Move Pacenote to Car Position", "", false, false)) {
		note->data.pos = ply->pCar->GetMatrix()->p;
	}
	if (DrawMenuOption("Teleport to Pacenote Position", "", false, false)) {
		ply->pCar->GetMatrix()->p = note->data.pos;
		*ply->pCar->GetVelocity() = {0, 0, 0};
		*ply->pCar->GetAngVelocity() = {0, 0, 0};
	}
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

	if (DrawMenuOption("Pacenote Editor")) {
		ChloeMenuLib::BeginMenu();
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			if (DrawMenuOption("Add Pacenotes")) {
				ChloeMenuLib::BeginMenu();
				static tPacenote temp;
				for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
					if (DrawMenuOption(std::format("Speech {} - {}", i + 1, temp.GetSpeechName(i)))) {
						ChloeMenuLib::BeginMenu();
						PacenoteTypeEditor(temp.data.types[i]);
						ChloeMenuLib::EndMenu();
					}
				}
				if (temp.data.types[0] >= 0 && DrawMenuOption("Place Here", "Places a pacenote at your car's position", false, false)) {
					auto ply = GetPlayer(0);
					temp.data.pos = ply->pCar->GetMatrix()->p;
					AddPacenote(temp);

					temp.Reset();
				}
				ChloeMenuLib::EndMenu();
			}
			if (!aPacenotes.empty()) {
				if (DrawMenuOption(std::format("Edit Pacenotes ({})", aPacenotes.size()))) {
					ChloeMenuLib::BeginMenu();
					for (auto &note: aPacenotes) {
						if (DrawMenuOption(
								std::format("{} - {}", (&note - &aPacenotes[0]) + 1, note.GetDisplayName()))) {
							ChloeMenuLib::BeginMenu();
							PacenoteEditor(&note);
							if (DrawMenuOption("Delete Pacenote", "", false, false)) {
								aPacenotes.erase(aPacenotes.begin() + (&note - &aPacenotes[0]));
								ChloeMenuLib::BackOut();
								break;
							}
							ChloeMenuLib::EndMenu();
						}
					}
					ChloeMenuLib::EndMenu();
				}
				if (DrawMenuOption("Save Pacenotes", "", false, false)) {
					SavePacenotes();
				}
			}
			DrawDebugMenuViewerOption(std::format("Next Pacenote - {}", nNextPacenote+1));
		}
		else {
			DrawDebugMenuViewerOption("Not in a race");
		}
		ChloeMenuLib::EndMenu();
	}

	DrawMenuOption("Game State:", "", true);

	auto prerace = GetLiteDB()->GetTable("GameFlow.PreRace");
	DrawDebugMenuViewerOption(std::format("Ray Carter State - {}", bRayAltProfileState ? "Alternate" : "Normal"), bRayAltProfileState ? "Ray is locked in" : "Ray is taking it easy");
	DrawDebugMenuViewerOption(std::format("Career Class - {}", prerace->GetPropertyAsInt("Class", 0)));
	DrawDebugMenuViewerOption(std::format("Career Cup - {}", prerace->GetPropertyAsInt("Cup", 0)));
	DrawDebugMenuViewerOption(std::format("Buoyancy Factor - {}", fLastBuoyancyResult));
	DrawDebugMenuViewerOption(std::format("Water Drag Factor - {}", fLastWaterDragResult));
	DrawDebugMenuViewerOption(std::format("Water Submerged Amount - {}", fLastWaterSubmergedResult));

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}