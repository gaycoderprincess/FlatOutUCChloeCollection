bool IsRallyTrack() {
	if (!DoesTrackValueExist(pGameFlow->PreRace.nLevel, "ForceOneLapOnly")) return false;
	if (strcmp(GetTrackValueString(pGameFlow->PreRace.nLevel, "GfxSetPath"), "data/Tracks/Rally/")) return false;
	return true;
}

bool IsInRallyMode() {
	if (pGameFlow->nGameState != GAME_STATE_RACE) return false;
	if (pLoadingScreen || !GetPlayer(0)) return false;
	if (!IsRallyTrack()) return false;
	return true;
}

bool ShouldUseReVoltPacenotes() {
	return DoesTrackValueExist(pGameFlow->PreRace.nLevel, "UseReVoltPacenotes");
}

const float fPacenoteRange = 20.0;
const int nMaxSpeechesPerPacenote = 8;

struct tPacenoteType {
	std::wstring name;
	std::string folder;
	std::string format;
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
		std::string format = ".wav";
		if (nPacenoteType < aPacenoteSpeechTypes.size()) {
			folder = aPacenoteSpeechTypes[nPacenoteType].folder;
			format = aPacenoteSpeechTypes[nPacenoteType].format;
		}
		return "data/sound/rally/" + folder + "/" + file + format;
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
		bool isRevolt = ShouldUseReVoltPacenotes();

		if (!isRevolt && IsMissing()) return speechName + " (Missing)";
		if (!isRevolt && IsPlaceholder()) return speechName + " (Placeholder)";
		return speechName;
	}

	static NyaAudio::NyaSound PlaySpeech(const std::string& file) {
		if (file.empty()) return 0;
		if (ShouldUseReVoltPacenotes()) return 0;

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

		std::string pacenotePath = "data/textures/" + (ShouldUseReVoltPacenotes() ? "pacenotes_revolt" : aPacenoteVisualTypes[nPacenoteVisualType].folder) + "/";
		pTexture = ::LoadTexture((pacenotePath + speechFile + ".png").c_str());
		if (!pTexture) pTexture = ::LoadTexture((pacenotePath + speechFileFallback + ".png").c_str());
		if (!pTexture) pTexture = ::LoadTexture((pacenotePath + speechFileFallback2 + ".png").c_str());
		textureType = nPacenoteVisualType;
	}
};

std::vector<tPacenoteSpeech> aPacenoteSpeechesSimple = {
		{"Left", "Left"},
		{"Right", "Right"},
		{"Slight Left", "SlightLeft"},
		{"Slight Right", "SlightRight"},
		{"Left Entry Chicane", "LeftChicane"},
		{"Right Entry Chicane", "RightChicane"},
		{"Caution", "Caution"},
		{"Fork", "Fork"},
		{"Straight", "Straight"},
		{"Hairpin Left", "HairpinLeft"},
		{"Hairpin Right", "HairpinRight"},
};

std::vector<tPacenoteSpeech> aPacenoteSpeechesRally = {
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
		{"And", "And"},
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
		{"Extra Long", "ExtraLong", "Long"},
		{"Left Entry Chicane","LeftEntryChicane"},
		{"Right Entry Chicane","RightEntryChicane"},
		{"Over", "Over"},
		{"Onto Concrete", "OntoConcrete"},
		{"Uphill", "Uphill"},
		{"Immediate", "Immediate"},
		{"Onto Snow", "OntoSnow"},
		{"Onto Mud", "OntoMud"},
		{"Caution - Fence", "CautionFence", "Caution"},
		{"Don't Cut - Fence", "DontcutFence", "Dontcut"},
};

