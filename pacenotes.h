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
	IDirect3DTexture9* pTexture = nullptr;
	int textureType = -1;

	static NyaAudio::NyaSound PlaySpeech(const std::string& file) {
		if (file.empty()) return 0;

		std::string folder = "rt_eng";
		if (nPacenoteType < aPacenoteSpeechTypes.size()) folder = aPacenoteSpeechTypes[nPacenoteType].folder;

		auto sound = NyaAudio::LoadFile(("data/sound/rally/" + folder + "/" + file + ".wav").c_str());
		if (!sound) return 0;
		NyaAudio::SetVolume(sound, nPacenoteVolume / 100.0);
		NyaAudio::Play(sound);
		return sound;
	}

	NyaAudio::NyaSound Play(bool useFallback) {
		auto sound = PlaySpeech(speechFile);
		if (!sound && useFallback) return PlaySpeech(speechFileFallback);
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
		if (!pTexture) {
			pTexture = ::LoadTexture((pacenotePath + speechFileFallback + ".png").c_str());
		}
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
		{"Into (placeholder)", "Into"},
		{"Crest (placeholder)", "Crest", "JumpMaybe"},
		{"Over Crest (placeholder)", "OverCrest", "JumpMaybe"},
		{"Jump", "Jump"},
		{"Jump Maybe", "JumpMaybe"},
		{"Opens", "Opens"},
		{"Straight", "Straight"},
		{"Tightens", "Tightens"},
		{"Water Splash (placeholder)", "WaterSplash"},
		{"Careful", "Careful"},
		{"Careful - Bridge", "CarefulBridge", "Careful"},
		{"Careful - Tunnel", "CarefulTunnel", "Careful"},
		{"Careful - Water (placeholder)", "CarefulWater", "Careful"},
		{"Caution", "Caution"},
		{"Caution - Ditch", "CautionDitch", "Caution"},
		{"Caution - Logs", "Cautionlogs", "Caution"},
		{"Caution - Post", "CautionPost", "Caution"},
		{"Caution - Rocks", "CautionRocks", "Caution"},
		{"Caution - Water (placeholder)", "CautionWater", "Caution"},
		{"Don't Cut", "Dontcut"},
		{"Don't Cut - Ditch", "Dontcutditch", "Dontcut"},
		{"Don't Cut - Logs", "Dontcutlogs", "Dontcut"},
		{"Don't Cut - Post", "Dontcutpost", "Dontcut"},
		{"Don't Cut - Rocks", "Dontcutrocks", "Dontcut"},
		{"Don't Cut - Water (placeholder)", "DontcutWater", "Dontcut"},
		{"Finish", "Finish"},
		{"Big Jump (placeholder)", "BigJump", "Jump"},
		{"Jumps (placeholder)", "Jumps", "Jump"},
		{"Double Caution (placeholder)", "DoubleCaution", "Caution"},
		{"Long (placeholder)", "Long"},
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
	NyaAudio::NyaSound audio = 0;
	double visualTimer = 0;
	double visualAppearTimer = 0;
	bool audioPlaying = false;
	bool audioFinished = false;

	static bool CanPlayAudio() {
		if (pLoadingScreen || pGameFlow->nRaceState != RACE_STATE_RACING) return false;
		auto ply = GetPlayer(0);
		if (!ply) return false;
		if (ply->pCar->nIsRagdolled) return false;
		return true;
	}

	void Play(bool useFallback) {
		audioPlaying = true;
		audio = speech->Play(useFallback);
		if (!audio) return;
		visualTimer = 0;
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

			// hack for repeats of fallback speeches
			bool useFallback = true;
			if (sLastPlayedPacenoteSpeech == speech->speechFileFallback || (sLastPlayedPacenoteSpeech == "Jump" && speech->speechFileFallback == "JumpMaybe")) useFallback = false;
			sLastPlayedPacenoteSpeech = speech->speechFile;

			audio = speech->Play(useFallback);
			if (!audio) {
				audioFinished = true;
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
		if (speech) return speech->speechName;
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

		float x = 0.1 * GetAspectRatioInv();
		float y = 0.45;
		float xSize = 0.0025 * GetAspectRatioInv();
		float ySize = 0.3;
		float markerXSize = 0.008 * GetAspectRatioInv();
		float markerYSize = 0.008;
		float markerOutlineXSize = markerXSize * 1.2;
		float markerOutlineYSize = markerYSize * 1.2;
		float markerPlayerOutlineXSize = markerXSize * 1.5;
		float markerPlayerOutlineYSize = markerYSize * 1.5;
		float lineXSize = 0.025 * GetAspectRatioInv();
		float lineYSize = 0.0025;
		float top = y - ySize;
		float bottom = y + ySize;
		DrawRectangle(x - xSize, x + xSize, top, bottom, {0,0,0,255});
		DrawRectangle(x - lineXSize, x + lineXSize, top - lineYSize, top + lineYSize, {0,0,0,255});
		DrawRectangle(x - lineXSize, x + lineXSize, bottom - lineYSize, bottom + lineYSize, {0,0,0,255});

		auto track = pTrackAI->pTrack;
		for (int i = 0; i < track->nNumSplitpoints; i++) {
			auto pos = track->aSplitpoints[i].fPosition;
			auto y = std::lerp(bottom, top, GetCoordProgressInStage({pos[0],pos[1],pos[2]}));
			DrawRectangle(x - lineXSize, x + lineXSize, y - lineYSize, y + lineYSize, {0,0,0,255});
		}

		for (int i = 1; i < 32; i++) {
			auto ply = GetPlayer(i);
			if (!ply) continue;
			auto pos = ply->pCar->GetMatrix()->p;
			auto replayPos = NyaVec3(500,-25,500);
			if (ply->nGhosting && (pos - replayPos).length() < 1) continue; // ignore empty replay ghosts

			float playerY = std::lerp(bottom, top, GetCoordProgressInStage(pos));
			auto tmp = *(NyaDrawing::CNyaRGBA32*)&ply->nArrowColor;
			auto color = NyaDrawing::CNyaRGBA32(tmp.b, tmp.g, tmp.r, tmp.a);
			DrawRectangle(x - markerOutlineXSize, x + markerOutlineXSize, playerY - markerOutlineYSize, playerY + markerOutlineYSize, {0,0,0,255}, 1);
			DrawRectangle(x - markerXSize, x + markerXSize, playerY - markerYSize, playerY + markerYSize, color, 1);
		}

		// always draw local player on top
		auto tmp = *(NyaDrawing::CNyaRGBA32*)&GetPlayer(0)->nArrowColor;
		auto color = NyaDrawing::CNyaRGBA32(tmp.b, tmp.g, tmp.r, tmp.a);
		float playerY = std::lerp(bottom, top, GetPlayerProgressInStage());
		DrawRectangle(x - markerPlayerOutlineXSize, x + markerPlayerOutlineXSize, playerY - markerPlayerOutlineYSize, playerY + markerPlayerOutlineYSize, {255,255,255,255}, 1);
		DrawRectangle(x - markerOutlineXSize, x + markerOutlineXSize, playerY - markerOutlineYSize, playerY + markerOutlineYSize, {0,0,0,255}, 1);
		DrawRectangle(x - markerXSize, x + markerXSize, playerY - markerYSize, playerY + markerYSize, color, 1);
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