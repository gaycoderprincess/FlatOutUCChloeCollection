double fAspectRatio = 16.0 / 9.0;
double fSpacingFixAmount43 = (4.0 / 3.0) / fAspectRatio;
double f43AspectCorrection = 480 * fAspectRatio; // 853
double f43AspectCorrectionCenter = f43AspectCorrection * 0.5; // 426
double fButtonPromptSpacing = 16 * fSpacingFixAmount43;
float fLUAAspect = 2.25;
void RecalculateAspectRatio() {
	fAspectRatio = *(float*)0x724BB4 / *(float*)0x724BB8;
	if (nWidescreenMenu) {
		f43AspectCorrection = 480 * fAspectRatio;
		fSpacingFixAmount43 = 1;
	}
	else {
		f43AspectCorrection = 640;
		fSpacingFixAmount43 = (4.0 / 3.0) / fAspectRatio;
	}
	f43AspectCorrectionCenter = f43AspectCorrection * 0.5;
	fButtonPromptSpacing = 16 * fSpacingFixAmount43;
	fLUAAspect = ((*(float*)0x71098C) * 640.0) / f43AspectCorrection;
}

float* fTextScale = nullptr;
void UltrawideTextScale() {
	if (!nWidescreenMenu) return;

	*fTextScale *= 640.0;
	*fTextScale /= f43AspectCorrection;
}

uintptr_t UltrawideTextScaleASM_jmp = 0x4CAF7B;
void __attribute__((naked)) __fastcall UltrawideTextScaleASM() {
	__asm__ (
		"fstp dword ptr [esp]\n\t"
		"mov %2, esp\n\t"

		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"

		"mov [esp+0x5C], ebp\n\t"
		"mov dword ptr [eax], 0xFFFFFFFF\n\t"
		"jmp %0\n\t"
			:
			: "m" (UltrawideTextScaleASM_jmp), "i" (UltrawideTextScale), "m" (fTextScale)
	);
}

class GUIRectangle {
public:
	uint8_t _0[0x4];
	float fPosX;
	float fPosY;
	float fSizeX;
	float fSizeY;
};

void __fastcall LUAResizer(GUIRectangle* pRect) {
	if (!nWidescreenMenu) return;

	// keeps position, works fine but makes lua changes really difficult
	//pRect->fPosX += pRect->fSizeX * 0.5;
	//pRect->fPosX /= 640.0;
	//pRect->fPosX *= f43AspectCorrection;
	//pRect->fPosX -= pRect->fSizeX * 0.5;

	// hack for listboxes
	if (pRect->fPosX == 0.0 && pRect->fSizeX > 640.0) return;

	pRect->fPosX -= 320.0;
	pRect->fPosX += f43AspectCorrectionCenter;
}

void __fastcall LUASliderResizer(GUIRectangle* pRect) {
	if (!nWidescreenMenu) return;

	static float pLastX = 0;
	if (pRect->fPosX == pLastX) return;

	pRect->fPosX -= 320.0;
	pRect->fPosX += f43AspectCorrectionCenter;

	pLastX = pRect->fPosX;
}

uintptr_t LUAResizerASM_jmp = 0x5EC912;
void __attribute__((naked)) __fastcall LUAResizerASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"test edx, edx\n\t"
		"fld dword ptr [esi+4]\n\t"
		"jmp %0\n\t"
			:
			: "m" (LUAResizerASM_jmp), "i" (LUAResizer)
	);
}

void __fastcall LUATextResizer(float* pText) {
	RecalculateAspectRatio();

	if (!nWidescreenMenu) return;

	pText[0] -= 320.0;
	pText[0] += f43AspectCorrectionCenter;
}

uintptr_t LUATextResizerASM_jmp = 0x5F8544;
void __attribute__((naked)) __fastcall LUATextResizerASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"

		"fld dword ptr [eax]\n"
		"fstp dword ptr [esp+0x20]\n"
		"lea ecx, [esp+0x14]\n\t"
		"fld dword ptr [eax+4]\n\t"
		"mov eax, [ebx]\n\t"
		"mov edx, [eax+0x14]\n\t"
		"jmp %0\n\t"
			:
			: "m" (LUATextResizerASM_jmp), "i" (LUATextResizer)
	);
}

uintptr_t LUASliderResizerASM_jmp = 0x5ED0B6;
void __attribute__((naked)) __fastcall LUASliderResizerASM() {
	__asm__ (
		"fstp dword ptr [esp+0x1C]\n\t"

		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"

		"fld dword ptr [esi+4]\n\t"
		"jmp %0\n\t"
			:
			: "m" (LUASliderResizerASM_jmp), "i" (LUASliderResizer)
	);
}