std::vector<tPacenoteSpeech>* GetPacenoteDB() {
	if (ShouldUseReVoltPacenotes()) return &aPacenoteSpeechesSimple;
	return &aPacenoteSpeechesRally;
}

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
	if (ShouldUseReVoltPacenotes()) {
		sizeX *= 1.4;
		sizeY *= 1.4;
	}
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
	int playedLap = -1;
	int atSplit = -1;

	void Reset() {
		data.pos = {0,0,0};
		for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
			data.types[i] = -1;
		}
		playedLap = -1;
		atSplit = -1;
	}
	tPacenote() {
		Reset();
	}

	tPacenoteSpeech* GetSpeech(int id) {
		auto& pacenotes = *GetPacenoteDB();
		if (id < 0 || id >= nMaxSpeechesPerPacenote) return nullptr;
		if (data.types[id] < 0 || data.types[id] >= pacenotes.size()) return nullptr;
		return &pacenotes[data.types[id]];
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
		int lap = GetPlayer(0)->nCurrentLap;
		if (playedLap == lap) return;
		if (atSplit >= 0 && (GetPlayer(0)->nCurrentSplit % pEnvironment->nNumSplitpoints) != atSplit) return;
		playedLap = lap;

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
	return (std::string)"Config/Pacenotes/" + GetTrackName(pGameFlow->PreRace.nLevel) + ".pac";
}

std::string GetPacenoteFilenameWithVoice() {
	std::string type;
	if (nPacenoteType < aPacenoteSpeechTypes.size()) {
		type = aPacenoteSpeechTypes[nPacenoteType].folder;
	}
	return (std::string)"Config/Pacenotes/" + GetTrackName(pGameFlow->PreRace.nLevel) + " " + type + ".pac";
}

void AddPacenote(tPacenote note) {
	int numValid = 0;
	for (int i = 0; i < nMaxSpeechesPerPacenote; i++) {
		if (note.data.types[i] >= 0) numValid++;
	}
	if (numValid <= 0) return;
	aPacenotes.push_back(note);
}

void SavePacenotes(const std::string& filename) {
	std::filesystem::create_directory("Config");
	std::filesystem::create_directory("Config/Pacenotes");

	std::ofstream fout(filename, std::ios::out | std::ios::binary );
	if (!fout.is_open()) return;

	uint32_t count = aPacenotes.size();
	fout.write((char*)&count, 4);
	for (int i = 0; i < count; i++) {
		fout.write((char*)&aPacenotes[i].data, sizeof(tPacenote::tData));
		if (!IsRallyTrack()) {
			fout.write((char*)&aPacenotes[i].atSplit, sizeof(aPacenotes[i].atSplit));
		}
	}
}

int nLastPacenote = 0;
bool LoadPacenotes(const std::string& filename) {
	aPacenotes.clear();

	std::ifstream fin(filename, std::ios::in | std::ios::binary );
	if (!fin.is_open()) return false;

	uint32_t count = 0;
	fin.read((char*)&count, 4);
	aPacenotes.reserve(count);
	for (int i = 0; i < count; i++) {
		if (fin.eof()) return true;

		tPacenote note;
		fin.read((char*)&note.data, sizeof(note.data));
		if (!IsRallyTrack()) {
			fin.read((char*)&note.atSplit, sizeof(note.atSplit));
		}
		AddPacenote(note);
	}
	return true;
}

struct tRallySplitpoint {
	NyaVec3 vPos;
	double fAbsoluteMapPos;
	double fPercentMapPos;
};
tRallySplitpoint aRallySplitpoints[32];
tRallySplitpoint gRallyInitialSplit;

float GetPlayerProgressInStage(Player* ply) {
	auto pos = ply->pCar->GetMatrix()->p;
	pos.y = 0;
	//auto progress = GetCoordProgressInStage(pos);
	float progress = 0;
	if (ply->nCurrentSplit >= pTrackAI->pTrack->nNumSplitpoints) {
		progress = 1;
	}
	else if (ply->nCurrentSplit >= 0) {
		auto split = ply->nCurrentSplit > 0 ? aRallySplitpoints[ply->nCurrentSplit-1] : gRallyInitialSplit;
		auto splitNext = aRallySplitpoints[ply->nCurrentSplit];

		auto start = split.fPercentMapPos;
		auto end = splitNext.fPercentMapPos;

		auto splitDist = (split.vPos - splitNext.vPos).length();
		auto plyDistFromNext = (pos - splitNext.vPos).length();

		auto delta = 1 - (plyDistFromNext / splitDist);
		//if (delta < 0) delta = 0;
		//if (delta > 1) delta = 1;
		progress = std::lerp(start,end, delta);
	}
	if (progress < 0) progress = 0;
	if (progress > 1) progress = 1;
	return progress;
}

