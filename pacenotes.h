const float fPacenoteRange = 20.0;
const int nMaxSpeechesPerPacenote = 8;

struct tPacenoteType {
	std::wstring name;
	std::string folder;
};
std::vector<tPacenoteType> aPacenoteSpeechTypes;
std::vector<tPacenoteType> aPacenoteVisualTypes;

bool AreVisualPacenotesEnabled() {
	if (nPacenoteVisualType < aPacenoteVisualTypes.size()) return !aPacenoteVisualTypes[nPacenoteVisualType].folder.empty();
	return false;
}

struct tPacenoteSpeech {
	std::string speechName;
	std::string speechFile;
	std::string speechFileFallback;
	std::string speechFileFallback2;
	IDirect3DTexture9* pTexture = nullptr;
	int textureType = -1;

	bool IsNumber() {
		if (speechName[0] >= '1' && speechName[0] <= '9') return true;
		return false;
	}

	static std::string GetSpeechPath(const std::string& file) {
		std::string folder = "rt_eng";
		if (nPacenoteType < aPacenoteSpeechTypes.size()) folder = aPacenoteSpeechTypes[nPacenoteType].folder;
		return "data/sound/rally/" + folder + "/" + file + ".wav";
	}

	bool IsPlaceholder() const {
		return !std::filesystem::exists(GetSpeechPath(speechFile));
	}

	bool IsMissing() const {
		if (std::filesystem::exists(GetSpeechPath(speechFile))) return false;
		if (!speechFileFallback.empty() && std::filesystem::exists(GetSpeechPath(speechFileFallback))) return false;
		if (!speechFileFallback.empty() && std::filesystem::exists(GetSpeechPath(speechFileFallback2))) return false;
		return true;
	}

	std::string GetName() const {
		if (IsMissing()) return speechName + " (Missing)";
		if (IsPlaceholder()) return speechName + " (Placeholder)";
		return speechName;
	}

	static NyaAudio::NyaSound PlaySpeech(const std::string& file) {
		if (file.empty()) return 0;

		auto sound = NyaAudio::LoadFile(GetSpeechPath(file).c_str());
		if (!sound) return 0;
		NyaAudio::SetVolume(sound, nPacenoteVolume / 100.0);
		NyaAudio::Play(sound);
		return sound;
	}

	NyaAudio::NyaSound Play(bool useFallback) {
		auto sound = PlaySpeech(speechFile);
		if (!sound && useFallback) sound = PlaySpeech(speechFileFallback);
		if (!sound && useFallback) sound = PlaySpeech(speechFileFallback2);
		return sound;
	}

	void LoadTexture() {
		if (!AreVisualPacenotesEnabled()) {
			textureType = nPacenoteVisualType;
			return;
		}

		if (textureType == nPacenoteVisualType) return;
		if (pTexture) {
			pTexture->Release();
			pTexture = nullptr;
		}

		std::string pacenotePath = "data/textures/" + aPacenoteVisualTypes[nPacenoteVisualType].folder + "/";
		pTexture = ::LoadTexture((pacenotePath + speechFile + ".png").c_str());
		if (!pTexture) pTexture = ::LoadTexture((pacenotePath + speechFileFallback + ".png").c_str());
		if (!pTexture) pTexture = ::LoadTexture((pacenotePath + speechFileFallback2 + ".png").c_str());
		textureType = nPacenoteVisualType;
	}
};

