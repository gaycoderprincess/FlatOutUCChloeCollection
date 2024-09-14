// 0 - male 1
// 1 - female 1
// 2 - male 2
// 3 - male 3
// 4 - male 4
// 5 - male 5
// 6 - male 6
// 7 - male 7
// 8 - male 8
// 9 - female 2
// 10 - female 3
// 11 - female 4
// 12 - female 5
// 13 - female 6
int GetPlayerModelSkinID() {
	if (nPlayerModel < 2) return 1;
	if (nPlayerModel < 9) return nPlayerModel; // - 2 + 2 players
	return nPlayerModel - 7; // - 8 + 2 players
}
int GetPlayerModelType() {
	if (nPlayerModel < 2) return nPlayerModel;
	if (nPlayerModel < 9) return 0;
	return 1;
}
void SetPlayerModel() {
	nPlayerModelSkinID = GetPlayerModelSkinID();
	*(uint32_t*)0x8494D0 = GetPlayerModelType();
}

uintptr_t PlayerModelASM_jmp = 0x4696C8;
void __attribute__((naked)) PlayerModelASM() {
	__asm__ (
		"push edx\n\t"
		"mov edx, %1\n\t"
		"mov [ebx+0x2A8], edx\n\t"
		"pop edx\n\t"
		"jmp %0\n\t"
			:
			: "m" (PlayerModelASM_jmp), "m" (nPlayerModelSkinID)
	);
}

void ApplyPlayerModelPatches() {
	NyaHookLib::Patch(0x482C16, &nPlayerModel);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4696C2, &PlayerModelASM);

	// skip region-based ctd models
	NyaHookLib::Patch<uint8_t>(0x4323A7, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x43231B, 0xEB);
}