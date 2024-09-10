float f219_21 = 21.0;
float f219_9 = 9.0;
uintptr_t Aspect219ASM_jmp = 0x45747F;
void __attribute__((naked)) __fastcall Aspect219ASM() {
	__asm__ (
		"fstp st(1)\n\t"
		"fstp st\n\t"
		"fld dword ptr %1\n\t"
		"fstp dword ptr [esp+0x10]\n\t"
		"fld dword ptr %2\n\t"
		"fstp dword ptr [esp+0x14]\n\t"
		"jmp %0\n\t"
			:
			: "m" (Aspect219ASM_jmp), "m" (f219_21), "m" (f219_9)
	);
}

float f329_21 = 32.0;
float f329_9 = 9.0;
uintptr_t Aspect329ASM_jmp = 0x45747F;
void __attribute__((naked)) __fastcall Aspect329ASM() {
	__asm__ (
		"fstp st(1)\n\t"
		"fstp st\n\t"
		"fld dword ptr %1\n\t"
		"fstp dword ptr [esp+0x10]\n\t"
		"fld dword ptr %2\n\t"
		"fstp dword ptr [esp+0x14]\n\t"
		"jmp %0\n\t"
			:
			: "m" (Aspect329ASM_jmp), "m" (f329_21), "m" (f329_9)
	);
}

LRESULT __stdcall SendDlgItemMessageAHooked(HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (nIDDlgItem == 1124 && Msg == 0x14E) {
		auto tmp = SendDlgItemMessageA(hDlg, 1124, 0x143u, 0, (LPARAM)"21:9");
		SendDlgItemMessageA(hDlg, 1124, 0x151u, tmp, 4);
		tmp = SendDlgItemMessageA(hDlg, 1124, 0x143u, 0, (LPARAM)"32:9");
		SendDlgItemMessageA(hDlg, 1124, 0x151u, tmp, 5);
	}
	return SendDlgItemMessageA(hDlg, nIDDlgItem, Msg, wParam, lParam);
}
auto pSendDlgItemMessageAHooked = &SendDlgItemMessageAHooked;

int GetAspectRatioForSetup(int x, int y) {
	auto aspect = (double)x / (double)y;
	if (std::abs(aspect - (16.0 / 10.0)) < 0.05) {
		return 1;
	}
	if (std::abs(aspect - (16.0 / 9.0)) < 0.05) {
		return 2;
	}
	if (std::abs(aspect - (5.0 / 4.0)) < 0.05) {
		return 3;
	}
	if (std::abs(aspect - (21.0 / 9.0)) < 0.05) {
		return 4;
	}
	if (std::abs(aspect - (32.0 / 9.0)) < 0.05) {
		return 5;
	}
	return 0; // 4:3
}

double fAspectRatio = 16.0 / 9.0;
double fSpacingFixAmount43 = (4.0 / 3.0) / fAspectRatio;
double f43AspectCorrection = 480 * fAspectRatio; // 853
double f43AspectCorrectionCenter = f43AspectCorrection * 0.5; // 426
double fButtonPromptSpacing = 16 * fSpacingFixAmount43;
float fLUAAspect = 2.25;
void RecalculateAspectRatio() {
	fAspectRatio = *(float*)0x724BB4 / *(float*)0x724BB8;
	fSpacingFixAmount43 = (4.0 / 3.0) / fAspectRatio;
	f43AspectCorrection = 480 * fAspectRatio;
	f43AspectCorrectionCenter = f43AspectCorrection * 0.5;
	fButtonPromptSpacing = 16 * fSpacingFixAmount43;
	if (nWidescreenMenu) fLUAAspect = ((*(float*)0x71098C) * 640.0) / f43AspectCorrection;
	else fLUAAspect = *(float*)0x71098C;
}

double fOrigAspect = 16.0 / 9.0;
void __fastcall UltrawideFOV(Camera* pCam) {
	RecalculateAspectRatio();

	auto mult = 1.0 / fOrigAspect;
	auto v12 = tan(pCam->fFOV * 0.5) * pCam->fNearZ;
	pCam->fRight = v12 * fAspectRatio * mult;
	pCam->fLeft = -v12 * fAspectRatio * mult;
	pCam->fBottom = v12 * mult;
	pCam->fTop = -v12 * mult;
}