float GetLocalPlayerProgressInStage() {
	return GetPlayerProgressInStage(GetPlayer(0));
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

	auto progress = GetPlayerProgressInStage(ply);

	auto y = Get1080pToAspectY(std::lerp(bottom, top, progress));
	auto tmp = *(NyaDrawing::CNyaRGBA32*)&ply->nArrowColor;
	auto color = NyaDrawing::CNyaRGBA32(tmp.b, tmp.g, tmp.r, tmp.a);
	if (ply->nPlayerId == 1) DrawRectangle(x - markerPlayerOutlineXSize, x + markerPlayerOutlineXSize, y - markerPlayerOutlineYSize, y + markerPlayerOutlineYSize, localPlayerHighlightColor, 1);
	DrawRectangle(x - markerOutlineXSize, x + markerOutlineXSize, y - markerOutlineYSize, y + markerOutlineYSize, {0,0,0,255}, 1);
	DrawRectangle(x - markerXSize, x + markerXSize, y - markerYSize, y + markerYSize, color, 1);
}

double fSplitTimer = 0;
double fSplitDiffTimer = 0;
uint32_t nLastSplitTime = 0;
uint32_t nLastSplitID = 0;
float fLastSplitSpeed = 0;

uint32_t nSplitTimes[32][512];
float fSplitSpeeds[32][512];
uint32_t GetFastestOpponentTimeForSplit(int id) {
	uint32_t out = 0;
	for (int i = 1; i < 32; i++) {
		auto split = nSplitTimes[i][id];
		if (!split) continue;
		if (!out || split < out) {
			out = split;
		}
	}
	return out;
}

float GetFastestOpponentSpeedForSplit(int id) {
	float out = 0;
	for (int i = 1; i < 32; i++) {
		auto split = fSplitSpeeds[i][id];
		if (split > out) {
			out = split;
		}
	}
	return out;
}

bool HasAnyOpponentCrossedThisSplit(int id) {
	for (int i = 1; i < 32; i++) {
		if (nSplitTimes[i][id]) return true;
	}
	return false;
}

bool HasLocalPlayerCrossedThisSplit(int id) {
	return nSplitTimes[0][id] != 0;
}

void DrawSplitHUD() {
	static CNyaTimer gSplitTimer;
	gSplitTimer.Process();

	tNyaStringData data;
	data.x = 0.5;
	data.y = 0.3;
	data.size = 0.04;
	if (aPacenotes.empty()) data.y -= data.size;
	data.XCenterAlign = true;
	data.outlinea = 255;
	if (fSplitTimer < 0.5) {
		data.a = data.outlinea = fSplitTimer * 2 * 255;
	}

	if (fSplitTimer > 0) {
		auto str = GetTimeFromMilliseconds(nLastSplitTime, true);
		str.pop_back(); // remove trailing zero
		DrawString(data, str, &DrawStringFO2);
	}

	if (fSplitDiffTimer > 0) {
		auto str = GetTimeFromMilliseconds(nLastSplitTime, true);
		str.pop_back(); // remove trailing zero

		data.a = 255;
		if (fSplitDiffTimer < 0.5) {
			data.a = data.outlinea = fSplitDiffTimer * 2 * 255;
		}

		auto comp = (int32_t)GetFastestOpponentTimeForSplit(nLastSplitID);
		if (comp && nSplitType != 0) {
			auto diff = (int32_t)nLastSplitTime - comp;

			bool ahead = diff <= 0;
			if (ahead) {
				diff *= -1;
				data.r = 0;
				data.g = 255;
				data.b = 0;
				str = "-" + GetTimeFromMilliseconds(diff, true);
			} else {
				data.r = 255;
				data.g = 0;
				data.b = 0;
				str = "+" + GetTimeFromMilliseconds(diff, true);
			}

			str.pop_back(); // remove trailing zero
			data.y += data.size;
			DrawString(data, str, &DrawStringFO2);
		}

		comp = GetFastestOpponentSpeedForSplit(nLastSplitID);
		if (comp && nSplitType >= 2) {
			auto diff = fLastSplitSpeed - comp;

			bool ahead = diff <= 0;
			if (ahead) {
				diff *= -1;
				data.r = 255;
				data.g = 0;
				data.b = 0;
				str = std::format("-{:.0f} KMH",diff);
			} else {
				data.r = 0;
				data.g = 255;
				data.b = 0;
				str = std::format("+{:.0f} KMH",diff);
			}

			data.y += data.size;
			DrawString(data, str, &DrawStringFO2);
		}
	}
	fSplitTimer -= gSplitTimer.fDeltaTime;
	fSplitDiffTimer -= gSplitTimer.fDeltaTime;
}

