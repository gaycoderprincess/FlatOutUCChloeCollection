float __fastcall NoAILookahead(void* a1, uintptr_t a2) {
	auto ai = *(AIPlayer**)(a2 + 0x1C);
	ai->AIProfile.fLookAheadMin = 10.0;
	ai->AIProfile.fLookAheadMax = 10.0;
	ai->AIProfile.fLookAheadModifier = 0.1;
	return 10.0;
}

// disable ai lookahead on fo1 tracks
void SetAILookahead() {
	if (auto game = pGameFlow) {
		if (game->nGameState != GAME_STATE_RACE) return;
		bool isFO1Track = DoesTrackValueExist(game->PreRace.nLevel, "UseLowAILookahead");

		//NyaHookLib::Patch<uint64_t>(0x406CF3, isFO1Track ? 0x818B90000000DEE9 : 0x818B000000DD840F);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x406FE0, isFO1Track ? (uintptr_t)&NoAILookahead : 0x406E50);
	}
}

struct tResetpoint {
	NyaMat4x4 matrix;
	int split = -1;
};
std::vector<tResetpoint> aNewResetPoints;
void ResetCarAt(Car* car, const NyaMat4x4& pos, float speed) {
	*car->GetMatrix() = pos;
	bool isReversed = pGameFlow->PreRace.nReversed;
	if (pGameFlow->nGameRules == GR_PONGRACE && car->pPlayer->nCurrentLap % 2 == 1) {
		isReversed = !isReversed;
	}
	if (isReversed) {
		auto mat = car->GetMatrix();
		mat->x.x *= -1;
		mat->x.z *= -1;
		mat->y.x *= -1;
		mat->y.z *= -1;
		mat->z.x *= -1;
		mat->z.z *= -1;
	}
	*car->GetVelocity() = car->GetMatrix()->z * speed;
	*car->GetAngVelocity() = {0, 0, 0};
	FO2MatrixToQuat(car->mMatrix, car->qQuaternion);
}

std::string GetResetPointFilename(bool withSplits) {
	return (std::string)"Config/Resets/" + GetTrackName(pGameFlow->PreRace.nLevel) + (withSplits ? ".rst2" : ".rst");
}

void SaveResetPoints(const std::string& filename, bool withSplits) {
	std::filesystem::create_directory("Config");
	std::filesystem::create_directory("Config/Resets");

	std::ofstream fout(filename, std::ios::out | std::ios::binary );
	if (!fout.is_open()) return;

	uint32_t count = aNewResetPoints.size();
	fout.write((char*)&count, 4);
	for (int i = 0; i < count; i++) {
		fout.write((char*)&aNewResetPoints[i], withSplits ? sizeof(tResetpoint) : sizeof(NyaMat4x4));
	}
}

bool LoadResetPoints(const std::string& filename, bool withSplits) {
	aNewResetPoints.clear();

	std::ifstream fin(filename, std::ios::in | std::ios::binary );
	if (!fin.is_open()) return false;

	uint32_t count = 0;
	fin.read((char*)&count, 4);
	aNewResetPoints.reserve(count);
	for (int i = 0; i < count; i++) {
		if (fin.eof()) return true;

		tResetpoint reset;
		fin.read((char*)&reset.matrix, sizeof(reset.matrix));
		if (withSplits) fin.read((char*)&reset.split, sizeof(reset.split));
		aNewResetPoints.push_back(reset);
	}
	return true;
}

