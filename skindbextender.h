int nHigherSkinIDReturn = 0;
void __fastcall CheckForHigherSkinID(int id) {
	bool useAISkins = id > 5;
	if (useAISkins) {
		id -= 5;
		if (id > 4) id = 4;
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

void ApplySkinDBExtenderPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C7819, &AISkinPropertiesForSkin6ASM);

	// read in stunt at 004693C4, same as race
	// then read at 004696DA
	NyaHookLib::Patch<uint16_t>(0x4696D8, 0x9090); // skin changing in stunt
}