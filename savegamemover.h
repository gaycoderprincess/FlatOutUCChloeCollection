void SavePlayerName(const wchar_t* name) {
	if (wcslen(name) >= 32) return;
	wcscpy_s(gCustomSave.playerName, 32, name);
	gCustomSave.Save();
}

const wchar_t* SaveNameHook(void* a1, int a2, void* a3) {
	auto name = lua_tolstring(a1, a2, a3);
	SavePlayerName(name);
	return name;
}

void SetCurrentPath() {
	auto path = std::filesystem::current_path().string();

	auto dest = (char*)0x8255B0;
	strcpy_s(dest, 260, path.c_str());
}

uintptr_t SaveFolderOverride_jmp = 0x55B454;
void __attribute__((naked)) SaveFolderOverride() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"add esp, 0xC\n\t"
		"mov dword ptr [edx+0x48], 0x8255B0\n\t"
		"cmp edi, ebp\n\t"
		"jmp %0\n\t"
			:
			:  "m" (SaveFolderOverride_jmp), "i" (SetCurrentPath)
	);
}

void __stdcall SavegamePlayerID(int id, uint64_t* out) {
	*out = 1;
}

void ApplySavegameMoverPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55B448, &SaveFolderOverride);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x482B12, &SaveNameHook);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x46FF8A, &SavegamePlayerID);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x470274, &SavegamePlayerID);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x470555, &SavegamePlayerID);

	static const char* saveGameFilename = "%s/Savegame/player%03d.sav";
	NyaHookLib::Patch(0x46FFA5 + 1, saveGameFilename);
	NyaHookLib::Patch(0x47028F + 1, saveGameFilename);
	NyaHookLib::Patch(0x47056C + 1, saveGameFilename);
}