tPacenoteSpeech aPacenoteSpeeches[] = {
		{"Left 1", "Left1"},
		{"Left 2", "Left2"},
		{"Left 3", "Left3"},
		{"Left 4", "Left4"},
		{"Left 5", "Left5"},
		{"Left Hairpin", "LeftHairPin"},
		{"Left Turn", "LeftTurn"},
		{"Right 1", "Right1"},
		{"Right 2", "Right2"},
		{"Right 3", "Right3"},
		{"Right 4", "Right4"},
		{"Right 5", "Right5"},
		{"Right Hairpin", "RightHairPin"},
		{"Right Turn", "RightTurn"},
		{"Into", "Into"},
		{"Crest", "Crest", "JumpMaybe"},
		{"Over Crest", "OverCrest", "JumpMaybe"},
		{"Jump", "Jump"},
		{"Jump Maybe", "JumpMaybe"},
		{"Opens", "Opens"},
		{"Straight", "Straight"},
		{"Tightens", "Tightens"},
		{"Water Splash", "WaterSplash"},
		{"Careful", "Careful"},
		{"Careful - Bridge", "CarefulBridge", "Careful"},
		{"Careful - Tunnel", "CarefulTunnel", "Careful"},
		{"Careful - Water", "CarefulWater", "Careful"},
		{"Caution", "Caution"},
		{"Caution - Ditch", "CautionDitch", "Caution"},
		{"Caution - Logs", "Cautionlogs", "Caution"},
		{"Caution - Post", "CautionPost", "Caution"},
		{"Caution - Rocks", "CautionRocks", "Caution"},
		{"Caution - Water", "CautionWater", "Caution"},
		{"Don't Cut", "Dontcut"},
		{"Don't Cut - Ditch", "Dontcutditch", "Dontcut"},
		{"Don't Cut - Logs", "Dontcutlogs", "Dontcut"},
		{"Don't Cut - Post", "Dontcutpost", "Dontcut"},
		{"Don't Cut - Rocks", "Dontcutrocks", "Dontcut"},
		{"Don't Cut - Water", "DontcutWater", "Dontcut"},
		{"Finish", "Finish"},
		{"Big Jump", "BigJump", "Jump"},
		{"Jumps", "Jumps", "Jump"},
		{"Double Caution", "DoubleCaution", "Caution"},
		{"Long", "Long"},
		{"Careful - Dip", "CarefulDip", "Careful"},
		{"Caution - Slowing", "CautionSlowing", "Caution"},
		{"Caution - Bump", "CautionBump", "Caution"},
		{"Caution - Bumps", "CautionBumps", "CautionBump", "Caution"},
		{"Caution - Tree", "CautionTree", "Caution"},
		{"Caution - Trees", "CautionTrees", "CautionTree", "Caution"},
		{"Caution - Dip", "CautionDip", "Caution"},
		{"Don't Cut - Bump", "DontcutBump", "Dontcut"},
		{"Don't Cut - Bumps", "DontcutBumps", "DontcutBump", "Dontcut"},
		{"Don't Cut - Tree", "DontcutTree", "Dontcut"},
		{"Don't Cut - Trees", "DontcutTrees", "DontcutTree", "Dontcut"},
		{"Bump", "Bump", "Crest", "JumpMaybe"},
		{"Bumps", "Bumps", "Bump", "Jumps"},
		{"Big Crest", "BigCrest", "Crest"},
		{"Keep Left", "KeepLeft"},
		{"Keep Right", "KeepRight"},
		{"Keep Mid", "KeepMid"},
		{"Water", "Water", "WaterSplash"},
		{"Through Water", "ThroughWater", "Water", "WaterSplash"},
		{"Left 6", "Left6", "Left5"},
		{"Right 6", "Right6", "Right5"},
		{"And", "And", "Into"},
		{"Dip", "Dip"},
		{"Downhill", "Downhill"},
		{"Slowing", "Slowing"},
		{"Open Hairpin Left", "OpenHairPinLeft", "LeftHairPin"},
		{"Open Hairpin Right", "OpenHairPinRight", "RightHairPin"},
		{"Flying Finish", "FlyingFinish", "Finish"},
		{"Over Finish", "OverFinish", "Finish"},
		{"Bad Camber", "BadCamber"},
		{"Narrows", "Tightens"},
		{"Onto Gravel", "OntoGravel"},
		{"Onto Tar", "OntoTar"},
		{"Onto Dirt", "OntoDirt"},
		{"Cut", "Cut"},
		{"40", "40"},
		{"50", "50"},
		{"60", "60"},
		{"70", "70"},
		{"80", "80"},
		{"90", "90"},
		{"100","100"},
		{"110","110"},
		{"120","120"},
		{"130","130"},
		{"140","140"},
		{"150","150"},
		{"160","160"},
		{"170","170"},
		{"180","180"},
		{"190","190"},
		{"200","200"},
};

CNyaTimer gPacenoteTimer;

struct tPacenoteVisual {
	IDirect3DTexture9* pTexture;
	double alpha;
	double appearTime;
};
std::vector<tPacenoteVisual> aVisualPacenotes;

