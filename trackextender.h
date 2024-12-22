float __fastcall NoAILookahead(void* a1, uintptr_t a2) {
	auto ai = *(Player**)(a2 + 0x1C);
	ai->fLookAheadMin = 10.0;
	ai->fLookAheadMax = 10.0;
	ai->fLookAheadModifier = 0.1;
	return 10.0;
}

// disable ai lookahead on fo1 tracks
void SetAILookahead() {
	if (auto game = pGameFlow) {
		if (game->nGameState != GAME_STATE_RACE) return;
		bool isFO1Track = DoesTrackValueExist(game->nLevelId, "UseLowAILookahead");

		//NyaHookLib::Patch<uint64_t>(0x406CF3, isFO1Track ? 0x818B90000000DEE9 : 0x818B000000DD840F);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x406FE0, isFO1Track ? (uintptr_t)&NoAILookahead : 0x406E50);
	}
}

std::vector<NyaMat4x4> aNewResetPoints;
void ResetCarAt(Car* car, const NyaMat4x4& pos, float speed) {
	*car->GetMatrix() = pos;
	*car->GetVelocity() = car->GetMatrix()->z * speed;
	*car->GetAngVelocity() = {0, 0, 0};
	FO2MatrixToQuat(car->mMatrix, car->qQuaternion);
}

std::string GetResetPointFilename() {
	return (std::string)"Config/Resets/" + GetTrackName(pGameFlow->nLevelId) + ".rst";
}

void SaveResetPoints(const std::string& filename) {
	std::filesystem::create_directory("Config");
	std::filesystem::create_directory("Config/Resets");

	std::ofstream fout(filename, std::ios::out | std::ios::binary );
	if (!fout.is_open()) return;

	uint32_t count = aNewResetPoints.size();
	fout.write((char*)&count, 4);
	for (int i = 0; i < count; i++) {
		fout.write((char*)&aNewResetPoints[i], sizeof(NyaMat4x4));
	}
}

bool LoadResetPoints(const std::string& filename) {
	aNewResetPoints.clear();

	std::ifstream fin(filename, std::ios::in | std::ios::binary );
	if (!fin.is_open()) return false;

	uint32_t count = 0;
	fin.read((char*)&count, 4);
	aNewResetPoints.reserve(count);
	for (int i = 0; i < count; i++) {
		if (fin.eof()) return true;

		NyaMat4x4 v;
		fin.read((char*)&v, sizeof(v));
		aNewResetPoints.push_back(v);
	}
	return true;
}

