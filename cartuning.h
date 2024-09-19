bool bEnableCarTuningForMultiplayer = false;
int nCarTuningCurrectCarForMultiplayer = -1;
tCarTuning aMultiplayerCarTunings[256];

auto GetCarTuning(int car) {
	auto data = bEnableCarTuningForMultiplayer ? &aMultiplayerCarTunings[car] : &gCustomSave.aCarTunings[car];
	if (!data->initialized) {
		auto table = GetLiteDB()->GetTable(std::format("FlatOut2.Cars.Car[{}]", car).c_str());
		float brakeBalance[2];
		table->GetTable("Body")->GetPropertyAsVector2(brakeBalance, "BrakeBalance", 0);
		data->fBrakeBias = brakeBalance[0];
		data->fNitroModifier = 0.5;
		data->fEngineModifier = 0.5;
		data->initialized = true;
	}

	if (data->fNitroModifier < 0) data->fNitroModifier = 0;
	if (data->fNitroModifier > 1) data->fNitroModifier = 1;
	if (data->fBrakeBias < 0) data->fBrakeBias = 0;
	if (data->fBrakeBias > 1) data->fBrakeBias = 1;
	if (data->fEngineModifier < 0) data->fEngineModifier = 0;
	if (data->fEngineModifier > 1) data->fEngineModifier = 1;

	return data;
}
auto GetCurrentCarTuning() {
	int activeCar = pGame->Profile.nActiveCar - 1;
	if (bEnableCarTuningForMultiplayer && nCarTuningCurrectCarForMultiplayer >= 0) activeCar = nCarTuningCurrectCarForMultiplayer;
	return GetCarTuning(activeCar);
}

void __fastcall DoCarTuning(Car* pCar, float* pStackHandling) {
	if (!bEnableCarTuningForMultiplayer && pGame->nGameMode != GM_CAREER) return;
	auto player = pCar->pPlayer;
	if (player->nPlayerType != PLAYER_TYPE_LOCAL) return;

	auto tuning = GetCarTuning(player->nCarId);

	auto& fNitroStorage = *(float*)0x764C48;
	auto& fNitroAcceleration = *(float*)0x764C4C;
	auto& fBrakeBalance = *(float*)0x84963C;
	auto& fPeakPower = *(float*)0x764C2C;
	auto& fPeakTorque = *(float*)0x764C34;

	float nitroModifier = tuning->fNitroModifier + 0.5;
	float engineModifier = tuning->fEngineModifier + 0.5;

	fPeakPower *= engineModifier;
	fPeakTorque *= engineModifier;
	if (pStackHandling) {
		auto& fSpeedLimit = pStackHandling[11];
		fSpeedLimit /= engineModifier;
	}

	fNitroStorage /= nitroModifier;
	fNitroAcceleration *= nitroModifier;
	fBrakeBalance = tuning->fBrakeBias;
}

uintptr_t CarTuningASM_jmp = 0x45E670;
void __attribute__((naked)) CarTuningASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"sub ecx, 0x368\n\t"
		"xor edx, edx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (CarTuningASM_jmp), "i" (DoCarTuning)
	);
}

uintptr_t CarTuningASM2_jmp = 0x45E670;
void __attribute__((naked)) CarTuningASM2() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"lea edx, [esp+0x20+0x20+0x4]\n\t" // account for pushad and call
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (CarTuningASM2_jmp), "i" (DoCarTuning)
	);
}

void ApplyCarTuningPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x444FA5, &CarTuningASM); // engine
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43B62F, &CarTuningASM2); // body
}