void DrawVisualPacenotes() {
	if (pLoadingScreen) return;
	if (pGameFlow->nGameState != GAME_STATE_RACE) return;
	if (pGameFlow->nRaceState != RACE_STATE_RACING) return;

	float sizeX = 0.05 * GetAspectRatioInv();
	float spacingX = sizeX * 2  * 1.1;
	float sizeY = 0.05;
	float posX = 0.5;
	float posY = 0.2;
	for (auto& note : aVisualPacenotes) {
		double realAlpha = note.alpha;
		if (note.appearTime < 0.25) realAlpha *= (note.appearTime * 4);
		DrawRectangle(posX - sizeX, posX + sizeX, posY - sizeY, posY + sizeY, {255,255,255,(uint8_t)(realAlpha * 255)}, 0, note.pTexture);
		double moveAmount = note.alpha * 2;
		if (moveAmount > 1) moveAmount = 1;
		posX += moveAmount * spacingX;
	}
}

std::string sLastPlayedPacenoteSpeech;

struct tPacenotePlaying {
	tPacenoteSpeech* speech = nullptr;
	std::string speechFile;
	NyaAudio::NyaSound audio = 0;
	double visualTimer = 0;
	double visualAppearTimer = 0;
	bool audioPlaying = false;
	bool audioFinished = false;
	bool skipAudio = false;

	static bool CanPlayAudio() {
		if (pLoadingScreen || pGameFlow->nRaceState != RACE_STATE_RACING) return false;
		auto ply = GetPlayer(0);
		if (!ply) return false;
		if (ply->pCar->nIsRagdolled) return false;
		return true;
	}

	void Process(bool isNextInQueue, bool canLinger) {
		if (audioPlaying) {
			visualTimer += gPacenoteTimer.fDeltaTime;
		}
		if (audioFinished && !canLinger) {
			if (visualTimer < 2.5) visualTimer = 2.5;
		}
		visualAppearTimer += gPacenoteTimer.fDeltaTime;

		if (speech->pTexture) {
			double alpha = 1;
			if (visualTimer > 2.5) alpha = std::lerp(1, 0, (visualTimer - 2.5) * 2);
			if (alpha <= 0) alpha = 0;
			if (alpha >= 1) alpha = 1;
			aVisualPacenotes.push_back({speech->pTexture, alpha, visualAppearTimer});
		}

		if (CanPlayAudio() && isNextInQueue && !audioPlaying) {
			audioPlaying = true;

			if (skipAudio) {
				audioFinished = true;
			}
			else if (speechFile.empty()) {
				// hack for repeats of fallback speeches
				bool useFallback = true;
				if (sLastPlayedPacenoteSpeech == speech->speechFileFallback || (sLastPlayedPacenoteSpeech == "Jump" && speech->speechFileFallback == "JumpMaybe")) {
					useFallback = false;
				}
				sLastPlayedPacenoteSpeech = speech->speechFile;

				audio = speech->Play(useFallback);
				if (!audio) {
					audioFinished = true;
				}
			}
			else {
				audio = tPacenoteSpeech::PlaySpeech(speechFile);
				if (!audio) {
					audioFinished = true;
				}
			}
		}
		if (audio && NyaAudio::IsFinishedPlaying(audio)) {
			NyaAudio::Delete(&audio);
			audioFinished = true;
		}
	}
};
std::vector<tPacenotePlaying> aPacenoteQueue;

void ClearPacenoteQueue() {
	for (auto& data : aPacenoteQueue) {
		if (data.audio) {
			NyaAudio::Stop(data.audio);
			NyaAudio::Delete(&data.audio);
		}
	}
	aPacenoteQueue.clear();
}

struct tPacenote {
	struct tData {
		NyaVec3 pos;
		int types[nMaxSpeechesPerPacenote];
	} data;
	bool played = false;

