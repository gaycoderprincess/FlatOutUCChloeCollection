uintptr_t NoSlideControlASM_jmp = 0x42B4AE;
void __attribute__((naked)) __fastcall NoSlideControlASM() {
	__asm__ (
		"push ebp\n\t"
		"mov ebp, esp\n\t"
		"and esp, 0xFFFFFFF8\n\t"
		"sub esp, 0x98\n\t"
		"fldz\n\t"
		"jmp %0\n\t"
			:
			: "m" (NoSlideControlASM_jmp)
	);
}

void SetSlideControl(bool disabled) {
	if (disabled) {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42F9CE, &NoSlideControlASM);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480D2C, &NoSlideControlASM);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x51460E, &NoSlideControlASM);

		// disable slidecontrol stuff in the fouc code
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42B6B6, 0x42BCF7);
		NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42C02C, 0x42C26D);
	}
	else {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42F9CE, 0x42B4A0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480D2C, 0x42B4A0);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x51460E, 0x42B4A0);
	}
}

void SetBetaHandling(bool enabled) {
	NyaHookLib::Patch(0x45CC35 + 1, enabled ? "BetaBody" : "Body");
	NyaHookLib::Patch(0x45DA4F + 1, enabled ? "BetaEngine" : "Engine");
	//NyaHookLib::Patch(0x423CCA + 1, enabled ? "BetaEngineSound" : "EngineSound");
	NyaHookLib::Patch(0x45CF8D + 1, enabled ? "BetaGearbox" : "Gearbox");
	NyaHookLib::Patch(0x45D200 + 1, enabled ? "BetaSuspension" : "Suspension");
	NyaHookLib::Patch(0x45D7B6 + 1, enabled ? "BetaTires" : "Tires");
}

int GetCarNumWheelsOnGround(Car* car) {
	int count = 0;
	for (int i = 0; i < 4; i++) {
		if (car->aTires[i].bOnGround) count++;
	}
	return count;
}

int nMultiplayerHandlingMode = 0;
int GetHandlingMode() {
	// only allow normal and professional for career
	if (CareerTimeTrial::bIsCareerTimeTrial) {
		if (nHandlingMode == HANDLING_PROFESSIONAL) return HANDLING_PROFESSIONAL;
		return HANDLING_NORMAL;
	}
	if (bIsInMultiplayer) return nMultiplayerHandlingMode;
	return nHandlingMode;
}

void __fastcall DoFO2Downforce(Car* pCar) {
	int handlingMode = GetHandlingMode();
	if (CareerTimeTrial::bIsCareerTimeTrial) return; // no downforce in career time trials
	if (handlingMode == HANDLING_PROFESSIONAL || handlingMode == HANDLING_NORMAL_LEGACY) return; // no downforce on professional or legacy
	if (handlingMode == HANDLING_NORMAL && GetCarNumWheelsOnGround(pCar) > 0) return; // no downforce on ground on normal
	*pCar->GetVelocityGravity() += pCar->GetMatrix()->y * -pCar->GetVelocity()->LengthSqr() * pCar->fMass * 0.0011772001;
}

uintptr_t FO2SlideControlWrappedASM_jmp = 0x42AFBF;
void __attribute__((naked)) FO2DownforceASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebp\n\t"
		"call %1\n\t"
		"popad\n\t"
		"fld dword ptr [ebp+0x290]\n\t"
		"jmp %0\n\t"
			:
			: "m" (FO2SlideControlWrappedASM_jmp), "i" (DoFO2Downforce)
	);
}

void SetFO2Downforce(bool on) {
	NyaHookLib::Patch<uint64_t>(0x42B11C, on ? 0x86D990909090D8DD : 0x86D9000000F09ED9); // downforce x
	NyaHookLib::Patch<uint64_t>(0x42B132, on ? 0x44D990909090D8DD : 0x44D9000000F49ED9); // downforce y
	NyaHookLib::Patch<uint64_t>(0x42B144, on ? 0x44D990909090D8DD : 0x44D9000000F89ED9); // downforce z
	NyaHookLib::Patch<uint64_t>(0x42B18D, on ? 0x44D990909090D8DD : 0x44D9000001009ED9); // downforce rx
	NyaHookLib::Patch<uint64_t>(0x42B1B5, on ? 0xCAD990909090D8DD : 0xCAD9000001049ED9); // downforce ry
	NyaHookLib::Patch<uint64_t>(0x42B1D3, on ? 0x1DD890909090D8DD : 0x1DD8000001089ED9); // downforce rz
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42AFB9, &FO2DownforceASM);
}

void SetSlideControl() {
	static int nLastHandling = -1;
	static bool bLastCareerTimeTrial = false;

	int nCurrentHandling = GetHandlingMode();
	if (nLastHandling != nCurrentHandling || bLastCareerTimeTrial != CareerTimeTrial::bIsCareerTimeTrial) {
		SetFO2Downforce(nCurrentHandling != HANDLING_PROFESSIONAL && nCurrentHandling != HANDLING_NORMAL_LEGACY && !CareerTimeTrial::bIsCareerTimeTrial);
		SetSlideControl(nCurrentHandling == HANDLING_PROFESSIONAL);
		SetBetaHandling(nCurrentHandling == HANDLING_BETA);
		nLastHandling = nCurrentHandling;
		bLastCareerTimeTrial = CareerTimeTrial::bIsCareerTimeTrial;
	}
}