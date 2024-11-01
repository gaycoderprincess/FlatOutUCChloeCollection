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

bool PacenoteComp(tPacenoteSpeech* a, tPacenoteSpeech* b) {
	return a->speechName < b->speechName;
}

void PacenoteTypeEditor(int& out) {
	static bool bHidePlaceholders = false;
	static bool bHideMissing = false;

	if (DrawMenuOption(std::format("Hide Placeholders - {}", bHidePlaceholders), "", false, false)) {
		bHidePlaceholders=!bHidePlaceholders;
	}
	if (DrawMenuOption(std::format("Hide Missing Lines - {}", bHideMissing), "", false, false)) {
		bHideMissing=!bHideMissing;
	}

	DrawMenuOption("Lines:", "", true, false);

	if (DrawMenuOption("None", "", false, false)) {
		out = -1;
		ChloeMenuLib::BackOut();
	}

	std::vector<tPacenoteSpeech*> tmpSpeechesLR;
	std::vector<tPacenoteSpeech*> tmpSpeechesYards;
	std::vector<tPacenoteSpeech*> tmpSpeeches;
	for (auto& speech : aPacenoteSpeeches) {
		if (bHideMissing && speech.IsMissing()) continue;
		if (bHidePlaceholders && speech.IsPlaceholder() && !speech.IsMissing()) continue;

		if (speech.IsNumber()) {
			tmpSpeechesYards.push_back(&speech);
		} else if (speech.speechName.starts_with("Left") || speech.speechName.starts_with("Right")) {
			tmpSpeechesLR.push_back(&speech);
		} else {
			tmpSpeeches.push_back(&speech);
		}
	}
	std::sort(tmpSpeeches.begin(),tmpSpeeches.end(),PacenoteComp);
	std::sort(tmpSpeechesLR.begin(),tmpSpeechesLR.end(),PacenoteComp);

	// sort left/right first for convenience, then everything else, then distance
	for (auto& note : tmpSpeechesLR) {
		if (DrawMenuOption(note->GetName(), "", false, false)) {
			out = note - &aPacenoteSpeeches[0];
			ChloeMenuLib::BackOut();
		}
	}
	for (auto& note : tmpSpeeches) {
		if (DrawMenuOption(note->GetName(), "", false, false)) {
			out = note - &aPacenoteSpeeches[0];
			ChloeMenuLib::BackOut();
		}
	}
	for (auto& note : tmpSpeechesYards) {
		if (DrawMenuOption(note->GetName(), "", false, false)) {
			out = note - &aPacenoteSpeeches[0];
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
				if (DrawMenuOption("Save Pacenotes Globally", "", false, false)) {
					SavePacenotes(GetPacenoteFilename());
				}
				if (DrawMenuOption("Save Pacenotes for this voice", "", false, false)) {
					SavePacenotes(GetPacenoteFilenameWithVoice());
				}
			}
			DrawDebugMenuViewerOption(std::format("Last Played Pacenote - {}", nLastPacenote+1));
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
	//DrawDebugMenuViewerOption(std::format("Buoyancy Factor - {}", fLastBuoyancyResult));
	//DrawDebugMenuViewerOption(std::format("Water Drag Factor - {}", fLastWaterDragResult));
	//DrawDebugMenuViewerOption(std::format("Water Submerged Amount - {}", fLastWaterSubmergedResult));
	DrawDebugMenuViewerOption(std::format("Player Score Pointer - {:X}", (uint32_t)GetPlayerScore<PlayerScoreRace>(1)));
	if (auto ply = GetPlayer(0)) {
		auto track = pTrackAI->pTrack;
		auto start = track->aStartpoints[0].fPosition;
		auto end = track->aSplitpoints[track->nNumSplitpoints-1].fPosition;
		auto plyPos = ply->pCar->GetMatrix()->p;
		DrawDebugMenuViewerOption(std::format("Start Point - {:.2f} {:.2f} {:.2f}", start[0], start[1], start[2]));
		DrawDebugMenuViewerOption(std::format("End Point - {:.2f} {:.2f} {:.2f}", end[0], end[1], end[2]));
		DrawDebugMenuViewerOption(std::format("Player Point - {:.2f} {:.2f} {:.2f}", plyPos[0], plyPos[1], plyPos[2]));
		DrawDebugMenuViewerOption(std::format("Player Progress - {:.0f}%", GetPlayerProgressInStage()*100));
	}

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}