	void Reset() {
		data.pos = {0,0,0};
		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			data.types[i] = -1;
		}
		played = false;
	}
	tPacenote() {
		Reset();
	}

	tPacenoteSpeech* GetSpeech(int id) {
		if (id < 0 || id >= nMaxSpeechesPerPacenote) return nullptr;
		if (data.types[id] < 0 || data.types[id] >= sizeof(aPacenoteSpeeches)/sizeof(aPacenoteSpeeches[0])) return nullptr;
		return &aPacenoteSpeeches[data.types[id]];
	}
	std::string GetSpeechName(int id) {
		auto speech = GetSpeech(id);
		if (speech) return speech->GetName();
		return "None";
	}
	std::string GetDisplayName() {
		std::string str;
		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			auto speech = GetSpeech(i);
			if (!speech) continue;
			if (!str.empty()) str += ", ";
			str += speech->speechName;
		}
		return str;
	}
	bool IsInRange() {
		auto ply = GetPlayer(0)->pCar->GetMatrix()->p;
		return (ply - data.pos).length() < fPacenoteRange;
	}
	void Play() {
		if (played) return;
		played = true;

		/*struct tPacenoteGroup {
			int begin;
			int end;
			std::string combinedFile;
		};
		std::vector<tPacenoteGroup> groups;

		bool isGrouped[nMaxSpeechesPerPacenote] = {};
		std::string groupLine;
		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			auto speech = GetSpeech(i);
			if (!speech) continue;

			if (std::filesystem::exists(groupLine + speech->speechFile)) {
				groupLine += speech->speechFile;
			}
			else {
				groupLine += speech->speechFile;
			}

			tPacenotePlaying note;
			note.speech = speech;
			aPacenoteQueue.push_back(note);
		}*/

		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			auto speech = GetSpeech(i);
			if (!speech) continue;

			tPacenotePlaying note;
			note.speech = speech;
			aPacenoteQueue.push_back(note);
		}
	}
};
std::vector<tPacenote> aPacenotes;

struct tExtraPacenote {
	const std::string speechFile;
	NyaAudio::NyaSound audio = 0;

	void Play() {
		if (nPacenoteVolume == 0) return;
		audio = tPacenoteSpeech::PlaySpeech(speechFile);
	}
	void Process() {
		if (audio && NyaAudio::IsFinishedPlaying(audio)) {
			NyaAudio::Delete(&audio);
		}
	}
};
tExtraPacenote EjectPacenote = {"Windscreen"};

std::string GetPacenoteFilename() {
	return (std::string)"Config/Pacenotes/" + GetTrackName(pGameFlow->nLevelId) + ".pac";
}

void AddPacenote(tPacenote note) {
	if (note.data.types[0] < 0) return;
	aPacenotes.push_back(note);
}

void SavePacenotes() {
	std::filesystem::create_directory("Config");
	std::filesystem::create_directory("Config/Pacenotes");

	std::ofstream fout(GetPacenoteFilename(), std::ios::out | std::ios::binary );
	if (!fout.is_open()) return;

	uint32_t count = aPacenotes.size();
	fout.write((char*)&count, 4);
	for (int i = 0; i < count; i++) {
		fout.write((char*)&aPacenotes[i].data, sizeof(tPacenote::tData));
	}
}

int nLastPacenote = 0;
void LoadPacenotes() {
	aPacenotes.clear();

	std::ifstream fin(GetPacenoteFilename(), std::ios::in | std::ios::binary );
	if (!fin.is_open()) return;

	uint32_t count = 0;
	fin.read((char*)&count, 4);
	aPacenotes.reserve(count);
	for (int i = 0; i < count; i++) {
		if (fin.eof()) return;

		tPacenote note;
		fin.read((char*)&note.data, sizeof(note.data));
		AddPacenote(note);
	}
}

float GetCoordProgressInStage(NyaVec3 coord) {
	auto track = pTrackAI->pTrack;
	auto start = track->aStartpoints[0].fPosition;
	auto end = track->aSplitpoints[track->nNumSplitpoints-1].fPosition;
	auto vStart = NyaVec3(start[0],0,start[2]);
	auto vEnd = NyaVec3(end[0],0,end[2]);
	auto dist = (vStart - vEnd).length();
	auto progress = 1 - (((coord - vStart) - (vEnd - vStart)).length() / dist);
	if (progress < 0) progress = 0;
	if (progress > 1) progress = 1;
	return progress;
}

float GetPlayerProgressInStage() {
	return GetCoordProgressInStage(GetPlayer(0)->pCar->GetMatrix()->p);
}

double Get1080pToAspectX(double x) {
	auto f = x / 1920.0;
	f *= 16.0 / 9.0;
	f /= GetAspectRatio();
	return f;
}

double Get1080pToAspectY(double y) {
	return y / 1080.0;
}

void DrawRectangle_1080pScaled(float left, float top, float xSize, float ySize, NyaDrawing::CNyaRGBA32 rgb) {
	auto l = Get1080pToAspectX(left);
	auto r = Get1080pToAspectX(left + xSize);
	auto t = Get1080pToAspectY(top);
	auto b = Get1080pToAspectY(top + ySize);
	DrawRectangle(l,r,t,b,rgb);
}

