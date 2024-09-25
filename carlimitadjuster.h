uintptr_t CarMatchupSwapASM_call = 0x45AFB0;
uintptr_t CarMatchupSwapASM_jmp = 0x45ED1D;
void __attribute__((naked)) __fastcall CarMatchupSwapASM() {
	__asm__ (
		"mov ebp, 0x9298FC8\n\t"
		"mov ebp, [ebp]\n\t"
		"push ebp\n\t"
		"call %1\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarMatchupSwapASM_jmp), "m" (CarMatchupSwapASM_call)
	);
}

int* newCarMatchupArrayDataToDB = nullptr;
int* newCarMatchupArrayDBToData = nullptr;
uintptr_t CarMatchupSwap2ASM_jmp = 0x45ECE2;
void __attribute__((naked)) __fastcall CarMatchupSwap2ASM() {
	__asm__ (
		"mov ecx, %1\n"
		"mov [ecx+eax*4], edi\n\t"
		"jmp %0\n\t"
			:
			: "m" (CarMatchupSwap2ASM_jmp), "m" (newCarMatchupArrayDataToDB)
	);
}

int GetCarDataID(int dbId) {
	return newCarMatchupArrayDBToData[dbId+128];
}

int GetCarDBID(int dataId) {
	static int matchups[1024];
	static bool bOnce = true;
	if (bOnce) {
		for (auto& i : matchups) {
			i = -1;
		}
		bOnce = false;
	}

	if (matchups[dataId] != -1) return matchups[dataId];

	auto db = GetLiteDB();
	for (int i = 0; i < 255; i++) {
		auto table = db->GetTable(std::format("FlatOut2.Cars.Car[{}]", i).c_str());
		auto str = (const char*)table->GetPropertyPointer("DataPath");
		if (str == std::format("data/Cars/Car_{}/", dataId)) {
			matchups[dataId] = i;
			return i;
		}
	}
	return -1;
}

int GetCarByName(const std::string& name) {
	auto db = GetLiteDB();
	for (int i = 0; i < 255; i++) {
		auto table = db->GetTable(std::format("FlatOut2.Cars.Car[{}]", i).c_str());
		auto str = (const char*)table->GetPropertyPointer("Name");
		if (str == name) {
			return i;
		}
	}
	return -1;
}

void ApplyCarLimitAdjuster() {
	newCarMatchupArrayDataToDB = new int[8192];
	newCarMatchupArrayDBToData = new int[8192]; // +512
	memset(newCarMatchupArrayDataToDB, 0, sizeof(int)*8192);
	memset(newCarMatchupArrayDBToData, 0, sizeof(int)*8192);
	NyaHookLib::Patch(0x4C7EB9, &newCarMatchupArrayDataToDB);
	NyaHookLib::Patch(0x47FB8E, &newCarMatchupArrayDBToData);
	NyaHookLib::Patch(0x55B713, &newCarMatchupArrayDBToData);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x45ED17, &CarMatchupSwapASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x45ECDB, &CarMatchupSwap2ASM);
}