float* pUpgradePos1 = nullptr;
float* pUpgradePos2 = nullptr;
float* pUpgradePos3 = nullptr;
float* pUpgradePos4 = nullptr;
float fUpgradePosNew1[2];
float fUpgradePosNew2[2];
float fUpgradePosNew3[2];
float fUpgradePosNew4[2];
void LUAUpgradeSliderResizer() {
	if (!nWidescreenMenu) return;

	memcpy(fUpgradePosNew1, pUpgradePos1, sizeof(fUpgradePosNew1));
	memcpy(fUpgradePosNew2, pUpgradePos2, sizeof(fUpgradePosNew2));
	memcpy(fUpgradePosNew3, pUpgradePos3, sizeof(fUpgradePosNew3));
	memcpy(fUpgradePosNew4, pUpgradePos4, sizeof(fUpgradePosNew4));

	fUpgradePosNew1[0] -= 320.0;
	fUpgradePosNew1[0] += f43AspectCorrectionCenter;
	fUpgradePosNew2[0] -= 320.0;
	fUpgradePosNew2[0] += f43AspectCorrectionCenter;
	fUpgradePosNew3[0] -= 320.0;
	fUpgradePosNew3[0] += f43AspectCorrectionCenter;
	fUpgradePosNew4[0] -= 320.0;
	fUpgradePosNew4[0] += f43AspectCorrectionCenter;

	pUpgradePos1 = fUpgradePosNew1;
	pUpgradePos2 = fUpgradePosNew2;
	pUpgradePos3 = fUpgradePosNew3;
	pUpgradePos4 = fUpgradePosNew4;
}

uintptr_t LUAUpgradeSliderResizerASM_jmp = 0x5ED2FA;
void __attribute__((naked)) __fastcall LUAUpgradeSliderResizerASM() {
	__asm__ (
		"mov ebx, [esp+0x74]\n\t"
		"mov %2, ebx\n\t"
		"mov ebx, [esp+0x78]\n\t"
		"mov %3, ebx\n\t"
		"mov ebx, [esp+0x7C]\n\t"
		"mov %4, ebx\n\t"
		"mov ebx, [esp+0x80]\n\t"
		"mov %5, ebx\n\t"

		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"

		"mov ebx, %2\n\t"
		"mov [esp+0x74], ebx\n\t"
		"mov ebx, %3\n\t"
		"mov [esp+0x78], ebx\n\t"
		"mov ebx, %4\n\t"
		"mov [esp+0x7C], ebx\n\t"
		"mov ebx, %5\n\t"
		"mov [esp+0x80], ebx\n\t"

		"mov ebx, [esp+0x74]\n\t"
		"fld dword ptr [ebx]\n\t"

		"jmp %0\n\t"
			:
			: "m" (LUAUpgradeSliderResizerASM_jmp), "i" (LUAUpgradeSliderResizer), "m" (pUpgradePos1), "m" (pUpgradePos2), "m" (pUpgradePos3), "m" (pUpgradePos4)
	);
}

double GetSafeAspect() {
	switch (nWidescreenSafeZone) {
		case 0:
			return 4.0 / 3.0;
		case 1:
			return 16.0 / 9.0;
		case 2:
			return 21.0 / 9.0;
		default:
			return INFINITY;
	}
}

void ApplyUltrawidePatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4CAF6E, &UltrawideTextScaleASM);

	NyaHookLib::Patch(0x4CF39B, &f43AspectCorrection); // menu button prompts
	NyaHookLib::Patch(0x5F08C7, &f43AspectCorrection); // menu selected option text
	NyaHookLib::Patch(0x5F0B4A, &f43AspectCorrectionCenter); // menu selected option text centered
	NyaHookLib::Patch(0x4CB242 + 2, &fButtonPromptSpacing); // prompt spacing
	NyaHookLib::Patch(0x4CB428 + 2, &f43AspectCorrection); // prompt spacing

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5EC90D, &LUAResizerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5F8532, &LUATextResizerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5ED0AF, &LUASliderResizerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5ED2F4, &LUAUpgradeSliderResizerASM);

	// always draw lua sprites even if "offscreen"
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5E8CC0, 0x5E8CFF);

	uintptr_t aLUAAspectRefs[] = {
			0x5E2E0D,
			0x5E2E50,
			0x5E2FD0,
			0x5E31A1,
			0x5E325D,
			0x5E3917,
			0x5E3AFC,
			//0x5E901B, // write
			//0x5E9B0C, // write
			0x5EC661,
			0x5EC6D6,
			0x5EC914, // menu options
			0x5ECA09,
			0x5ECC1F,
			0x5ECD0B,
			0x5ECE7D,
			0x5ECED3,
			0x5ED0B8,
			0x5ED10E,
			0x5ED1CE,
			0x5ED261,
			0x5ED4B0,
			0x5ED530,
			0x5ED5A8,
			0x5ED626,
			0x5EDA96,
			0x5EDF34,
			//0x5F0CD3, // centered selected option text
			0x5F443C,
			0x5F855C,
			0x5F865D,
			0x5FECB8,
			0x6009BC,
			0x60121F,
			0x601290,
			0x601379,
			0x601466,
			0x6015BC,
	};
	for (auto& addr : aLUAAspectRefs) {
		NyaHookLib::Patch(addr, &fLUAAspect);
	}

	std::ifstream t("Config/WindowFunctions.bed");
	if (t.is_open()) {
		std::stringstream buffer;
		buffer << t.rdbuf();

		int len = buffer.str().length()+1;
		static char* windowFunctions = new char[len];
		strcpy_s(windowFunctions, len, buffer.str().c_str());
		windowFunctions[len-1]=0;
		NyaHookLib::Patch(0x5E96AA, windowFunctions);
		NyaHookLib::Patch(0x5E96A1 + 1, len-1);
	}
}