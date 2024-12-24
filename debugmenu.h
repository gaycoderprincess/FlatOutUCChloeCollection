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
	if (!bIsInMultiplayer && DrawMenuOption("Teleport to Pacenote Position", "", false, false)) {
		ply->pCar->GetMatrix()->p = note->data.pos;
		*ply->pCar->GetVelocity() = {0, 0, 0};
		*ply->pCar->GetAngVelocity() = {0, 0, 0};
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
	std::ofstream fout((std::string)GetTrackName(pGameFlow->nLevelId) + "_splines.ai", std::ios::out);
	if (!fout.is_open()) return;

	fout << "Count = 2";
	fout << "\n\nSplines = {";

	WriteSpline(fout, aCustomSplinesL, "AIBorderLineLeft");
	WriteSpline(fout, aCustomSplinesR, "AIBorderLineRight");

	fout << "\n}";
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
				if (DrawMenuOption("Save Splines Globally", "", false, false)) {
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

	if (DrawMenuOption("Resetpoint Editor")) {
		ChloeMenuLib::BeginMenu();
		if (pGameFlow->nGameState == GAME_STATE_RACE) {
			if (DrawMenuOption("Add Reset")) {
				auto ply = GetPlayer(0);
				aNewResetPoints.push_back(*ply->pCar->GetMatrix());
			}
			if (!aNewResetPoints.empty()) {
				if (DrawMenuOption(std::format("Edit Resetpoints ({})", aNewResetPoints.size()))) {
					ChloeMenuLib::BeginMenu();
					for (auto& reset: aNewResetPoints) {
						if (DrawMenuOption(std::to_string((&reset - &aNewResetPoints[0]) + 1))) {
							ChloeMenuLib::BeginMenu();
							if (DrawMenuOption("Teleport to Node", "", false, false)) {
								auto ply = GetPlayer(0);
								ResetCarAt(ply->pCar, reset, *(float*)0x849430);
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
					SaveResetPoints(GetResetPointFilename());
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
	}

	ChloeMenuLib::EndMenu();
}

void ApplyDebugMenuPatches() {
	ChloeMenuLib::RegisterMenu("Chloe Collection Debug Menu", &ProcessDebugMenu);
}