void DrawPlayerOnRallyMap(Player* ply) {
	if (!ply) return;
	auto pos = ply->pCar->GetMatrix()->p;
	auto replayPos = NyaVec3(500,-25,500);
	if (ply->nGhosting && (pos - replayPos).length() < 1) return; // ignore empty replay ghosts

	// 1 pixel off so it has less chance to look wrong
	int top = 160;
	int bottom = 811;

	//float markerYSize = 0.008;
	float markerYSize = 0.0065;
	float markerXSize = markerYSize * GetAspectRatioInv();
	float markerOutlineXSize = markerXSize * 1.2;
	float markerOutlineYSize = markerYSize * 1.2;
	float markerPlayerOutlineXSize = markerXSize * 1.5;
	float markerPlayerOutlineYSize = markerYSize * 1.5;

	float x = Get1080pToAspectX(111.5);

	//NyaDrawing::CNyaRGBA32 localPlayerHighlightColor = {241,99,33,255};
	//NyaDrawing::CNyaRGBA32 localPlayerHighlightColor = {255,255,0,255};
	NyaDrawing::CNyaRGBA32 localPlayerHighlightColor = {241,193,45,255};

	auto y = Get1080pToAspectY(std::lerp(bottom, top, GetCoordProgressInStage(pos)));
	auto tmp = *(NyaDrawing::CNyaRGBA32*)&ply->nArrowColor;
	auto color = NyaDrawing::CNyaRGBA32(tmp.b, tmp.g, tmp.r, tmp.a);
	if (pGameFlow->nGameMode == GM_CAREER && ply->nPlayerId > 1) {
		switch (ply->nPlayerId) {
			// gold
			case 2:
				color.r = 236;
				color.g = 221;
				color.b = 16;
				break;
			// silver
			case 3:
				color.r = 186;
				color.g = 186;
				color.b = 186;
				break;
			// bronze
			case 4:
				color.r = 175;
				color.g = 100;
				color.b = 0;
				break;
			// author
			case 5:
				color.r = 30;
				color.g = 160;
				color.b = 0;
				break;
			default:
				break;
		}
	}
	if (ply->nPlayerId == 1) DrawRectangle(x - markerPlayerOutlineXSize, x + markerPlayerOutlineXSize, y - markerPlayerOutlineYSize, y + markerPlayerOutlineYSize, localPlayerHighlightColor, 1);
	DrawRectangle(x - markerOutlineXSize, x + markerOutlineXSize, y - markerOutlineYSize, y + markerOutlineYSize, {0,0,0,255}, 1);
	DrawRectangle(x - markerXSize, x + markerXSize, y - markerYSize, y + markerYSize, color, 1);
}

void DrawRallyHUD() {
	// left transparent bg
	DrawRectangle_1080pScaled(81, 159, 18, 653, {0,0,0,50});
	// right transparent bg
	DrawRectangle_1080pScaled(99, 159, 24, 653, {255,255,255,50});

	auto playerProgress = GetPlayerProgressInStage();

	// left filled bg
	DrawRectangle(Get1080pToAspectX(81), Get1080pToAspectX(81+18), Get1080pToAspectY(std::lerp(811,160,playerProgress)), Get1080pToAspectY(811), {241,99,33,255});

	// splitpoints
	auto track = pTrackAI->pTrack;
	for (int i = 0; i < track->nNumSplitpoints; i++) {
		auto pos = track->aSplitpoints[i].fPosition;

		// 1 pixel off so it has less chance to look wrong
		int top = 160;
		int bottom = 811;

		auto y = std::lerp(bottom, top, GetCoordProgressInStage({pos[0],pos[1],pos[2]}));
		DrawRectangle(Get1080pToAspectX(81), Get1080pToAspectX(81+18), Get1080pToAspectY(y-1), Get1080pToAspectY(y+1), {0,0,0,127});
	}

	// top line
	DrawRectangle_1080pScaled(81, 159, 42, 2, {22,22,22,255});
	// bottom line
	DrawRectangle_1080pScaled(81, 810, 42, 2, {22,22,22,255});
	// left line
	DrawRectangle_1080pScaled(81, 161-1, 2, 649+1, {22,22,22,255});
	// right line
	DrawRectangle_1080pScaled(97, 161-1, 2, 649+1, {22,22,22,255});
	// right top line
	DrawRectangle_1080pScaled(121, 161-1, 2, 5, {22,22,22,255});
	// right bottom line
	DrawRectangle_1080pScaled(121, 806, 2, 4+1, {22,22,22,255});

	for (int i = 1; i < 32; i++) {
		DrawPlayerOnRallyMap(GetPlayer(i));
	}
	DrawPlayerOnRallyMap(GetPlayer(0));
}