bool bVoidResetEnabled = false;
bool bOutOfMapResetEnabled = false;
float fOutOfMapResetY = 0;
bool bInvisWaterPlane = false;
float fWaterPlaneY = 0;
void SetTrackCustomProperties() {
	bOutOfMapResetEnabled = false;
	bVoidResetEnabled = false;

	bool increased = false;
	bool increasedNegY = false;
	bool increasedNegY2 = false;
	const char* textureFolder = "textures/";
	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		increased = DoesTrackValueExist(pGameFlow->PreRace.nLevel, "IncreasedVisibility");
		increasedNegY = DoesTrackValueExist(pGameFlow->PreRace.nLevel, "IncreasedNegYVisibility");
		increasedNegY2 = DoesTrackValueExist(pGameFlow->PreRace.nLevel, "IncreasedNegYVisibility2");
		if (increasedNegY2) increasedNegY = true;

		int derbyTimer = 45;
		static double waterPlaneY;
		static float camWaterPlaneY;
		waterPlaneY = 0.0;
		if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "WaterPlaneY")) {
			waterPlaneY = GetTrackValueNumber(pGameFlow->PreRace.nLevel, "WaterPlaneY");
		}
		if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "TextureFolder")) {
			textureFolder = GetTrackValueString(pGameFlow->PreRace.nLevel, "TextureFolder");
		}
		if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "OutOfMapResetY")) {
			bOutOfMapResetEnabled = true;
			fOutOfMapResetY = GetTrackValueNumber(pGameFlow->PreRace.nLevel, "OutOfMapResetY");
		}
		if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "ResetInVoid")) {
			bVoidResetEnabled = true;
		}
		if (DoesTrackValueExist(pGameFlow->PreRace.nLevel, "DerbyTimeout")) {
			derbyTimer = GetTrackValueNumber(pGameFlow->PreRace.nLevel, "DerbyTimeout");
		}
		camWaterPlaneY = waterPlaneY + 1.0;
		NyaHookLib::Patch(0x4405FE + 2, &waterPlaneY);
		NyaHookLib::Patch(0x5076EB + 2, &camWaterPlaneY);
		fWaterPlaneY = waterPlaneY;

		auto db = GetLiteDB()->GetTable("Settings.Derbies");
		*(int*)db->GetPropertyPointer("InactivityTimer") = derbyTimer;

		bInvisWaterPlane = DoesTrackValueExist(pGameFlow->PreRace.nLevel, "ForceInvisibleWaterPlane");
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

		LoadResetPoints(GetResetPointFilename(true), true);
		if (aNewResetPoints.empty()) {
			LoadResetPoints(GetResetPointFilename(false), false);
		}

		bool noMap = IsRallyTrack() && !bIsInMultiplayer;

		// just disabling the map doesn't work - that sets some render flags for shadows to work correctly
		// so disabling the markers instead, rally stages don't have minimap textures anyway
		// disable player markers for rally stages
		NyaHookLib::Patch<uint64_t>(0x4F1575, noMap ? 0x03EB9000000575E9 : 0x03EB000005748E0F);
		// disable local player marker
		NyaHookLib::Patch<uint64_t>(0x4F210A, noMap ? 0x909090000000BCE9 : 0x00014024848B168B);

		if (!bIsStuntMode) {
			NyaHookLib::Patch<uint64_t>(0x4DF94E, DoesTrackValueExist(pGameFlow->PreRace.nLevel, "NoWrongWay") ? 0x0E606800000E32E9 : 0x0E606809298FD8A1);
		}
	}

	NyaHookLib::Patch<const char*>(0x56163D, textureFolder);
	NyaHookLib::Patch<const char*>(0x56165A, textureFolder);
	NyaHookLib::Patch<const char*>(0x562318, textureFolder);
	NyaHookLib::Patch<const char*>(0x562331, textureFolder);
	NyaHookLib::Patch<const char*>(0x58C3D3, textureFolder);
	NyaHookLib::Patch<const char*>(0x58C40E, textureFolder);

	bool muteMusic = pGameFlow->nGameState == GAME_STATE_RACE && IsRallyTrack();
	NyaHookLib::Patch(0x41D348, muteMusic ? (uintptr_t)&nRallyMusicVolume : 0x849548);
	NyaHookLib::Patch(0x41DEAB, muteMusic ? (uintptr_t)&nRallyMusicVolume : 0x849548);
	NyaHookLib::Patch(0x4AF18C, muteMusic ? (uintptr_t)&nRallyMusicVolume : 0x849548);
	NyaHookLib::Patch(0x715184, muteMusic ? (uintptr_t)&nRallyMusicVolume : 0x849548);

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
	if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->PreRace.nLevel, "InvisibleWaterPlane")) {
		NyaHookLib::Patch<uint8_t>(0x4F4CAD, 0xEB);
	}
	else {
		NyaHookLib::Patch<uint8_t>(0x4F4CAD, 0x74);
	}
	if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->PreRace.nLevel, "ForceWaterPlane")) {
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
NyaMat4x4* GetClosestResetpoint(NyaVec3 pos, int split) {
	if (aNewResetPoints.empty()) return nullptr;

	float dist = 99999;
	NyaMat4x4* out = nullptr;
	for (auto& reset : aNewResetPoints) {
		if (reset.split >= 0 && reset.split != split) continue;

		auto d = (reset.matrix.p - pos).length();
		if (d < dist) {
			out = &reset.matrix;
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

NyaMat4x4* pNewResetpoint = nullptr;
float fNewResetpointSpeed = 0;
void __stdcall ResetCarNew(Car* car, int a2, float* a3, float speed) {
	// never reset ai cars during career time trials
	if ((CareerTimeTrial::bIsCareerTimeTrial || bIsCareerRally) && car->pPlayer->nPlayerId != 1) {
		return;
	}
	if (pLastPlayerResetpoint && car->pPlayer->nPlayerId == 1) {
		pNewResetpoint = pLastPlayerResetpoint;
	}
	else if (auto reset = GetClosestResetpoint(car->GetMatrix()->p, car->pPlayer->nCurrentSplit % pEnvironment->nNumSplitpoints)) {
		pNewResetpoint = reset;
	}
	fNewResetpointSpeed = speed;
	ResetCar(car, a2, a3, speed);
	pNewResetpoint = nullptr;
}

// race restart, use end pos
void __stdcall ResetCarNewRestart(Car* car, int a2, float* a3, float speed) {
	ResetCar(car, a2, a3, speed);
	if (auto reset = GetClosestResetpoint(car->GetMatrix()->p, car->pPlayer->nCurrentSplit % pEnvironment->nNumSplitpoints)) {
		ResetCarAt(car, *reset, speed);
	}
}

// grab closest resetpoint if it's within 5m, keep that
// should be enough to prevent resets ahead of your current position
void ProcessNewReset() {
	static double fTimeSinceLastGround = 0;
	static CNyaTimer timer;
	timer.Process();

	if (pLoadingScreen || pGameFlow->nGameState != GAME_STATE_RACE || pGameFlow->nRaceState == RACE_STATE_COUNTDOWN) {
		fTimeSinceLastGround = 0;
		pLastPlayerResetpoint = nullptr;
		return;
	}
	auto ply = GetPlayer(0);
	if (!ply || !ply->pCar) {
		fTimeSinceLastGround = 0;
		pLastPlayerResetpoint = nullptr;
		return;
	}

	if (bOutOfMapResetEnabled && ply->pCar->GetMatrix()->p.y < fOutOfMapResetY) {
		auto data = tEventData(EVENT_PLAYER_RESPAWN, ply->nPlayerId);
		pEventManager->SendEvent(&data);
		return;
	}

	// reset if no ground was found
	if (bVoidResetEnabled && !pEnvironment->bWaterPlane && !bInvisWaterPlane) {
		auto origin = ply->pCar->GetMatrix()->p;
		origin.y += 2;
		auto dir = NyaVec3(0,-1,0);

		tLineOfSightIn prop;
		prop.fMaxDistance = 10000;
		tLineOfSightOut out;
		if (!CheckLineOfSight(&prop, pGameFlow->pHost->pUnkForLOS, &origin, &dir, &out)) {
			fTimeSinceLastGround += timer.fDeltaTime;
			if (fTimeSinceLastGround > 0.5) {
				auto data = tEventData(EVENT_PLAYER_RESPAWN, ply->nPlayerId);
				pEventManager->SendEvent(&data);
				fTimeSinceLastGround = 0;
				return;
			}
		}
		else {
			fTimeSinceLastGround = 0;
		}
	}

	if (ply->nTimeInAir > 100) return;

	auto closest = GetClosestResetpoint(ply->pCar->GetMatrix()->p, ply->nCurrentSplit % pEnvironment->nNumSplitpoints);
	if (!closest) return;

	if ((closest->p - ply->pCar->GetMatrix()->p).length() <= 5) {
		pLastPlayerResetpoint = closest;
	}
}

void __fastcall OnCarReset(Car* pCar) {
	if (!pNewResetpoint) return;
	ResetCarAt(pCar, *pNewResetpoint, fNewResetpointSpeed);
}

uintptr_t OnCarResetASM_jmp = 0x655590;
void __attribute__((naked)) __fastcall OnCarResetASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (OnCarResetASM_jmp), "i" (OnCarReset)
	);
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
	OnCarResetASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42F109, &OnCarResetASM);
}