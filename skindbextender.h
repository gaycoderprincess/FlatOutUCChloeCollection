int nHigherSkinIDReturn = 0;
void __fastcall CheckForHigherSkinID(int id) {
	bool useAISkins = id > 5;
	if (useAISkins) {
		id -= 5;
		if (id > 3) id = 3;
	}
	nHigherSkinIDReturn = id;

	static const char* str1 = "Skin%dPaintBrightness";
	static const char* str2 = "Skin%dPaintBlend";
	static const char* str3 = "Skin%dPaintColor0";
	static const char* str4 = "Skin%dMidColor";
	static const char* str5 = "Skin%dPaintColor2";
	static const char* str6 = "Skin%dFlakeColor";
	static const char* aiStr1 = "AiSkin%dPaintBrightness";
	static const char* aiStr2 = "AiSkin%dPaintBlend";
	static const char* aiStr3 = "AiSkin%dPaintColor0";
	static const char* aiStr4 = "AiSkin%dMidColor";
	static const char* aiStr5 = "AiSkin%dPaintColor2";
	static const char* aiStr6 = "AiSkin%dFlakeColor";

	NyaHookLib::Patch(0x4C7827 + 1, useAISkins ? aiStr1 : str1);
	NyaHookLib::Patch(0x4C783A + 1, useAISkins ? aiStr2 : str2);
	NyaHookLib::Patch(0x4C784D + 1, useAISkins ? aiStr3 : str3);
	NyaHookLib::Patch(0x4C7860 + 1, useAISkins ? aiStr4 : str4);
	NyaHookLib::Patch(0x4C7873 + 1, useAISkins ? aiStr5 : str5);
	NyaHookLib::Patch(0x4C7886 + 1, useAISkins ? aiStr6 : str6);
}

uintptr_t AISkinPropertiesForSkin6ASM_jmp = 0x4C7827;
void __attribute__((naked)) AISkinPropertiesForSkin6ASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebx\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov ebx, %2\n\t"
		"fstp dword ptr [edi+0xB5E8]\n\t"
		"push ebx\n\t"
		"lea eax, [esp+0x624]\n\t"
		"jmp %0\n\t"
			:
			:  "m" (AISkinPropertiesForSkin6ASM_jmp), "i" (CheckForHigherSkinID), "m" (nHigherSkinIDReturn)
	);
}

uintptr_t CapDBSkinIDASM_jmp = 0x433EB1;
void __attribute__((naked)) __fastcall CapDBSkinIDASM() {
	__asm__ (
		// 0 skin1
		// 1 skin2
		// 2 skin3
		// 3 skin4
		// 4 skin5
		// 5 skinai1
		// 6 skinai2
		// 7 skinai3

		"mov eax, [esp+0x14]\n\t"
		"cmp eax, 7\n\t"
		"jle noCap\n\t"
		"mov eax, 7\n\t"
		"mov [esp+0x14], eax\n\t"

		"noCap:\n\t"
		"mov eax, [esp+0x18]\n\t"
		"test eax, eax\n\t"
		"jmp %0\n\t"
			:
			: "m" (CapDBSkinIDASM_jmp)
	);
}

LiteDb* pSkinDBVisualsNode = nullptr;
void __fastcall GetSkinDBVisuals(LiteDb* node) {
	pSkinDBVisualsNode = node->GetTable("Visuals");
}

// load visuals from Car->Visuals instead of %sVisuals
uintptr_t SkinDBVisualsASM_jmp = 0x4C768D;
void __attribute__((naked)) __fastcall SkinDBVisualsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov eax, %2\n\t"
		"jmp %0\n\t"
			:
			: "m" (SkinDBVisualsASM_jmp), "i" (GetSkinDBVisuals), "m" (pSkinDBVisualsNode)
	);
}

void ApplySkinDBExtenderPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C7819, &AISkinPropertiesForSkin6ASM);

	// read in stunt at 004693C4, same as race
	// then read at 004696DA
	NyaHookLib::Patch<uint16_t>(0x4696D8, 0x9090); // skin changing in stunt
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x433EAB, &CapDBSkinIDASM);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C7644, &SkinDBVisualsASM);
}