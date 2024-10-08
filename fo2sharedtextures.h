const char* sCarSharedPath = nullptr;
void SetFO2SharedTextures() {
	int carId = std::stoi((std::string)(&sTextureFolder[14]));
	//WriteLog(std::format("Loading textures for car {}", carId));
	sCarSharedPath = (carId > 200 && carId < 300) ? "data/cars/sharedfo2/" : "data/cars/shared/";
}

uintptr_t FO2SharedTexturesASM_jmp = 0x5A6E90;
void __attribute__((naked)) __fastcall FO2SharedTexturesASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov eax, %2\n\t"
		"jmp %0\n\t"
			:
			: "m" (FO2SharedTexturesASM_jmp), "i" (SetFO2SharedTextures), "m" (sCarSharedPath)
	);
}

/*void __fastcall SetFO2SharedTexturesMenu(int carId) {
	NyaHookLib::Patch(0x4C7095 + 1, (carId >= 200 && carId < 300) ? "data/cars/sharedfo2/%s" : "data/cars/shared/%s");
}

uintptr_t FO2SharedTexturesASM2_jmp = 0x4C706B;
void __attribute__((naked)) __fastcall FO2SharedTexturesASM2() {
	__asm__ (
		"mov ecx, [esi+0x2E1C0]\n"
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"push ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (FO2SharedTexturesASM2_jmp), "i" (SetFO2SharedTexturesMenu)
	);
}

bool IsFO2CarTextureShared(const std::string& str) {
	if (str.find("windows") != std::string::npos) return true;
	if (str.find("lights") != std::string::npos) return true;
	if (str.find("interior") != std::string::npos) return true;
	return false;
}

bool useSharedPath = false;
char* sCarSharedPathMenu = nullptr;
void __fastcall SetFO2SharedTexturesMenuDedicated(const char* file, int carId) {
	useSharedPath = false;
	if (carId > 200 && carId < 300 && IsFO2CarTextureShared(file)) {
		snprintf(sCarSharedPathMenu, 512, "data/cars/shared/%s", file);
		useSharedPath = true;
	}
}

uintptr_t FO2SharedTexturesASM3_jmp = 0x4C6F1C;
uintptr_t FO2SharedTexturesASM3_call = 0x649299;
void __attribute__((naked)) __fastcall FO2SharedTexturesASM3() {
	__asm__ (
		"mov edx, [eax+0x2E1C0]\n\t"
		"add esp, 4\n\t"
		"mov %4, esp\n\t"
		"sub esp, 4\n\t"
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"cmp %2, 1\n\t"
		"je noId\n\t"

		"push edx\n\t"
		"push 0x6E7B84\n\t"
		"lea eax, [esp+0xC]\n\t"
		"push eax\n\t"
		"call %3\n\t"
		"add esp, 0x10\n\t"
		"jmp %0\n\t"

		"noId:\n\t"
		"add esp, 4\n\t"
		"jmp %0\n\t"
			:
			: "m" (FO2SharedTexturesASM3_jmp), "i" (SetFO2SharedTexturesMenuDedicated), "m" (useSharedPath), "m" (FO2SharedTexturesASM3_call), "m" (sCarSharedPathMenu)
	);
}

uintptr_t FO2SharedTexturesASM4_jmp = 0x4C6F83;
uintptr_t FO2SharedTexturesASM4_call = 0x649299;
void __attribute__((naked)) __fastcall FO2SharedTexturesASM4() {
	__asm__ (
		"mov edx, [esi+0x2E1C0]\n\t"
		"add esp, 0xC\n\t"
		"mov %4, esp\n\t"
		"sub esp, 0xC\n\t"
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"cmp %2, 1\n\t"
		"je noId2\n\t"

		"push edx\n\t"
		"push 0x6E7B84\n\t"
		"lea eax, [esp+0x14]\n\t"
		"push eax\n\t"
		"call %3\n\t"
		"add esp, 0x10\n\t"
		"jmp %0\n\t"

		"noId2:\n\t"
		"add esp, 4\n\t"
		"jmp %0\n\t"
			:
			: "m" (FO2SharedTexturesASM4_jmp), "i" (SetFO2SharedTexturesMenuDedicated), "m" (useSharedPath), "m" (FO2SharedTexturesASM4_call), "m" (sCarSharedPathMenu)
	);
}*/

void ApplyFO2SharedTexturesPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x432CDF, &FO2SharedTexturesASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x434CD2, &FO2SharedTexturesASM);
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C7064, &FO2SharedTexturesASM2);
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C6F03, &FO2SharedTexturesASM3);
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4C6F6A, &FO2SharedTexturesASM4);
}