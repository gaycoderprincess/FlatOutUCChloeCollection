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

LRESULT __stdcall SendDlgItemMessageAHooked(HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam) {
	if (nIDDlgItem == 1124 && Msg == 0x14E) {
		auto tmp = SendDlgItemMessageA(hDlg, 1124, 0x143u, 0, (LPARAM)"21:9");
		SendDlgItemMessageA(hDlg, 1124, 0x151u, tmp, 4);
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
	return 0; // 4:3
}

void ApplyUltrawidePatches() {
	NyaHookLib::Patch(0x45642F, &pSendDlgItemMessageAHooked);
	NyaHookLib::Patch<uint8_t>(0x45741D + 2, 4);
	NyaHookLib::Patch(0x45768C, &Aspect219ASM);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x455C80, &GetAspectRatioForSetup);
}