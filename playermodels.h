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
// 14 - fo1male 1
// 15 - fo1male 2 (aiprof7 - Seth Bellinger)
// 16 - fo1male 3 (aiprof6 - Paul McGuire)
// 17 - fo1male 4 (aiprof5 - Ray Smith)
// 18 - fo1male 5 (aiprof1 - Frank Benton)
// 19 - fo1female 1
// 20 - fo1female 2 (aiprof3 - Tania Graham)
// 21 - fo1female 3 (aiprof4 - Katie Daggert)
// 22 - fo1female 4 (aiprof2 - Sue O'Neill)
// 23 - ctd_male 1
// 24 - ctd_female 1
int GetPlayerModelSkinID() {
	if (nPlayerModel < 2) return 1;
	if (nPlayerModel < 9) return nPlayerModel; // - 2 + 2 players
	if (nPlayerModel < 14) return nPlayerModel - 7; // - 8 + 2 players
	if (nPlayerModel < 19) return nPlayerModel - 13; // fo1 male
	if (nPlayerModel < 23) return nPlayerModel - 18; // fo1 female
	if (nPlayerModel == 23) return 1; // ctd male
	if (nPlayerModel == 24) return 1; // ctd female
}
int GetPlayerModelType() {
	if (nPlayerModel < 2) return nPlayerModel;
	if (nPlayerModel < 9) return 0;
	if (nPlayerModel < 14) return 1;
	if (nPlayerModel < 19) return 2; // fo1 male
	if (nPlayerModel < 23) return 3; // fo1 female
	if (nPlayerModel == 23) return 4; // ctd male
	if (nPlayerModel == 24) return 5; // ctd female
	return 0;
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

uintptr_t PlayerModelTypeASM1_jmp = 0x423EA6;
void __attribute__((naked)) PlayerModelTypeASM1() {
	__asm__ (
		"mov ecx, [ecx+0x2A4]\n\t"
		"and ecx, 1\n\t"
		"xor eax, eax\n\t"
		"jmp %0\n\t"
			:
			: "m" (PlayerModelTypeASM1_jmp), "m" (nPlayerModelSkinID)
	);
}

uintptr_t PlayerModelTypeASM2_jmp = 0x426382;
void __attribute__((naked)) PlayerModelTypeASM2() {
	__asm__ (
		"push eax\n\t"
		"mov eax, [eax+0x2A4]\n\t"
		"and eax, 1\n\t"
		"cmp eax, ecx\n\t"
		"pop eax\n\t"

		"push edx\n\t"
		"setnz cl\n\t"
		"jmp %0\n\t"
			:
			: "m" (PlayerModelTypeASM2_jmp), "m" (nPlayerModelSkinID)
	);
}

uintptr_t PlayerModelTypeASM3_jmp = 0x476CD4;
void __attribute__((naked)) PlayerModelTypeASM3() {
	__asm__ (
		"push ebx\n\t"
		"mov ebx, [ebx+0x2A4]\n\t"
		"and ebx, 1\n\t"
		"cmp ebx, eax\n\t"
		"pop ebx\n\t"

		"fld dword ptr [ecx+0x30]\n\t"
		"jmp %0\n\t"
			:
			: "m" (PlayerModelTypeASM3_jmp), "m" (nPlayerModelSkinID)
	);
}

uintptr_t PlayerModelTypeASM4_jmp = 0x425C3F;
void __attribute__((naked)) PlayerModelTypeASM4() {
	__asm__ (
		"push ecx\n\t"
		"mov ecx, [ecx+0x2A4]\n\t"
		"and ecx, 1\n\t"
		"cmp ecx, eax\n\t"
		"pop ecx\n\t"

		"setnz al\n\t"
		"add eax, 4\n\t"
		"jmp %0\n\t"
			:
			: "m" (PlayerModelTypeASM4_jmp), "m" (nPlayerModelSkinID)
	);
}

void __fastcall SetPlayerModelType(Player* pPlayer) {
	struct tPlayerModel {
		const char* playerTexture;
		const char* playerModel;
		const char* playerHands;
		const char* playerINI;
	};
	tPlayerModel gMale = {
		"male_%i.tga",
		"data/drivers/ragdoll/male.bgm",
		"data/drivers/interior/male_hands_%i.bsa",
		"data/drivers/male.ini"
	};
	tPlayerModel gFemale = {
		"female_%i.tga",
		"data/drivers/ragdoll/female.bgm",
		"data/drivers/interior/female_hands_%i.bsa",
		"data/drivers/female.ini"
	};
	tPlayerModel gFO1Male = {
		"fo1male_%i.tga",
		"data/drivers/ragdoll/fo1male.bgm",
		"data/drivers/interior/fo1male_hands_%i.bsa",
		"data/drivers/fo1male.ini"
	};
	tPlayerModel gFO1Female = {
		"fo1female_%i.tga",
		"data/drivers/ragdoll/fo1female.bgm",
		"data/drivers/interior/fo1female_hands_%i.bsa",
		"data/drivers/fo1female.ini"
	};
	tPlayerModel gCTDMale = {
		"ctd_male_%i.tga",
		"data/drivers/ragdoll/ctd_male.bgm",
		"data/drivers/interior/ctd_male_hands_%i.bsa",
		"data/drivers/ctd_male.ini"
	};
	tPlayerModel gCTDFemale = {
		"ctd_female_%i.tga",
		"data/drivers/ragdoll/ctd_female.bgm",
		"data/drivers/interior/ctd_female_hands_%i.bsa",
		"data/drivers/ctd_female.ini"
	};

	auto playerModel = &gMale;
	switch (pPlayer->nPlayerModel) {
		case 0:
			playerModel = &gMale;
			break;
		case 1:
			playerModel = &gFemale;
			break;
		case 2:
			playerModel = &gFO1Male;
			break;
		case 3:
			playerModel = &gFO1Female;
			break;
		case 4:
			playerModel = &gCTDMale;
			break;
		case 5:
			playerModel = &gCTDFemale;
			break;
		default:
			break;
	}

	NyaHookLib::Patch(0x432333 + 1, playerModel->playerTexture);
	NyaHookLib::Patch(0x432342 + 1, playerModel->playerHands);
	NyaHookLib::Patch(0x43235B + 1, playerModel->playerINI);
	NyaHookLib::Patch(0x432389 + 1, playerModel->playerModel);
}

uintptr_t PlayerModelTypesASM_jmp = 0x432301;
void __attribute__((naked)) PlayerModelTypesASM() {
	__asm__ (
		"mov eax, [ebx+0x469C]\n\t"
		"pushad\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"popad\n\t"
		"push eax\n\t"
		"mov eax, [eax+0x2A4]\n\t"
		"and eax, 1\n\t"
		"cmp eax, 0\n\t"
		"pop eax\n\t"
		"mov esi, [ebx+0x1D94]\n\t"
		"jmp %0\n\t"
			:
			: "m" (PlayerModelTypesASM_jmp), "i" (SetPlayerModelType)
	);
}

// fo1 speedevil driverloc:
// -0.39, 0.29, -0.14 in fo2
// -0.382, 0.2, 0.027 in fo1
float fFO1DriverLocOffset[3] = { (-0.39) - (-0.382), 0.29 - 0.2, (-0.14) - 0.027 };

void __fastcall SetDriverLocOffset(Player* pPlayer, Car* pCar) {
	if (pPlayer->nPlayerModel == 2 || pPlayer->nPlayerModel == 3) {
		for (int i = 0; i < 3; i++) {
			pCar->vDriverLoc[i] -= fFO1DriverLocOffset[i];
		}
	}
}

uintptr_t DriverLocOffsetASM_jmp = 0x43B6FE;
void __attribute__((naked)) DriverLocOffsetASM() {
	__asm__ (
		"fstp dword ptr [edi+0x1D98]\n\t"
		"fld dword ptr [esp+0x3C]\n\t"
		"fstp dword ptr [edi+0x1D9C]\n\t"
		"fld dword ptr [esp+0x40]\n\t"
		"fstp dword ptr [edi+0x1DA0]\n\t"

		"pushad\n\t"
		"mov ecx, [edi+0x469C]\n\t"
		"mov edx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (DriverLocOffsetASM_jmp), "i" (SetDriverLocOffset)
	);
}

void ApplyPlayerModelPatches() {
	NyaHookLib::Patch(0x482C16, &nPlayerModel);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4696C2, &PlayerModelASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4322EE, &PlayerModelTypesASM);
	NyaHookLib::Fill(0x432301, 0x90, 0x432307 - 0x432301); // always use the male player code path
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x43B6E4, &DriverLocOffsetASM);

	// type == 1 -> type % 2 == 1
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x423E9E, &PlayerModelTypeASM1);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x426378, &PlayerModelTypeASM2);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x476CCB, &PlayerModelTypeASM3);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x425C33, &PlayerModelTypeASM4);

	// skip region-based ctd models
	NyaHookLib::Patch<uint8_t>(0x4323A7, 0xEB);
	NyaHookLib::Patch<uint8_t>(0x43231B, 0xEB);
}