void DrawDebugMenuViewerOption(const std::string& name, const std::string& desc = "") {
	if (DrawMenuOption(name, desc, false, false, true)) {
		ChloeMenuLib::SetEnterHint("");
	}
}

void ValueEditorMenu(float& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stof(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

void ValueEditorMenu(int& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stoi(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
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
	for (auto& speech : *GetPacenoteDB()) {
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
			out = note - &(*GetPacenoteDB())[0];
			ChloeMenuLib::BackOut();
		}
	}
	for (auto& note : tmpSpeeches) {
		if (DrawMenuOption(note->GetName(), "", false, false)) {
			out = note - &(*GetPacenoteDB())[0];
			ChloeMenuLib::BackOut();
		}
	}
	for (auto& note : tmpSpeechesYards) {
		if (DrawMenuOption(note->GetName(), "", false, false)) {
			out = note - &(*GetPacenoteDB())[0];
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
	if (!bIsInMultiplayer && DrawMenuOption("Teleport to Pacenote Position", "", false, false)) {
		ply->pCar->GetMatrix()->p = note->data.pos;
		*ply->pCar->GetVelocity() = {0, 0, 0};
		*ply->pCar->GetAngVelocity() = {0, 0, 0};
	}
	if (!IsRallyTrack()) {
		if (DrawMenuOption(std::format("Note Split - {} (player split {})", note->atSplit, ply->nCurrentSplit % pEnvironment->nNumSplitpoints))) {
			ValueEditorMenu(note->atSplit);
		}
	}
}

void PlaylistViewer(FO2Vector<MusicInterface::tSong>& songs, int* current, int* next) {
	ChloeMenuLib::BeginMenu();
	DrawDebugMenuViewerOption(std::format("Current Song ID - {}", (*current) + 1));
	DrawDebugMenuViewerOption(std::format("Next Song ID - {}", (*next) + 1));
	for (int i = 0; i < songs.GetSize(); i++) {
		DrawDebugMenuViewerOption(std::format("Song {}", i + 1));

		auto song = &songs[i];
		DrawDebugMenuViewerOption(std::format("File Path - {}", song->sFile.Get()));
		DrawDebugMenuViewerOption(std::format("Artist - {}", song->sArtist.Get()));
		DrawDebugMenuViewerOption(std::format("Title - {}", song->sTitle.Get()));
	}
	ChloeMenuLib::EndMenu();
}

void PaletteEditorMenu(uint8_t& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stoi(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

std::vector<NyaVec3> aCustomSplinesL;
std::vector<NyaVec3> aCustomSplinesR;
std::vector<NyaMat4x4> aCustomStartpoints;

struct tSplitpoint {
	NyaVec3 pos;
	NyaVec3 left;
	NyaVec3 right;
};
std::vector<tSplitpoint> aCustomSplitpoints;

void SplineViewerMenu(std::vector<NyaVec3>& splines, const std::string& name) {
	if (!splines.empty()) {
		if (DrawMenuOption(std::format("Edit {} ({})", name, splines.size()))) {
			ChloeMenuLib::BeginMenu();
			for (auto& node: splines) {
				if (DrawMenuOption(std::to_string((&node - &splines[0]) + 1))) {
					ChloeMenuLib::BeginMenu();
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Node", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (DrawMenuOption("Delete Node", "", false, false)) {
						splines.erase(splines.begin() + (&node - &splines[0]));
						ChloeMenuLib::BackOut();
						break;
					}
					ChloeMenuLib::EndMenu();
				}
			}
			ChloeMenuLib::EndMenu();
		}
	}
}

void StartpointViewerMenu(std::vector<NyaMat4x4>& startpoints, const std::string& name) {
	if (!startpoints.empty()) {
		if (DrawMenuOption(std::format("Edit {} ({})", name, startpoints.size()))) {
			ChloeMenuLib::BeginMenu();
			for (auto& node : startpoints) {
				if (DrawMenuOption(std::to_string((&node - &startpoints[0]) + 1))) {
					ChloeMenuLib::BeginMenu();
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Node", "", false, false)) {
						auto ply = GetPlayer(0);
						*ply->pCar->GetMatrix() = node;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (DrawMenuOption("Delete Node", "", false, false)) {
						startpoints.erase(startpoints.begin() + (&node - &startpoints[0]));
						ChloeMenuLib::BackOut();
						break;
					}
					ChloeMenuLib::EndMenu();
				}
			}
			ChloeMenuLib::EndMenu();
		}
	}
}

void SplitpointViewerMenu(std::vector<tSplitpoint>& splitpoints, const std::string& name) {
	if (!splitpoints.empty()) {
		if (DrawMenuOption(std::format("Edit {} ({})", name, splitpoints.size()))) {
			ChloeMenuLib::BeginMenu();
			for (auto& node : splitpoints) {
				if (DrawMenuOption(std::to_string((&node - &splitpoints[0]) + 1))) {
					ChloeMenuLib::BeginMenu();
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Node", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node.pos;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Left", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node.left;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (!bIsInMultiplayer && DrawMenuOption("Teleport to Right", "", false, false)) {
						auto ply = GetPlayer(0);
						ply->pCar->GetMatrix()->p = node.right;
						*ply->pCar->GetVelocity() = {0, 0, 0};
						*ply->pCar->GetAngVelocity() = {0, 0, 0};
						break;
					}
					if (DrawMenuOption("Delete Node", "", false, false)) {
						splitpoints.erase(splitpoints.begin() + (&node - &splitpoints[0]));
						ChloeMenuLib::BackOut();
						break;
					}
					ChloeMenuLib::EndMenu();
				}
			}
			ChloeMenuLib::EndMenu();
		}
	}
}

void WriteSplitpoints() {
	std::ofstream fout((std::string)GetTrackName(pGameFlow->PreRace.nLevel) + "_splitpoints.bed", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = ";
	fout << aCustomSplitpoints.size();
	fout << "\n\nSplitpoints = {";
	for (auto& point : aCustomSplitpoints) {
		fout << std::format("\n\t[{}] = {{", (&point - &aCustomSplitpoints[0]) + 1);
		fout << std::format("\n\t\tPosition = {{ {}, {}, {} }},", point.pos.x, point.pos.y, point.pos.z);
		fout << std::format("\n\t\tLeft = {{ {}, {}, {} }},", point.left.x, point.left.y, point.left.z);
		fout << std::format("\n\t\tRight = {{ {}, {}, {} }},", point.right.x, point.right.y, point.right.z);
		fout << "\n\n\t},";
	}
	fout << "\n}";
}

void WriteSpline(std::ofstream& fout, std::vector<NyaVec3>& vec, const std::string& name) {
	if (vec.empty()) return;

	fout << "\n\t[\"" + name + "\"] = {";
	fout << "\n\t\tCount = " + std::to_string(vec.size()) + ",";
	fout << "\n\t\tControlPoints = {";
	for (auto& pos : vec) {
		fout << std::format("\n\t\t\t[{}] = {{ {}, {}, {} }},", std::to_string((&pos - &vec[0]) + 1), pos.x, pos.y, pos.z);
	}
	fout << "\n\t\t},";
	fout << "\n\t},\n";
}

void WriteSplines() {
	std::ofstream fout((std::string)GetTrackName(pGameFlow->PreRace.nLevel) + "_splines.ai", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = 2";
	fout << "\n\nSplines = {";

	WriteSpline(fout, aCustomSplinesL, "AIBorderLineLeft");
	WriteSpline(fout, aCustomSplinesR, "AIBorderLineRight");

	fout << "\n}";
}

void WriteStartpoints() {
	std::ofstream fout((std::string)GetTrackName(pGameFlow->PreRace.nLevel) + "_startpoints.bed", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = ";
	fout << aCustomStartpoints.size();
	fout << "\n\nStartpoints = {";
	for (auto& point : aCustomStartpoints) {
		for (int i = 0; i < 16; i++) {
			if (std::abs(point[i]) < 0.001) point[i] = 0;
		}

		fout << std::format("\n\t[{}] = {{", (&point - &aCustomStartpoints[0]) + 1);
		fout << std::format("\n\t\tPosition = {{ {}, {}, {} }},", point[12], point[13], point[14]);
		fout << "\n\t\tOrientation = {";
		fout << std::format("\n\t\t\t[\"x\"]={{{},{},{}}},", point[0], point[1], point[2]);
		fout << std::format("\n\t\t\t[\"y\"]={{{},{},{}}},", point[4], point[5], point[6]);
		fout << std::format("\n\t\t\t[\"z\"]={{{},{},{}}},", point[8], point[9], point[10]);
		fout << "\n\t\t},";
		fout << "\n\n\t},";
	}
	fout << "\n}";
}

void WriteTrackListDebug() {
	std::ofstream fout("tracklist.txt", std::ios::out);
	if (!fout.is_open()) return;

	const char* trackTypes[] = {
		"TRACKTYPE_FOREST",
		"TRACKTYPE_FIELDS",
		"TRACKTYPE_DESERT",
		"TRACKTYPE_CANAL",
		"TRACKTYPE_CITY",
		"TRACKTYPE_RACING",
		"TRACKTYPE_FO1_TOWN",
		"TRACKTYPE_FO1_PIT",
		"TRACKTYPE_FO1_WINTER",
		"TRACKTYPE_TOUGHTRUCKS",
		"TRACKTYPE_RALLYTROPHY",
		"TRACKTYPE_RALLYTROPHYARCADE",
		"TRACKTYPE_EVENT",
		"TRACKTYPE_STUNT",
		"TRACKTYPE_DERBY",
		"TRACKTYPE_REVERSED",
		"TRACKTYPE_ARCADERACE",
		"TRACKTYPE_BEATTHEBOMB",
		"TRACKTYPE_TONYHAWK",
		"TRACKTYPE_SPEEDTRAP",
		"TRACKTYPE_GARAGETEST",
	};

	bool firstLine = true;
	for (auto& type : trackTypes) {
		bool first = true;
		for (int i = 1; i < GetNumTracks() + 1; i++) {
			if (!DoesTrackExist(i)) continue;
			if (GetTrackValueNumber(i, "TrackType") != (&type - trackTypes) + 1) continue;

			if (first) {
				if (!firstLine) {
					fout << "\n";
				}
				firstLine = false;
				fout << type;
				fout << "\n";
				first = false;
			}

			fout << GetTrackName(i);
			auto desc = GetTrackDescription(i);
			if (!desc.empty()) {
				fout << " - ";
				fout << desc;
			}
			fout << "\n";
		}
	}
}

void ProcessDebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption(std::format("Force Alternate Ray Carter - {}", bForceRayAltProfile), "", false, false)) {
		bForceRayAltProfile = !bForceRayAltProfile;
	}

	if (DrawMenuOption("Playtime Stats")) {
		ChloeMenuLib::BeginMenu();

		for (int i = 0; i < NUM_PLAYTIME_TYPES_NEW; i++) {
			DrawDebugMenuViewerOption(std::format("{} - {}", aPlaytimeTypeNames[i], GetTimeString(gCustomSave.playtimeNew[i])));
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Achievements")) {
		ChloeMenuLib::BeginMenu();

		if (Achievements::GetNumUnlockedAchievements() > 0) {
			DrawDebugMenuViewerOption("-- Unlocked --");
			for (auto& achievement : Achievements::gAchievements) {
				if (!achievement->bUnlocked) continue;
				DrawDebugMenuViewerOption(achievement->sName);
				DrawDebugMenuViewerOption(achievement->sDescription);
				DrawDebugMenuViewerOption("");
			}
		}

		if (Achievements::GetNumUnlockedAchievements() < Achievements::GetNumVisibleAchievements()) {
			DrawDebugMenuViewerOption("-- Locked --");
			for (auto& achievement : Achievements::gAchievements) {
				if (achievement->bUnlocked) continue;
				if (achievement->bHidden) continue;
				DrawDebugMenuViewerOption(std::format("{} ({}%)", achievement->sName, achievement->nProgress));
				DrawDebugMenuViewerOption(achievement->sDescription);
				DrawDebugMenuViewerOption("");
			}
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Palette Editor")) {
		ChloeMenuLib::BeginMenu();
		for (int i = 0; i < 256; i++) {
			auto& col = *(NyaDrawing::CNyaRGBA32*)&gPalette[i];
			if (DrawMenuOption(std::format("Color {} - {} {} {}", i, col.b, col.g, col.r))) {
				ChloeMenuLib::BeginMenu();
				if (DrawMenuOption(std::format("Red - {}", col.b))) {
					PaletteEditorMenu(col.b);
				}
				if (DrawMenuOption(std::format("Green - {}", col.g))) {
					PaletteEditorMenu(col.g);
				}
				if (DrawMenuOption(std::format("Blue - {}", col.r))) {
					PaletteEditorMenu(col.r);
				}
				ChloeMenuLib::EndMenu();
			}
		}
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Drift Mode Tweaks")) {
		ChloeMenuLib::BeginMenu();

#ifdef DRIFT_DEBUG
		if (DrawMenuOption(std::format("Drift Score Speed Factor - {}", DriftMode::fDriftScoreSpeedFactor))) {
			ValueEditorMenu(DriftMode::fDriftScoreSpeedFactor);
		}

		if (DrawMenuOption(std::format("Handling Factor - {}", DriftMode::fDriftHandlingFactor))) {
			ValueEditorMenu(DriftMode::fDriftHandlingFactor);
		}

		if (DrawMenuOption(std::format("Handling Factor Fwd - {}", DriftMode::fDriftHandlingFactorFwd))) {
			ValueEditorMenu(DriftMode::fDriftHandlingFactorFwd);
		}

		if (DrawMenuOption(std::format("Handling Top Speed - {}", DriftMode::fDriftHandlingTopSpeed))) {
			ValueEditorMenu(DriftMode::fDriftHandlingTopSpeed);
		}

		if (DrawMenuOption(std::format("Handling Top Turn Speed - {}", DriftMode::fDriftTurnTopSpeed))) {
			ValueEditorMenu(DriftMode::fDriftTurnTopSpeed);
		}

		if (DrawMenuOption(std::format("Handling Turn Factor - {}", DriftMode::fDriftTurnSpeed))) {
			ValueEditorMenu(DriftMode::fDriftTurnSpeed);
		}

		if (DrawMenuOption(std::format("Angular Velocity Cap - {}", DriftMode::fDriftTurnAngSpeedCap))) {
			ValueEditorMenu(DriftMode::fDriftTurnAngSpeedCap);
		}

		//if (DrawMenuOption(std::format("Total Velocity Cap - {}", DriftMode::fDriftVelocityCap))) {
		//	ValueEditorMenu(DriftMode::fDriftVelocityCap);
		//}

		if (DrawMenuOption(std::format("Speed Dropoff Factor - {}", DriftMode::fDriftHandlingSpeedDropoffFactor))) {
			ValueEditorMenu(DriftMode::fDriftHandlingSpeedDropoffFactor);
		}
#endif

		if (DrawMenuOption(std::format("Drift Lookat Offset - {}", DriftCamera::fLookatOffset))) {
			ValueEditorMenu(DriftCamera::fLookatOffset);
		}

		if (DrawMenuOption(std::format("Drift Follow Offset - {}", DriftCamera::fFollowOffset))) {
			ValueEditorMenu(DriftCamera::fFollowOffset);
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Rally Career")) {
		ChloeMenuLib::BeginMenu();

		if (DrawMenuOption(std::format("Current Class - {}", gCustomSave.nRallyClass))) {
			int i = gCustomSave.nRallyClass;
			ValueEditorMenu(i);
			gCustomSave.nRallyClass = i;
		}

		if (DrawMenuOption(std::format("Current Cup - {}", gCustomSave.nRallyCup))) {
			int i = gCustomSave.nRallyCup;
			ValueEditorMenu(i);
			gCustomSave.nRallyCup = i;
		}

		if (DrawMenuOption(std::format("Next Stage - {}", gCustomSave.nRallyCupNextStage))) {
			int i = gCustomSave.nRallyCupNextStage;
			ValueEditorMenu(i);
			gCustomSave.nRallyCupNextStage = i;
		}

		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Car Helpers")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption("Driver Location")) {
			ChloeMenuLib::BeginMenu();

			if (auto ply = GetPlayer(0)) {
				if (DrawMenuOption(std::format("X - {}", ply->pCar->vDriverLoc[0]))) {
					ValueEditorMenu(ply->pCar->vDriverLoc[0]);
				}
				if (DrawMenuOption(std::format("Y - {}", ply->pCar->vDriverLoc[1]))) {
					ValueEditorMenu(ply->pCar->vDriverLoc[1]);
				}
				if (DrawMenuOption(std::format("Z - {}", ply->pCar->vDriverLoc[2]))) {
					ValueEditorMenu(ply->pCar->vDriverLoc[2]);
				}
				for (int i = 0; i < 3; i++) {
					ply->pCar->vDriverLocAbsolute[i] = ply->pCar->vDriverLoc[i] - ply->pCar->vCenterOfMassAbsolute[i];
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}

			ChloeMenuLib::EndMenu();
		}
		ChloeMenuLib::EndMenu();
	}

	if (DrawMenuOption("Track Helpers")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption(std::format("Load FO2 Track - {}", bLoadFO2Track), "", false, false)) {
			bLoadFO2Track = !bLoadFO2Track;
		}
		if (DrawMenuOption("Spline Creator")) {
			ChloeMenuLib::BeginMenu();
			if (pGameFlow->nGameState == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Left Node")) {
					auto ply = GetPlayer(0);
					aCustomSplinesL.push_back(ply->pCar->GetMatrix()->p);
				}
				if (DrawMenuOption("Add Right Node")) {
					auto ply = GetPlayer(0);
					aCustomSplinesR.push_back(ply->pCar->GetMatrix()->p);
				}
				SplineViewerMenu(aCustomSplinesL, "Left");
				SplineViewerMenu(aCustomSplinesR, "Right");
				if (!aCustomSplinesL.empty() && !aCustomSplinesR.empty()) {
					if (DrawMenuOption("Save Splines", "", false, false)) {
						WriteSplines();
					}
					if (DrawMenuOption("Delete All Splines", "", false, false)) {
						aCustomSplinesL.clear();
						aCustomSplinesR.clear();
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Startpoint Creator")) {
			ChloeMenuLib::BeginMenu();
			if (pGameFlow->nGameState == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Startpoint")) {
					auto ply = GetPlayer(0);
					aCustomStartpoints.push_back(*ply->pCar->GetMatrix());
				}
				StartpointViewerMenu(aCustomStartpoints, "Startpoints");
				if (!aCustomStartpoints.empty()) {
					if (DrawMenuOption("Save Startpoints", "", false, false)) {
						WriteStartpoints();
					}
					if (DrawMenuOption("Delete All Startpoints", "", false, false)) {
						aCustomStartpoints.clear();
					}
					if (DrawMenuOption("Apply Startpoints to Game", "", false, false)) {
						pEnvironment->nNumStartpoints = aCustomStartpoints.size();
						if (pEnvironment->nNumStartpoints > 32) pEnvironment->nNumStartpoints = 32;
						for (int i = 0; i < pEnvironment->nNumStartpoints; i++) {
							auto& start = aCustomStartpoints[i];
							memcpy(pEnvironment->aStartpoints[i].fMatrix, &start, sizeof(start));
							pEnvironment->aStartpoints[i].fPosition[0] = start.p[0];
							pEnvironment->aStartpoints[i].fPosition[1] = start.p[1];
							pEnvironment->aStartpoints[i].fPosition[2] = start.p[2];
						}
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Splitpoint Creator")) {
			ChloeMenuLib::BeginMenu();
			if (pGameFlow->nGameState == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Splitpoint")) {
					auto ply = GetPlayer(0);
					tSplitpoint point;
					point.pos = point.left = point.right = ply->pCar->GetMatrix()->p;
					point.left -= ply->pCar->GetMatrix()->x * 50;
					point.right += ply->pCar->GetMatrix()->x * 50;
					aCustomSplitpoints.push_back(point);
				}
				SplitpointViewerMenu(aCustomSplitpoints, "Splitpoints");
				if (!aCustomSplitpoints.empty()) {
					if (DrawMenuOption("Save Splitpoints", "", false, false)) {
						WriteSplitpoints();
					}
					if (DrawMenuOption("Delete All Splitpoints", "", false, false)) {
						aCustomSplitpoints.clear();
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Resetpoint Editor")) {
			ChloeMenuLib::BeginMenu();
			if (pGameFlow->nGameState == GAME_STATE_RACE) {
				if (DrawMenuOption("Add Reset")) {
					auto ply = GetPlayer(0);
					tResetpoint point;
					point.matrix = *ply->pCar->GetMatrix();
					point.split = ply->nCurrentSplit % pEnvironment->nNumSplitpoints;
					aNewResetPoints.push_back(point);
				}
				if (!aNewResetPoints.empty()) {
					if (DrawMenuOption(std::format("Edit Resetpoints ({})", aNewResetPoints.size()))) {
						ChloeMenuLib::BeginMenu();
						for (auto& reset: aNewResetPoints) {
							if (DrawMenuOption(std::to_string((&reset - &aNewResetPoints[0]) + 1))) {
								ChloeMenuLib::BeginMenu();
								if (DrawMenuOption("Teleport to Node", "", false, false)) {
									auto ply = GetPlayer(0);
									ResetCarAt(ply->pCar, reset.matrix, *(float*)0x849430);
									break;
								}
								if (DrawMenuOption("Delete Resetpoint", "", false, false)) {
									aNewResetPoints.erase(aNewResetPoints.begin() + (&reset - &aNewResetPoints[0]));
									ChloeMenuLib::BackOut();
									break;
								}
								ChloeMenuLib::EndMenu();
							}
						}
						ChloeMenuLib::EndMenu();
					}
					if (DrawMenuOption("Save Resetpoints", "", false, false)) {
						bool hasSplits = false;
						for (auto& reset : aNewResetPoints) {
							if (reset.split >= 0) {
								hasSplits = true;
								break;
							}
						}
						SaveResetPoints(GetResetPointFilename(hasSplits), hasSplits);
					}
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
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
								if (DrawMenuOption("Delete Pacenote")) {
									ChloeMenuLib::BeginMenu();
									if (DrawMenuOption("Confirm Deletion", "", false, false)) {
										aPacenotes.erase(aPacenotes.begin() + (&note - &aPacenotes[0]));
										ChloeMenuLib::BackOut();
										ChloeMenuLib::BackOut();
										return;
									}
									if (DrawMenuOption("Cancel", "", false, false)) {
										ChloeMenuLib::BackOut();
										return;
									}
									ChloeMenuLib::EndMenu();
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
		if (DrawMenuOption("Map Position Editor")) {
			ChloeMenuLib::BeginMenu();
			if (pGameFlow->nGameState == GAME_STATE_RACE) {
				auto map = pEnvironment->pMinimap;
				if (DrawMenuOption(std::format("Top Left X - {}", map->fMapTopLeft[0]))) {
					ValueEditorMenu(map->fMapTopLeft[0]);
				}
				if (DrawMenuOption(std::format("Top Left Y - {}", map->fMapTopLeft[1]))) {
					ValueEditorMenu(map->fMapTopLeft[1]);
				}
				if (DrawMenuOption(std::format("Top Left Z - {}", map->fMapTopLeft[2]))) {
					ValueEditorMenu(map->fMapTopLeft[2]);
				}
				if (DrawMenuOption(std::format("Bottom Right X - {}", map->fMapBottomRight[0]))) {
					ValueEditorMenu(map->fMapBottomRight[0]);
				}
				if (DrawMenuOption(std::format("Bottom Right Y - {}", map->fMapBottomRight[1]))) {
					ValueEditorMenu(map->fMapBottomRight[1]);
				}
				if (DrawMenuOption(std::format("Bottom Right Z - {}", map->fMapBottomRight[2]))) {
					ValueEditorMenu(map->fMapBottomRight[2]);
				}
			}
			else {
				DrawDebugMenuViewerOption("Not in a race");
			}
			ChloeMenuLib::EndMenu();
		}
		if (DrawMenuOption("Create Track List", "", false, false)) {
			WriteTrackListDebug();
		}
		ChloeMenuLib::EndMenu();
	}

	DrawMenuOption("Game State:", "", true);

	auto prerace = GetLiteDB()->GetTable("GameFlow.PreRace");
	if (DrawMenuOption("Game Playlist State")) {
		ChloeMenuLib::BeginMenu();
		if (DrawMenuOption("Title")) {
			PlaylistViewer(MusicInterface::gPlaylistTitle, &MusicInterface::gPlaylistTitleCurrent, &MusicInterface::gPlaylistTitleNext);
		}
		if (DrawMenuOption("Ingame")) {
			PlaylistViewer(MusicInterface::gPlaylistIngame, &MusicInterface::gPlaylistIngameCurrent, &MusicInterface::gPlaylistIngameNext);
		}
		if (DrawMenuOption("Stunt")) {
			PlaylistViewer(MusicInterface::gPlaylistStunt, &MusicInterface::gPlaylistStuntCurrent, &MusicInterface::gPlaylistStuntNext);
		}
		ChloeMenuLib::EndMenu();
	}
	if (DrawMenuOption("Mod Playlist State")) {
		ChloeMenuLib::BeginMenu();
		for (auto& playlist : aPlaylists) {
			if (!playlist.gamePlaylist.begin) continue;

			if (DrawMenuOption(GetStringNarrow(playlist.name))) {
				PlaylistViewer(playlist.gamePlaylist, &playlist.gamePlaylistCurrent, &playlist.gamePlaylistNext);
			}
		}
		if (gCarnageModernPlaylist.gamePlaylist.begin) {
			if (DrawMenuOption("MODERN (CARNAGE)")) {
				auto& playlist = gCarnageModernPlaylist;
				PlaylistViewer(playlist.gamePlaylist, &playlist.gamePlaylistCurrent, &playlist.gamePlaylistNext);
			}
		}
		for (auto& playlist : aMenuPlaylists) {
			if (!playlist.gamePlaylist.begin) continue;

			if (DrawMenuOption(GetStringNarrow(playlist.name) + " (MENU)")) {
				PlaylistViewer(playlist.gamePlaylist, &playlist.gamePlaylistCurrent, &playlist.gamePlaylistNext);
			}
		}
		for (auto& playlist : aStuntPlaylists) {
			if (!playlist.gamePlaylist.begin) continue;

			if (DrawMenuOption(GetStringNarrow(playlist.name) + " (STUNT)")) {
				PlaylistViewer(playlist.gamePlaylist, &playlist.gamePlaylistCurrent, &playlist.gamePlaylistNext);
			}
		}
		ChloeMenuLib::EndMenu();
	}
	if (pCupManager && DrawMenuOption("Cup State")) {
		ChloeMenuLib::BeginMenu();
		DrawDebugMenuViewerOption(std::format("In Cup - {}", pCupManager->nResultsValid));
		for (int i = 0; i < 32; i++) {
			auto ply = pCupManager->aStandings[i];
			DrawMenuOption(std::format("{}: Player {}, {} pts", i+1, ply.nPlayerId, ply.nCupPoints));
		}
		ChloeMenuLib::EndMenu();
	}
	if (DrawMenuOption("Arcade Mode Verification Data")) {
		ChloeMenuLib::BeginMenu();
		/*if (DrawMenuOption("Save")) {
			auto file = std::ofstream("arcadedata.txt", std::ios::out);
			if (file.is_open()) {
				for (int x = 0; x < nNumArcadeRacesX; x++) {
					for (int y = 0; y < nNumArcadeRacesY; y++) {
						auto verify = gCustomSave.aArcadeRaceVerify[x][y];
						file << std::format("verify[{}][{}] = {{ (uint16_t)GetCarByName(\"{}\"), {} }};\n", x, y, GetCarName(verify.car), verify.level);
					}
				}
			}
		}*/
		for (int x = 0; x < nNumArcadeRacesX; x++) {
			for (int y = 0; y < nNumArcadeRacesY; y++) {
				auto data = gCustomSave.aArcadeRaceVerify[x][y];
				DrawMenuOption(std::format("[{}][{}]: {}, {}", x, y, GetCarName(data.car), GetTrackName(data.level)));
			}
		}
		ChloeMenuLib::EndMenu();
	}
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
		DrawDebugMenuViewerOption(std::format("Player Progress - {:.0f}%", GetLocalPlayerProgressInStage()*100));
		DrawDebugMenuViewerOption(std::format("Player Pointer - {:X}", (uintptr_t)ply));
		DrawDebugMenuViewerOption(std::format("Player Car Pointer - {:X}", (uintptr_t)ply->pCar));

		if (bIsDriftEvent) {
			auto car = ply->pCar;
			auto fwd = car->GetMatrix()->z;
			auto vel = *car->GetVelocity();
			auto velNorm = vel;
			velNorm.Normalize();
			auto dot = fwd.Dot(velNorm);
			DrawDebugMenuViewerOption(std::format("Player Drift Value - {:.2f}", dot));
			auto cross = fwd.Cross(vel);
			DrawDebugMenuViewerOption(
					std::format("Player Drift Cross - {:.2f} {:.2f} {:.2f}", cross.x, cross.y, cross.z));

			double dropoff = 1;
			if (vel.length() >= DriftMode::fDriftHandlingTopSpeed / 3.6) {
				dropoff = 1 - ((vel.length() - (DriftMode::fDriftHandlingTopSpeed / 3.6)) *
							   DriftMode::fDriftHandlingSpeedDropoffFactor);
				if (dropoff < 0) dropoff = 0;
			}
			DrawDebugMenuViewerOption(std::format("Player Drift Power - {:.2f}", dropoff));
		}
	}

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}