bool bInvisWaterPlane = false;
float fWaterPlaneY = 0;
void SetTrackCustomProperties() {
	bool increased = false;
	bool increasedNegY = false;
	bool increasedNegY2 = false;
	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		increased = DoesTrackValueExist(pGameFlow->nLevelId, "IncreasedVisibility");
		increasedNegY = DoesTrackValueExist(pGameFlow->nLevelId, "IncreasedNegYVisibility");
		increasedNegY2 = DoesTrackValueExist(pGameFlow->nLevelId, "IncreasedNegYVisibility2");
		if (increasedNegY2) increasedNegY = true;

		static double waterPlaneY;
		static float camWaterPlaneY;
		waterPlaneY = 0.0;
		if (DoesTrackValueExist(pGameFlow->nLevelId, "WaterPlaneY")) {
			waterPlaneY = GetTrackValueNumber(pGameFlow->nLevelId, "WaterPlaneY");
		}
		camWaterPlaneY = waterPlaneY + 1.0;
		NyaHookLib::Patch(0x4405FE + 2, &waterPlaneY);
		NyaHookLib::Patch(0x5076EB + 2, &camWaterPlaneY);
		fWaterPlaneY = waterPlaneY;

		bInvisWaterPlane = DoesTrackValueExist(pGameFlow->nLevelId, "ForceInvisibleWaterPlane");
		NyaHookLib::Patch<uint64_t>(0x44056E, bInvisWaterPlane ? 0x8D8D909090909090 : 0x8D8D000000CA840F); // collision
		NyaHookLib::Patch<uint64_t>(0x562B2D, bInvisWaterPlane && waterPlaneY == 0.0 ? 0x3D80909090909090 : 0x3D80000002D3840F); // ragdoll collision
		NyaHookLib::Patch<uint16_t>(0x5076E9, bInvisWaterPlane ? 0x9090 : 0x1974); // camera
		NyaHookLib::Patch<uint16_t>(0x414DB9, bInvisWaterPlane ? 0x9090 : 0x7F74); // sound

		bool disableReplays = increased || increasedNegY;
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AD957, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4ADAA3, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4ADB02, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE0B0, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE0EC, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE158, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE1BE, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE229, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE279, disableReplays ? 0x4AB1E2 : 0x4AB1B0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4AE33A, disableReplays ? 0x4AB1E2 : 0x4AB1B0);

		if (!LoadPacenotes(GetPacenoteFilenameWithVoice())) {
			LoadPacenotes(GetPacenoteFilename());
		}

		LoadResetPoints(GetResetPointFilename());

		bool noMap = !aPacenotes.empty() && !bIsInMultiplayer;

		// just disabling the map doesn't work - that sets some render flags for shadows to work correctly
		// so disabling the markers instead, rally stages don't have minimap textures anyway
		// disable player markers for rally stages
		NyaHookLib::Patch<uint64_t>(0x4F1575, noMap ? 0x03EB9000000575E9 : 0x03EB000005748E0F);
		// disable local player marker
		NyaHookLib::Patch<uint64_t>(0x4F210A, noMap ? 0x909090000000BCE9 : 0x00014024848B168B);
	}

	bool muteMusic = pGameFlow->nGameState == GAME_STATE_RACE && !aPacenotes.empty() && nMuteMusicInRally;

	static uint32_t tmpMusicVolume = 0;
	NyaHookLib::Patch(0x41D348, muteMusic ? (uintptr_t)&tmpMusicVolume : 0x849548);
	NyaHookLib::Patch(0x41DEAB, muteMusic ? (uintptr_t)&tmpMusicVolume : 0x849548);
	NyaHookLib::Patch(0x4AF18C, muteMusic ? (uintptr_t)&tmpMusicVolume : 0x849548);

	// increase VisibilitySet grid extents for rally trophy tracks
	// rally russia extends to about 4600 for reference
	static float fNegExtentsExtended = -5000.0;
	static float fPosExtentsExtended = 5000.0;
	static float fNegExtents = -4096.0;
	static float fPosExtents = 4096.0;
	static float fNegYExtentsExtended2 = -400.0;
	static float fNegYExtentsExtended = -100.0;
	static float fNegYExtents = -50.0;
	NyaHookLib::Patch(0x57AD5F + 2, increased ? &fNegExtentsExtended : &fNegExtents);
	NyaHookLib::Patch(0x57AD8E + 2, increased ? &fPosExtentsExtended : &fPosExtents);
	NyaHookLib::Patch(0x57AD6A + 2, increasedNegY2 ? &fNegYExtentsExtended2 : (increasedNegY ? &fNegYExtentsExtended : &fNegYExtents));
}

uintptr_t InitTrackASM_jmp = 0x55AAB9;
void __attribute__((naked)) __fastcall InitTrackASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (InitTrackASM_jmp), "i" (SetTrackCustomProperties)
	);
}

/*uint32_t __fastcall ForceWaterPlane(uint32_t origValue) {
	if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->nLevelId, "InvisibleWaterPlane")) {
		NyaHookLib::Patch<uint8_t>(0x4F4CAD, 0xEB);
	}
	else {
		NyaHookLib::Patch<uint8_t>(0x4F4CAD, 0x74);
	}
	if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->nLevelId, "ForceWaterPlane")) {
		return 1;
	}
	return origValue;
}

uintptr_t ForceWaterPlaneASM_jmp = 0x55EF3F;
void __attribute__((naked)) __fastcall ForceWaterPlaneASM() {
	__asm__ (
		// push all but eax
		"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push ebp\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"

		"mov ecx, [ebp+0x18]\n"
		"mov edx, [ebp+0x14]\n"
		"push ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (ForceWaterPlaneASM_jmp), "i" (ForceWaterPlane)
	);
}*/