void ProcessPacenotes() {
	static bool bInited = false;
	if (!bInited) {
		NyaAudio::Init(pDeviceD3d->hWnd);
		bInited = true;
	}

	gPacenoteTimer.Process();
	bool isNextInQueue = true;
	for (auto& note : aPacenoteQueue) {
		note.Process(isNextInQueue, aPacenoteQueue.size() == 1);
		if (!note.audioFinished) isNextInQueue = false;
	}
	if (!aPacenoteQueue.empty()) {
		auto& note = aPacenoteQueue[0];
		if (note.audioFinished && note.visualTimer >= 3) {
			aPacenoteQueue.erase(aPacenoteQueue.begin());
		}
	}
	EjectPacenote.Process();

	if (AreVisualPacenotesEnabled()) DrawVisualPacenotes();
	aVisualPacenotes.clear();

	if (pGameFlow->nGameState != GAME_STATE_RACE) return;
	if (aPacenotes.empty()) return;
	for (auto& note : aPacenoteSpeeches) {
		note.LoadTexture();
	}

	if (pLoadingScreen || !GetPlayer(0)) return;
	if (pGameFlow->nRaceState != RACE_STATE_RACING) {
		ClearPacenoteQueue();
		sLastPlayedPacenoteSpeech = "";
		for (auto& note : aPacenotes) {
			note.played = false;
		}
	}
	else {
		static bool bLastEjected = false;
		bool bEjected = GetPlayer(0)->pCar->nIsRagdolled;
		if (!bLastEjected && bEjected) {
			EjectPacenote.Play();
		}
		bLastEjected = bEjected;

		for (auto& note : aPacenotes) {
			if (!note.played && note.IsInRange()) {
				note.Play();
				nLastPacenote = &note - &aPacenotes[0];
			}
		}

		if (!ChloeMenuLib::IsMenuOpen()) DrawRallyHUD();
	}
}

void LoadPacenoteConfigs() {
	static auto config = toml::parse_file("Config/Pacenotes.toml");
	int numPacenoteSpeech = config["main"]["pacenote_speech_count"].value_or(1);
	int numPacenoteVisual = config["main"]["pacenote_visual_count"].value_or(1);
	int defaultSpeech = config["main"]["default_speech"].value_or(1) - 1;
	int defaultVisual = config["main"]["default_visual"].value_or(1) - 1;
	for (int i = 0; i < numPacenoteSpeech; i++) {
		tPacenoteType type;
		type.name = config[std::format("pacenotespeech{}", i+1)]["name"].value_or(L"");
		type.folder = config[std::format("pacenotespeech{}", i+1)]["folder"].value_or("");
		if (type.name.empty()) continue;
		if (type.folder.empty()) continue;
		aPacenoteSpeechTypes.push_back(type);
	}
	for (int i = 0; i < numPacenoteVisual; i++) {
		tPacenoteType type;
		type.name = config[std::format("pacenotevisual{}", i+1)]["name"].value_or(L"");
		type.folder = config[std::format("pacenotevisual{}", i+1)]["folder"].value_or("");
		if (type.name.empty()) continue;
		//if (type.folder.empty()) continue;
		aPacenoteVisualTypes.push_back(type);
	}
	if (defaultSpeech < 0 || defaultSpeech >= aPacenoteSpeechTypes.size()) defaultSpeech = 0;
	if (defaultVisual < 0 || defaultVisual >= aPacenoteVisualTypes.size()) defaultVisual = 0;
	nPacenoteType = defaultSpeech;
	nPacenoteVisualType = defaultVisual;

	for (auto& setting : aNewGameSettings) {
		if (setting.value == &nPacenoteType) setting.maxValue = aPacenoteSpeechTypes.size()-1;
		if (setting.value == &nPacenoteVisualType) setting.maxValue = aPacenoteVisualTypes.size()-1;
	}
}