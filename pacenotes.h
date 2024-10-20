const float fPacenoteRange = 20.0;
const int nMaxSpeechesPerPacenote = 8;

struct tPacenoteSpeech {
	const std::string speechName;
	const std::string speechFile;
	const std::string speechFileFallback;

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
};

tPacenoteSpeech aPacenoteSpeeches[] = {
		{"Left 1", "Left1"},
		{"Left 2", "Left2"},
		{"Left 3", "Left3"},
		{"Left 4", "Left4"},
		{"Left 5", "Left5"},
		{"Left Hairpin", "LeftHairPin"},
		{"Left Turn", "LeftTurn"},
		{"Right 1", "Rightone"},
		{"Right 2", "Righttwo"},
		{"Right 3", "Rightthree"},
		{"Right 4", "Rightfour"},
		{"Right 5", "Rightfive"},
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
		{"Careful - Bridge", "CarefulBridge"},
		{"Careful - Tunnel", "CarefulTunnel"},
		{"Careful - Water (placeholder)", "CarefulWater", "Careful"},
		{"Caution", "Caution"},
		{"Caution - Ditch", "CautionDitch"},
		{"Caution - Logs", "Cautionlogs"},
		{"Caution - Post", "CautionPost"},
		{"Caution - Rocks", "CautionRocks"},
		{"Caution - Water (placeholder)", "CautionWater", "Caution"},
		{"Don't Cut", "Dontcut"},
		{"Don't Cut - Ditch", "Dontcutditch"},
		{"Don't Cut - Logs", "Dontcutlogs"},
		{"Don't Cut - Post", "Dontcutpost"},
		{"Don't Cut - Rocks", "Dontcutrocks"},
		{"Don't Cut - Water (placeholder)", "DontcutWater", "Dontcut"},
		{"Finish", "Finish"},
};

struct tPacenote {
	struct tData {
		NyaVec3 pos;
		int types[nMaxSpeechesPerPacenote];
	} data;
	NyaAudio::NyaSound audios[nMaxSpeechesPerPacenote];
	bool audioMissing[nMaxSpeechesPerPacenote];

	void Reset() {
		data.pos = {0,0,0};
		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			data.types[i] = -1;
			audios[i] = 0;
			audioMissing[i] = false;
		}
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
	void Play(int id, bool useFallback) {
		if (id >= nMaxSpeechesPerPacenote) return;
		if (nPacenoteVolume == 0) return;

		auto speech = GetSpeech(id);
		if (!speech) return;
		audios[id] = speech->Play(useFallback);
		if (!audios[id]) audioMissing[id] = true;
	}
	void Process() {
		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			std::string oldSpeechFile;
			if (auto speech = GetSpeech(i)) {
				oldSpeechFile = speech->speechFile;
			}

			if (audioMissing[i] || (audios[i] && NyaAudio::IsFinishedPlaying(audios[i]))) {
				NyaAudio::Delete(&audios[i]);
				audioMissing[i] = false;
				bool useFallback = true;
				if (auto speech = GetSpeech(i+1)) {
					if (oldSpeechFile == speech->speechFileFallback || oldSpeechFile == "Jump" && speech->speechFileFallback == "JumpMaybe") {
						useFallback = false;
					}
				}
				Play(i+1, useFallback);
			}
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

int nNextPacenote = 0;
void LoadPacenotes() {
	aPacenotes.clear();
	nNextPacenote = 0;

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

	for (auto& note : aPacenotes) {
		note.Process();
	}
	EjectPacenote.Process();

	if (pGameFlow->nGameState != GAME_STATE_RACE) return;
	if (aPacenotes.empty()) return;
	if (pLoadingScreen || !GetPlayer(0)) return;

	static bool bLastEjected = false;
	bool bEjected = GetPlayer(0)->pCar->nIsRagdolled;
	if (!bLastEjected && bEjected) {
		EjectPacenote.Play();
	}
	bLastEjected = bEjected;

	// forward
	if (nNextPacenote <= aPacenotes.size()) {
		auto note = &aPacenotes[nNextPacenote];
		if (note->IsInRange()) {
			nNextPacenote++;
			note->Play(0, true);
		}
	}

	// reverse
	for (int i = 0; i < nNextPacenote - 1; i++) {
		auto note = &aPacenotes[i];
		if (note->IsInRange()) {
			nNextPacenote = i + 1;
			note->Play(0, true);
			break;
		}
	}
}

