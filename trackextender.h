float __fastcall NoAILookahead(void* a1, uintptr_t a2) {
	auto ai = *(Player**)(a2 + 0x1C);
	ai->fLookAheadMin = 10.0;
	ai->fLookAheadMax = 10.0;
	ai->fLookAheadModifier = 0.1;
	return 10.0;
}

// disable ai lookahead on fo1 tracks
void SetAILookahead() {
	if (auto game = pGameFlow) {
		if (game->nGameState != GAME_STATE_RACE) return;
		bool isFO1Track = DoesTrackValueExist(game->nLevelId, "UseLowAILookahead");

		//NyaHookLib::Patch<uint64_t>(0x406CF3, isFO1Track ? 0x818B90000000DEE9 : 0x818B000000DD840F);
		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x406FE0, isFO1Track ? (uintptr_t)&NoAILookahead : 0x406E50);
	}
}

float fWaterPlaneSoundY = 0;
void SetTrackVisibility() {
	bool increased = false;
	bool increasedNegY = false;
	if (pGameFlow->nGameState == GAME_STATE_RACE) {
		increased = DoesTrackValueExist(pGameFlow->nLevelId, "IncreasedVisibility");
		increasedNegY = DoesTrackValueExist(pGameFlow->nLevelId, "IncreasedNegYVisibility");

		static double waterPlaneY;
		static float camWaterPlaneY;
		waterPlaneY = 0.0;
		if (DoesTrackValueExist(pGameFlow->nLevelId, "WaterPlaneY")) {
			waterPlaneY = GetTrackValueNumber(pGameFlow->nLevelId, "WaterPlaneY");
		}
		camWaterPlaneY = waterPlaneY + 1.0;
		NyaHookLib::Patch(0x4405FE + 2, &waterPlaneY);
		NyaHookLib::Patch(0x5076EB + 2, &camWaterPlaneY);
		fWaterPlaneSoundY = waterPlaneY;

		bool invisWaterPlane = DoesTrackValueExist(pGameFlow->nLevelId, "ForceInvisibleWaterPlane");
		NyaHookLib::Patch<uint64_t>(0x44056E, invisWaterPlane ? 0x8D8D909090909090 : 0x8D8D000000CA840F); // collision
		NyaHookLib::Patch<uint16_t>(0x5076E9, invisWaterPlane ? 0x9090 : 0x1974); // camera
		NyaHookLib::Patch<uint16_t>(0x414DB9, invisWaterPlane ? 0x9090 : 0x7F74); // sound
	}

	// increase VisibilitySet grid extents for rally trophy tracks
	// rally russia extends to about 4600 for reference
	static float fNegExtentsExtended = -5000.0;
	static float fPosExtentsExtended = 5000.0;
	static float fNegExtents = -4096.0;
	static float fPosExtents = 4096.0;
	static float fNegYExtentsExtended = -100.0;
	static float fNegYExtents = -50.0;
	NyaHookLib::Patch(0x57AD5F + 2, increased ? &fNegExtentsExtended : &fNegExtents);
	NyaHookLib::Patch(0x57AD8E + 2, increased ? &fPosExtentsExtended : &fPosExtents);
	NyaHookLib::Patch(0x57AD6A + 2, increasedNegY ? &fNegYExtentsExtended : &fNegYExtents);
}

uintptr_t InitVisibilityASM_jmp = 0x55AAB9;
void __attribute__((naked)) __fastcall InitVisibilityASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			: "m" (InitVisibilityASM_jmp), "i" (SetTrackVisibility)
	);
}

/*uint32_t __fastcall ForceWaterPlane(uint32_t origValue) {
	if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->nLevelId, "InvisibleWaterPlane")) {
		NyaHookLib::Patch<uint8_t>(0x4F4CAD, 0xEB);
	}
	else {
		NyaHookLib::Patch<uint8_t>(0x4F4CAD, 0x74);
	}
	if (pGameFlow->nGameState == GAME_STATE_RACE && DoesTrackValueExist(pGameFlow->nLevelId, "ForceWaterPlane")) {
		return 1;
	}
	return origValue;
}

uintptr_t ForceWaterPlaneASM_jmp = 0x55EF3F;
void __attribute__((naked)) __fastcall ForceWaterPlaneASM() {
	__asm__ (
		// push all but eax
		"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push ebp\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"mov ecx, eax\n\t"
		"call %1\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"

		"mov ecx, [ebp+0x18]\n"
		"mov edx, [ebp+0x14]\n"
		"push ecx\n\t"
		"jmp %0\n\t"
			:
			: "m" (ForceWaterPlaneASM_jmp), "i" (ForceWaterPlane)
	);
}*/

uintptr_t WaterPlaneSoundYASM_jmp = 0x414DB9;
void __attribute__((naked)) __fastcall WaterPlaneSoundYASM() {
	__asm__ (
		"fld %1\n\t"
		"cmp byte ptr [esi+0x2DC], 0\n\t"
		"jmp %0\n\t"
			:
			: "m" (WaterPlaneSoundYASM_jmp), "m" (fWaterPlaneSoundY)
	);
}

void ApplyTrackExtenderPatches() {
	InitVisibilityASM_jmp = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x55E775, &InitVisibilityASM);
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55EF38, &ForceWaterPlaneASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x414DB0, &WaterPlaneSoundYASM);
}