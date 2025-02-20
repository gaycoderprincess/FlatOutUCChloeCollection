struct tProfileStructure {
	uint8_t _0[0xC];
	wchar_t playerName[16]; // +C
	uint8_t _2C[0x24];
	uint32_t money; // +50
	uint8_t _54[0x2];
	uint8_t carId; // +56
	uint8_t carClass; // +57
} gTempProfile;

tCustomSaveStructure gTempCustomSave;

std::string GetProfilePath(int id) {
	return std::format("Savegame/player{:03}.sav", id);
}
bool LoadProfileForStats(int id) {
	gTempCustomSave.Load(id, false);

	memset(&gTempProfile, 0, sizeof(gTempProfile));

	auto file = std::ifstream(GetProfilePath(id), std::ios::in | std::ios::binary);
	if (!file.is_open()) return false;

	file.read((char*)&gTempProfile, sizeof(gTempProfile));
	return true;
}
bool IsProfileValid(int id) {
	if (!std::filesystem::exists(GetProfilePath(id))) return false;
	if (!LoadProfileForStats(id)) return false;
	if (!gTempProfile.playerName[0]) return false;
	return true;
}
void DeleteProfile(int id) {
	std::filesystem::remove(GetProfilePath(id));
	std::filesystem::remove(GetCustomSavePath(id));
}
const wchar_t* GetProfileName(int id) {
	LoadProfileForStats(id);
	return gTempProfile.playerName;
}
int GetProfileCar(int id) {
	LoadProfileForStats(id);
	return gTempProfile.carId - 1;
}
int GetProfileClass(int id) {
	LoadProfileForStats(id);
	return gTempProfile.carClass;
}
int GetProfileCupsCompleted(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.numCupsPassed;
}
int GetProfileCarsUnlocked(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.numCarsUnlocked;
}
int GetProfileProgress(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.gameProgress;
}
int GetProfilePortrait(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.playerPortrait;
}
int GetProfilePlayerType(int id) {
	LoadProfileForStats(id);
	return GetPlayerModelType(gTempCustomSave.playerModel);
}
int GetProfileFlag(int id) {
	LoadProfileForStats(id);
	return gTempCustomSave.playerFlag;
}
int GetProfileMoney(int id) {
	LoadProfileForStats(id);
	return gTempProfile.money;
}

bool bHasProfileLoaded = false;
bool bWasProfileLoadSuccessful = false;
uintptr_t ProfileLoadedASM_jmp = 0x470195;
void __attribute__((naked)) __fastcall ProfileLoadedASM() {
	__asm__ (
		"pushad\n\t"
		"mov eax, 1\n\t"
		"mov byte ptr %1, al\n\t"
		"mov byte ptr %2, al\n\t"
		"popad\n\t"
		"mov dword ptr [esp+0x28], 0x233A\n\t"
		"jmp %0\n\t"
			:
			: "m" (ProfileLoadedASM_jmp), "m" (bHasProfileLoaded), "m" (bWasProfileLoadSuccessful)
	);
}

uintptr_t ProfileLoadFailedASM_jmp = 0x47009F;
void __attribute__((naked)) __fastcall ProfileLoadFailedASM() {
	__asm__ (
		"pushad\n\t"
		"mov eax, 1\n\t"
		"mov byte ptr %1, al\n\t"
		"xor eax, eax\n\t"
		"mov byte ptr %2, al\n\t"
		"popad\n\t"
		"mov dword ptr [esp+0x28], 0x233A\n\t"
		"jmp %0\n\t"
			:
			: "m" (ProfileLoadFailedASM_jmp), "m" (bHasProfileLoaded), "m" (bWasProfileLoadSuccessful)
	);
}

void ApplyProfilePatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47018D, &ProfileLoadedASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x470097, &ProfileLoadFailedASM);
}