uintptr_t WaterPlaneSoundYASM_jmp = 0x414DB9;
void __attribute__((naked)) __fastcall WaterPlaneSoundYASM() {
	__asm__ (
		"fld %1\n\t"
		"cmp byte ptr [esi+0x2DC], 0\n\t"
		"jmp %0\n\t"
			:
			: "m" (WaterPlaneSoundYASM_jmp), "m" (fWaterPlaneY)
	);
}

static inline auto ResetCar = (void(__stdcall*)(Car*, int, float*, float))0x42EEF0;

NyaMat4x4* pLastPlayerResetpoint = nullptr;
NyaMat4x4* GetClosestResetpoint(NyaVec3 pos) {
	if (aNewResetPoints.empty()) return nullptr;

	float dist = 99999;
	NyaMat4x4* out = nullptr;
	for (auto& reset : aNewResetPoints) {
		auto d = (reset.p - pos).length();
		if (d < dist) {
			out = &reset;
			dist = d;
		}
	}
	/*auto track = pTrackAI->pTrack;
	for (int i = 0; i < track->nNumStartpoints; i++) {
		auto start = track->aStartpoints[i];
		auto startPos = NyaVec3(start.fPosition[0], start.fPosition[1], start.fPosition[2]);
		auto d = (startPos - pos).length();
		if (d < dist) {
			out = (NyaMat4x4*)start.fMatrix;
			dist = d;
		}
	}*/
	return out;
}

void __stdcall ResetCarNew(Car* car, int a2, float* a3, float speed) {
	auto pos = car->GetMatrix()->p;
	ResetCar(car, a2, a3, speed);
	if (pLastPlayerResetpoint && car == GetPlayer(0)->pCar) {
		ResetCarAt(car, *pLastPlayerResetpoint, speed);
	}
	else if (auto reset = GetClosestResetpoint(pos)) {
		ResetCarAt(car, *reset, speed);
	}
}

// race restart, use end pos
void __stdcall ResetCarNewRestart(Car* car, int a2, float* a3, float speed) {
	ResetCar(car, a2, a3, speed);
	if (auto reset = GetClosestResetpoint(car->GetMatrix()->p)) {
		ResetCarAt(car, *reset, speed);
	}
}

// grab closest resetpoint if it's within 5m, keep that
// should be enough to prevent resets ahead of your current position
void ProcessNewReset() {
	if (pLoadingScreen || pGameFlow->nGameState != GAME_STATE_RACE || pGameFlow->nRaceState == RACE_STATE_COUNTDOWN) {
		pLastPlayerResetpoint = nullptr;
		return;
	}
	auto ply = GetPlayer(0);
	if (!ply || !ply->pCar) {
		pLastPlayerResetpoint = nullptr;
		return;
	}

	if (ply->nTimeInAir > 100) return;

	auto closest = GetClosestResetpoint(ply->pCar->GetMatrix()->p);
	if (!closest) return;

	if ((closest->p - ply->pCar->GetMatrix()->p).length() <= 5) {
		pLastPlayerResetpoint = closest;
	}
}

void ApplyTrackExtenderPatches() {
	InitTrackASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55E775, &InitTrackASM);
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55EF38, &ForceWaterPlaneASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x414DB0, &WaterPlaneSoundYASM);

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x40AD7E, &ResetCarNew);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x40ADD9, &ResetCarNew);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47BFAF, &ResetCarNew);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47C3DE, &ResetCarNew);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47C4A0, &ResetCarNew);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480559, &ResetCarNewRestart);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x5147C3, &ResetCarNew);
}