uintptr_t UltrawideFOVASM_jmp = 0x4F44FC;
void __attribute__((naked)) __fastcall UltrawideFOVASM() {
	__asm__ (
		"fstp dword ptr [esi+0x104]\n\t"
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"sub dword ptr [esp+0x1C], 1\n\t"
		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVASM_jmp), "i" (UltrawideFOV)
	);
}

class CameraExtents {
public:
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;
};
void __fastcall UltrawideFOVSky(CameraExtents* pCameraExtents, Camera* pCamera) {
	auto mult = 1.0 / fOrigAspect;
	auto v12 = tan(pCamera->fFOV * 0.5);
	pCameraExtents->fRight = v12 * fAspectRatio * mult;
	pCameraExtents->fLeft = -v12 * fAspectRatio * mult;
	pCameraExtents->fBottom = v12 * mult;
	pCameraExtents->fTop = -v12 * mult;
}

uintptr_t UltrawideFOVSkyASM_jmp = 0x591FAD;
void __attribute__((naked)) __fastcall UltrawideFOVSkyASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"mov edx, ebx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov eax, [edi]\n"
		"mov edx, [eax+0xD0]\n\t"
		"jmp %0\n\t"
			:
			: "m" (UltrawideFOVSkyASM_jmp), "i" (UltrawideFOVSky)
	);
}

float* fTextScale = nullptr;
void UltrawideTextScale() {
	*fTextScale *= 640.0;
	*fTextScale /= 480 * 1.777777;
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

uintptr_t UltrawideJumpTable[] = {
	0x457429,
	0x457435,
	0x45744F,
	0x457469,
	(uintptr_t)&Aspect219ASM,
	(uintptr_t)&Aspect329ASM,
};

void ApplyUltrawidePatches() {
	NyaHookLib::Patch(0x45642F, &pSendDlgItemMessageAHooked);
	NyaHookLib::Patch<uint8_t>(0x45741D + 2, 5);
	NyaHookLib::Patch(0x457422 + 3, &UltrawideJumpTable);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x455C80, &GetAspectRatioForSetup);

	NyaHookLib::Fill(0x4F44A1, 0x90, 0x4F44A6 - 0x4F44A1); // remove sub instruction
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4F44F6, &UltrawideFOVASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x591FA5, &UltrawideFOVSkyASM);

	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5A99C8, &UltrawideTextScaleOldASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4CAF6E, &UltrawideTextScaleASM);

	NyaHookLib::Patch(0x4CF39B, &f43AspectCorrection); // menu button prompts
	NyaHookLib::Patch(0x5F08C7, &f43AspectCorrection); // menu selected option text
	NyaHookLib::Patch(0x5F0B4A, &f43AspectCorrectionCenter); // menu selected option text centered
	//NyaHookLib::Patch(0x4CAF22, &f43AspectCorrection); // menu button prompt size
	NyaHookLib::Patch(0x4CB242 + 2, &fButtonPromptSpacing); // prompt spacing
	//NyaHookLib::Patch(0x4CB511 + 2, &fButtonPromptSpacing); // prompt right side clearance
	NyaHookLib::Patch(0x4CB428 + 2, &f43AspectCorrection); // prompt spacing
	//NyaHookLib::Patch(0x5E9B04 + 2, &f43AspectCorrection); // lua scaling

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5EC90D, &LUAResizerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5F8532, &LUATextResizerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5ED0AF, &LUASliderResizerASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5ED2F4, &LUAUpgradeSliderResizerASM);

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

	std::ifstream t("Config/WindowFunctions.bed", std::ios::in | std::ios::binary);
	if (t.is_open()) {
		std::stringstream buffer;
		buffer << t.rdbuf();

		int len = buffer.str().length()+1;
		static char* windowFunctions = new char[len];
		strcpy_s(windowFunctions, len, buffer.str().c_str());
		windowFunctions[len]=0;
		NyaHookLib::Patch(0x5E96AA, windowFunctions);
	}
}