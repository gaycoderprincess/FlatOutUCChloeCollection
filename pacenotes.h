const float fPacenoteRange = 20.0;
const int nMaxSpeechesPerPacenote = 8;

struct tPacenoteSpeech {
	std::string speechName;
	std::string speechFile;
	std::string speechFileFallback;
	IDirect3DTexture9* pTexture = nullptr;
	int textureType = -1;

	static NyaAudio::NyaSound PlaySpeech(const std::string& file) {
		if (file.empty()) return 0;

		std::string folder;
		switch (nPacenoteType) {
			// RT finnish
			case 1:
				folder = "rt_fin";
			// RT english
			case 0:
			default:
				folder = "rt_eng";
		}

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
		if (textureType == nPacenoteVisualType) return;
		if (pTexture) {
			pTexture->Release();
			pTexture = nullptr;
		}

		std::string pacenotePath = "data/textures/pacenotes/";
		if (nPacenoteVisualType == 2) pacenotePath = "data/textures/pacenotes_teddyator/";

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

struct tPacenotePlaying {
	tPacenoteSpeech* speech = nullptr;
	NyaAudio::NyaSound audio = 0;
	double visualTimer = 0;
	double visualAppearTimer = 0;
	bool audioPlaying = false;
	bool audioFinished = false;
	bool audioSkipped = false;

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

			// todo reimplement usefallback
			audio = speech->Play(true);
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

	if (nPacenoteVisualType) DrawVisualPacenotes();
	aVisualPacenotes.clear();

	if (pGameFlow->nGameState != GAME_STATE_RACE) return;
	if (aPacenotes.empty()) return;
	for (auto& note : aPacenoteSpeeches) {
		note.LoadTexture();
	}

	if (pLoadingScreen || !GetPlayer(0)) return;
	if (pGameFlow->nRaceState != RACE_STATE_RACING) {
		ClearPacenoteQueue();
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
	}
}