void DrawRallyHUD() {
	if (pGameFlow->nIsPauseMenuUp) return;

	// left transparent bg
	DrawRectangle_1080pScaled(81, 159, 18, 653, {0,0,0,50});
	// right transparent bg
	DrawRectangle_1080pScaled(99, 159, 24, 653, {255,255,255,50});

	auto playerProgress = GetLocalPlayerProgressInStage();

	// left filled bg
	DrawRectangle(Get1080pToAspectX(81), Get1080pToAspectX(81+18), Get1080pToAspectY(std::lerp(811,160,playerProgress)), Get1080pToAspectY(811), {241,99,33,255});

	// splitpoints
	auto track = pTrackAI->pTrack;

	double tmpMapPos = 0;
	auto lastSplitpointPos = NyaVec3(track->aStartpoints[0].fPosition[0],0,track->aStartpoints[0].fPosition[2]);
	gRallyInitialSplit.vPos = lastSplitpointPos;
	gRallyInitialSplit.fAbsoluteMapPos = 0;
	gRallyInitialSplit.fPercentMapPos = 0;

	for (int i = 0; i < track->nNumSplitpoints; i++) {
		auto pos = track->aSplitpoints[i].fPosition;
		auto posVec = NyaVec3(pos[0],0,pos[2]);
		tmpMapPos += (posVec - lastSplitpointPos).length();
		lastSplitpointPos = posVec;
		aRallySplitpoints[i].vPos = posVec;
		aRallySplitpoints[i].fAbsoluteMapPos = tmpMapPos;
	}
	for (int i = 0; i < track->nNumSplitpoints; i++) {
		aRallySplitpoints[i].fPercentMapPos = aRallySplitpoints[i].fAbsoluteMapPos / tmpMapPos;
	}
	for (int i = 0; i < track->nNumSplitpoints; i++) {
		// 1 pixel off so it has less chance to look wrong
		int top = 160;
		int bottom = 811;

		auto y = std::lerp(bottom, top, aRallySplitpoints[i].fPercentMapPos);
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

bool IsInTimeTrialWithSplits() {
	if (pGameFlow->nGameState != GAME_STATE_RACE) return false;
	if (pLoadingScreen || !GetPlayer(0)) return false;
	if (!bIsTimeTrial) return false;
	if (pScoreManager->nNumLaps > 3) return false;
	return true;
}

bool ShouldDrawSplits() {
	if (pGameFlow->nGameRules != GR_DEFAULT && pGameFlow->nGameRules != GR_RACE && pGameFlow->nGameRules != GR_PONGRACE) return false;
	if (pGameFlow->nDerbyType != DERBY_NONE) return false;

	switch (nDisplaySplits) {
		case 0:
		default:
			return false;
		case 1:
			return IsInRallyMode();
		case 2:
			return IsInRallyMode() || IsInTimeTrialWithSplits();
		case 3:
			return true;
	}
}

void __fastcall OnSplitpoint(Player* player, int id) {
	if (!ShouldDrawSplits()) return;
	if (pGameFlow->nRaceState != RACE_STATE_RACING) return;
	if (id >= pTrackAI->pTrack->nNumSplitpoints * pScoreManager->nNumLaps) return;

	auto time = pGameFlow->pHost->nRaceTime;
	auto speed = player->pCar->GetVelocity()->length() * 3.6;
	/*if (bIsTimeTrial) {
		int lap = player->nCurrentLap;
		if (lap > 0 && (player->nCurrentSplit % pTrackAI->pTrack->nNumSplitpoints) == 0) lap--;
		if (lap > 0) {
			id -= lap * pTrackAI->pTrack->nNumSplitpoints;
			time -= GetPlayerScore<PlayerScoreRace>(player->nPlayerId)->nLapTimes[lap];
		}
	}*/

	if (player->nPlayerId != 1) {
		if (HasLocalPlayerCrossedThisSplit(id) && !HasAnyOpponentCrossedThisSplit(id)) {
			fSplitDiffTimer = 3;
			if (fSplitTimer > 2) fSplitDiffTimer = fSplitTimer;
		}
	}
	else {
		nLastSplitTime = time;
		fLastSplitSpeed = speed;
		fSplitTimer = 3;
		fSplitDiffTimer = 3;
		nLastSplitID = id;
	}

	nSplitTimes[player->nPlayerId-1][id] = time;
	fSplitSpeeds[player->nPlayerId-1][id] = speed;
}

uintptr_t OnSplitpoint_jmp = 0x4780D8;
void __attribute__((naked)) OnSplitpointASM() {
	__asm__ (
		"pushad\n\t"
		"mov edx, ecx\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"push ecx\n"
		"mov ecx, [edi+0x2C4]\n"
		"push ecx\n"
		"mov ecx, 0x178A\n\t"
		"jmp %0\n\t"
			:
			: "m" (OnSplitpoint_jmp), "i" (OnSplitpoint)
	);
}

void ProcessRallyHUD() {
	if (!IsInRallyMode() || pGameFlow->nRaceState != RACE_STATE_RACING) return;
	if (ChloeMenuLib::IsMenuOpen()) return;
	DrawRallyHUD();
}

void ProcessSplitHUD() {
	if (!ShouldDrawSplits() || pGameFlow->nRaceState != RACE_STATE_RACING) {
		memset(nSplitTimes, 0, sizeof(nSplitTimes));
		memset(fSplitSpeeds, 0, sizeof(fSplitSpeeds));
		fSplitTimer = 0;
		fSplitDiffTimer = 0;
		nLastSplitTime = 0;
		nLastSplitID = 0;
		return;
	}
	DrawSplitHUD();
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
		note.Process(isNextInQueue, aPacenoteQueue.size() == 1 || &note != &aPacenoteQueue[0]);
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
	for (auto& note : *GetPacenoteDB()) {
		note.LoadTexture();
	}

	if (pLoadingScreen || !GetPlayer(0)) return;
	if (pGameFlow->nRaceState != RACE_STATE_RACING) {
		ClearPacenoteQueue();
		sLastPlayedPacenoteSpeech = "";
		for (auto& note : aPacenotes) {
			note.playedLap = -1;
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
			if (note.playedLap != GetPlayer(0)->nCurrentLap && note.IsInRange()) {
				note.Play();
				nLastPacenote = &note - &aPacenotes[0];
			}
		}
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
		type.format = config[std::format("pacenotespeech{}", i+1)]["format"].value_or("");
		if (type.name.empty()) continue;
		if (type.folder.empty()) continue;
		if (type.format.empty()) continue;
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

void ApplyRallyPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4780CB, &OnSplitpointASM);
}