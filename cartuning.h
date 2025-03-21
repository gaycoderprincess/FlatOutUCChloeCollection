bool bSkipTuningThisRace = false;
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
		data->fSuspensionStiffness = 0.5;
		data->initialized = true;
	}

	if (data->fNitroModifier < 0) data->fNitroModifier = 0;
	if (data->fNitroModifier > 1) data->fNitroModifier = 1;
	if (data->fBrakeBias < 0) data->fBrakeBias = 0;
	if (data->fBrakeBias > 1) data->fBrakeBias = 1;
	if (data->fEngineModifier < 0) data->fEngineModifier = 0;
	if (data->fEngineModifier > 1) data->fEngineModifier = 1;
	if (data->fSuspensionStiffness < 0) data->fSuspensionStiffness = 0;
	if (data->fSuspensionStiffness > 1) data->fSuspensionStiffness = 1;

	return data;
}
auto GetCurrentCarTuning() {
	int activeCar = pGameFlow->Profile.nActiveCar - 1;
	if (bEnableCarTuningForMultiplayer && nCarTuningCurrectCarForMultiplayer >= 0) activeCar = nCarTuningCurrectCarForMultiplayer;
	return GetCarTuning(activeCar);
}

void __fastcall DoCarTuning(Car* pCar, float* pStackHandling) {
	if (!bEnableCarTuningForMultiplayer && pGameFlow->PreRace.nMode != GM_CAREER && pGameFlow->PreRace.nMode != GM_TEST && pGameFlow->nGameRules != GR_TEST) return;
	if (!bEnableCarTuningForMultiplayer && nHandlingMode != HANDLING_PROFESSIONAL) return;
	auto player = pCar->pPlayer;
	if (player->nPlayerType != PLAYERTYPE_LOCAL) return;
	if (!bEnableCarTuningForMultiplayer && bSkipTuningThisRace) return;

	auto tuning = GetCarTuning(player->nCarId);

	auto& fNitroStorage = *(float*)0x764C48;
	auto& fNitroAcceleration = *(float*)0x764C4C;
	auto& fBrakeBalance = *(float*)0x84963C;
	auto& fFrontDefaultCompression = *(float*)0x764E84;
	auto& fFrontBumpDamp = *(float*)0x764E88;
	auto& fFrontReboundDamp = *(float*)0x764E8C;
	auto& fRearDefaultCompression = *(float*)0x764EC4;
	auto& fRearBumpDamp = *(float*)0x764EC8;
	auto& fRearReboundDamp = *(float*)0x764ECC;

	// blaster xl:
	/*
		vec2 FrontMinLength = { 0, 0 };
		vec2 FrontMaxLength = { 0.65, 0.65 };
		vec2 FrontRestLength = { 0.53, 0.5 };
		vec2 FrontDefaultCompression = { 0.13, 0.1 };
		vec2 FrontBumpDamp = { 0.6, 0.6 };
		vec2 FrontReboundDamp = { 0.8, 0.8 };
		vec2 FrontBumperLength = { 0.03, 0.03 };
		vec2 FrontBumperConst = { 0, 0 };
		vec2 FrontBumperRestitution = { 0, 0 };
		vec2 FrontRollbarStiffness = { 0.3, 0.315 };
		vec2 FrontCamberAngle = { 1, 1 };
		vec2 FrontCamberChangeUp = { 0.2, 0.2 };
		vec2 FrontCamberChangeDown = { 0.2, 0.2 };
		vec2 FrontCamberChangeIn = { 0.04, 0.04 };
		vec2 FrontCamberChangeOut = { 0.04, 0.04 };
	*/

	// chili pepper:
	/*
		vec2 FrontMinLength = { 0, 0 };
		vec2 FrontMaxLength = { 0.65, 0.65 };
		vec2 FrontRestLength = { 0.24, 0.23 };
		vec2 FrontDefaultCompression = { 0.08, 0.07 };
		vec2 FrontBumpDamp = { 0.4, 0.4 };
		vec2 FrontReboundDamp = { 0.6, 0.6 };
		vec2 FrontBumperLength = { 0.03, 0.03 };
		vec2 FrontBumperConst = { 0, 0 };
		vec2 FrontBumperRestitution = { 0, 0 };
		vec2 FrontRollbarStiffness = { 0.2, 0.25 };
		vec2 FrontCamberAngle = { 1, 1 };
		vec2 FrontCamberChangeUp = { 0.2, 0.2 };
		vec2 FrontCamberChangeDown = { 0.2, 0.2 };
		vec2 FrontCamberChangeIn = { 0.04, 0.04 };
		vec2 FrontCamberChangeOut = { 0.04, 0.04 };
	*/

	auto nitroModifier = sqrt(tuning->fNitroModifier + 0.5);
	auto suspModifier1 = sqrt(tuning->fSuspensionStiffness + 0.5);

	fFrontDefaultCompression /= suspModifier1;
	fRearDefaultCompression /= suspModifier1;
	fFrontBumpDamp /= suspModifier1;
	fFrontReboundDamp /= suspModifier1;
	fRearBumpDamp /= suspModifier1;
	fRearReboundDamp /= suspModifier1;

	fNitroStorage /= nitroModifier;
	fNitroAcceleration *= nitroModifier;
	fBrakeBalance = tuning->fBrakeBias;
}

uintptr_t CarTuningASM_jmp = 0x45E670;
void __attribute__((naked)) CarTuningASM() {
	__asm__ (
		"call %0\n\t"
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"sub ecx, 0x368\n\t"
		"xor edx, edx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (CarTuningASM_jmp), "i" (DoCarTuning)
	);
}

uintptr_t CarTuningASM2_jmp = 0x45E670;
void __attribute__((naked)) CarTuningASM2() {
	__asm__ (
		"call %0\n\t"
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"lea edx, [esp+0x20+0x20+0x4]\n\t" // account for pushad and call
		"call %1\n\t"
		"popad\n\t"
		"ret\n\t"
			:
			:  "m" (CarTuningASM2_jmp), "i" (DoCarTuning)
	);
}

void ApplyCarTuningPatches() {
	memset(&aMultiplayerCarTunings, 0, sizeof(aMultiplayerCarTunings));

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x444FA5, &CarTuningASM); // engine
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x43B62F, &CarTuningASM2); // body
}