auto mallocHooked_call = (void*(__cdecl*)(size_t))0x64E911;
void* __cdecl mallocHooked(size_t a1) {
	if (a1 <= 0 || a1 >= 0x7FFFFFFF) return nullptr;
	if (auto data = mallocHooked_call(a1)) {
		memset(data, 0, a1);
		return data;
	}
	return nullptr;
}

void HookMalloc() {
	uintptr_t mallocAddresses[] = {
			0x40142C,
			0x4015B5,
			0x40472C,
			0x40DFCF,
			0x40E20B,
			0x40EBAE,
			0x411902,
			0x415E05,
			0x415E4B,
			0x415E6B,
			0x415E97,
			0x415ED4,
			0x416511,
			0x4197EF,
			0x42033B,
			0x421D89,
			0x430CC5,
			0x4312BE,
			0x4315F9,
			0x433531,
			0x4336D5,
			0x433EC5,
			0x434BE8,
			0x434C48,
			0x435F21,
			0x43603C,
			0x4360F3,
			0x436719,
			0x43699A,
			0x43771D,
			0x437947,
			0x442063,
			0x44245A,
			0x44248A,
			0x44D1B5,
			0x44D270,
			0x450144,
			0x450171,
			0x450BDA,
			0x450F4D,
			0x4514FD,
			0x451963,
			0x452053,
			0x456BD6,
			0x4575F2,
			0x457E8F,
			0x457ED3,
			0x457F43,
			0x459922,
			0x459C08,
			0x45EA8A,
			0x4603D8,
			0x4603F4,
			0x461AAB,
			0x461AD7,
			0x461E13,
			0x461E4F,
			0x4620A3,
			0x46215D,
			0x468646,
			0x468669,
			0x4686EF,
			0x468A58,
			0x468A87,
			0x469543,
			0x469780,
			0x469909,
			0x46BC89,
			0x46CF26,
			0x46DADC,
			0x46F8E7,
			0x46F977,
			0x46F9C7,
			0x46FA15,
			0x46FA67,
			0x46FAB5,
			0x46FB04,
			0x46FB54,
			0x46FBB7,
			0x470016,
			0x470337,
			0x4703E8,
			0x477641,
			0x477781,
			0x4777E0,
			0x47FC7E,
			0x47FEAE,
			0x486178,
			0x4865CF,
			0x486784,
			0x486B77,
			0x486D9E,
			0x4889D8,
			0x488A09,
			0x488BF5,
			0x4892B7,
			0x48937D,
			0x48A150,
			0x48A2F0,
			0x48B462,
			0x48B487,
			0x48B4B8,
			0x48B4E0,
			0x48B506,
			0x48B534,
			0x48B555,
			0x48B592,
			0x48DA16,
			0x48E9D3,
			0x48FF85,
			0x4921D2,
			0x493123,
			0x4982A8,
			0x4999B6,
			0x4A33C2,
			0x4A3CD6,
			0x4A4395,
			0x4A43A5,
			0x4A4B7F,
			0x4A67D9,
			0x4A67E2,
			0x4A7C2F,
			0x4A83FF,
			0x4A847F,
			0x4A9104,
			0x4A91A5,
			0x4A9C92,
			0x4A9FDA,
			0x4AB909,
			0x4AB926,
			0x4AB94C,
			0x4ABA81,
			0x4AFA29,
			0x4AFB33,
			0x4AFC14,
			0x4C0611,
			0x4C064E,
			0x4C068A,
			0x4C06C7,
			0x4C06FD,
			0x4C0733,
			0x4C076F,
			0x4C07AB,
			0x4C07E3,
			0x4C081F,
			0x4C085E,
			0x4C087C,
			0x4C08B4,
			0x4C08F0,
			0x4C092C,
			0x4C0968,
			0x4C09A0,
			0x4C6292,
			0x4C62BE,
			0x4C6B27,
			0x4C6B3E,
			0x4C8265,
			0x4C849E,
			0x4C990A,
			0x4C9927,
			0x4C994D,
			0x4C9DE2,
			0x4C9E2A,
			0x4C9E72,
			0x4C9EB9,
			0x4C9F01,
			0x4C9F49,
			0x4C9F90,
			0x4C9FD8,
			0x4CA020,
			0x4CA067,
			0x4CA3D8,
			0x4CA3FA,
			0x4CA4A8,
			0x4CA4C5,
			0x4CA56F,
			0x4CAC20,
			0x4CD8D5,
			0x4CDF43,
			0x4CF203,
			0x4CF253,
			0x4CF2A3,
			0x4CF2DF,
			0x4D22AE,
			0x4D2468,
			0x4D2752,
			0x4D34EF,
			0x4D35D8,
			0x4D3A0C,
			0x4D4547,
			0x4D4786,
			0x4D4954,
			0x4D4973,
			0x4D4992,
			0x4D4B0D,
			0x4D5950,
			0x4D734D,
			0x4D7EA1,
			0x4D804C,
			0x4D892B,
			0x4D8C56,
			0x4D8CEC,
			0x4D8D29,
			0x4DB938,
			0x4DBB0B,
			0x4DBCD3,
			0x4DC00A,
			0x4DC032,
			0x4DC070,
			0x4DC098,
			0x4DC0F2,
			0x4DC147,
			0x4DC19D,
			0x4DC1BF,
			0x4DC1E1,
			0x4DC20A,
			0x4DC243,
			0x4EA738,
			0x4EBADE,
			0x4ED1AB,
			0x4EFE33,
			0x4F0128,
			0x4F0147,
			0x4F0163,
			0x4F2474,
			0x4F3A30,
			0x4F3A7A,
			0x4F3B4A,
			0x4F95FA,
			0x4F9A95,
			0x4F9B72,
			0x5019DD,
			0x501E96,
			0x501EB5,
			0x504F8C,
			0x504FAC,
			0x504FCC,
			0x504FEC,
			0x505177,
			0x505666,
			0x50579D,
			0x505C89,
			0x50A067,
			0x50B6E3,
			0x50C2FC,
			0x50D2AA,
			0x512E6E,
			0x5361F8,
			0x53625F,
			0x53A27C,
			0x53BA71,
			0x53BA80,
			0x53BBEF,
			0x53BC20,
			0x5484B5,
			0x5484F3,
			0x548519,
			0x548654,
			0x54868C,
			0x5486AC,
			0x548E25,
			0x548FF7,
			0x5492A4,
			0x5493E3,
			0x54A9B8,
			0x54BA12,
			0x54CBD6,
			0x54E131,
			0x54E17E,
			0x54E1B8,
			0x54FD1D,
			0x54FD5F,
			0x551C42,
			0x551CDB,
			0x552D35,
			0x552E7B,
			0x5530B8,
			0x5532A4,
			0x55375B,
			0x55399A,
			0x553DA3,
			0x553F51,
			0x5540F5,
			0x554E83,
			0x555298,
			0x55584E,
			0x5570C0,
			0x558B72,
			0x558C74,
			0x558CC5,
			0x558CE5,
			0x558D0B,
			0x55AC04,
			0x55AC7F,
			0x55ACFF,
			0x55AD92,
			0x55AF0C,
			0x55B638,
			0x55B992,
			0x55BAD7,
			0x55BB24,
			0x55BBB7,
			0x55BC07,
			0x55BC58,
			0x55BCB8,
			0x55CFC6,
			0x55D1A4,
			0x55D88E,
			0x55DD84,
			0x55E18E,
			0x55E77F,
			0x5600CB,
			0x560B47,
			0x56212C,
			0x562297,
			0x562937,
			0x56349A,
			0x563B27,
			0x563CC0,
			0x563E95,
			0x5645CF,
			0x578C97,
			0x578CE3,
			0x578E5E,
			0x578F0D,
			0x5792A9,
			0x579422,
			0x579456,
			0x579487,
			0x579519,
			0x57978C,
			0x579938,
			0x579A9C,
			0x57A66F,
			0x57AF35,
			0x57AFBD,
			0x57D719,
			0x57D8A2,
			0x580B1B,
			0x580DF9,
			0x5810B4,
			0x5814E4,
			0x5818C2,
			0x581D0E,
			0x58356C,
			0x58359B,
			0x5835AE,
			0x584925,
			0x584D83,
			0x584DD2,
			0x584E0A,
			0x584F6B,
			0x585283,
			0x58536F,
			0x5857AF,
			0x585DBF,
			0x586A53,
			0x586C30,
			0x58A07E,
			0x58A76D,
			0x58AC28,
			0x58AEC0,
			0x58AF08,
			0x58B3C8,
			0x58B660,
			0x58B6FF,
			0x58BCC3,
			0x58BD05,
			0x58BD3E,
			0x58BD7E,
			0x58BDDF,
			0x58BE8E,
			0x58D276,
			0x58D426,
			0x591E8C,
			0x592B35,
			0x592B78,
			0x592B94,
			0x592EEF,
			0x59326F,
			0x5969C6,
			0x5981E8,
			0x598230,
			0x598D02,
			0x598E66,
			0x59A110,
			0x59A14B,
			0x59B10A,
			0x59B180,
			0x59B701,
			0x59B813,
			0x59B8AB,
			0x59B93D,
			0x59FEE7,
			0x5A011A,
			0x5A03B1,
			0x5A0837,
			0x5A0BCF,
			0x5A0E0F,
			0x5A1799,
			0x5A337A,
			0x5A3E81,
			0x5A4819,
			0x5A4826,
			0x5A4858,
			0x5A55F4,
			0x5A573E,
			0x5A58B8,
			0x5A5A91,
			0x5A7DCB,
			0x5A7E65,
			0x5A7F38,
			0x5A81CC,
			0x5A8483,
			0x5A8545,
			0x5A88C7,
			0x5A89E7,
			0x5AA867,
			0x5AA895,
			0x5AA8D4,
			0x5AA912,
			0x5AA95D,
			0x5ABB35,
			0x5ABC5B,
			0x5ABD7F,
			0x5ABDF5,
			0x5AF445,
			0x5B2794,
			0x5B41DB,
			0x5B4366,
			0x5B4422,
			0x5B44A4,
			0x5B451C,
			0x5B4586,
			0x5B460A,
			0x5B46AD,
			0x5B474C,
			0x5B479D,
			0x5B47C9,
			0x5B485A,
			0x5B4DCC,
			0x5B4F03,
			0x5B5507,
			0x5B6E1E,
			0x5B6E59,
			0x5B6F6F,
			0x5B79DB,
			0x5B7A18,
			0x5BCDCD,
			0x5BDB97,
			0x5BDCAF,
			0x5C0F2A,
			0x5CDDBC,
			0x5D8B85,
			0x5D8EC8,
			0x5D8F57,
			0x5D9AF6,
			0x5D9B4D,
			0x5DA486,
			0x5DB012,
			0x5DD943,
			0x5DEE23,
			0x5E7B68,
			0x5E904A,
			0x5E9068,
			0x5E9086,
			0x5E90A4,
			0x5E90C2,
			0x5E90DF,
			0x5E9146,
			0x5E917F,
			0x5E919B,
			0x5E91D4,
			0x5E96C5,
			0x5E96EE,
			0x5E973A,
			0x5E9D36,
			0x5EA7EF,
			0x5EAC9B,
			0x5EACCA,
			0x5EC203,
			0x5EC2A0,
			0x5EC2FF,
			0x5EC363,
			0x5EE39E,
			0x5EE40A,
			0x5EE641,
			0x5EE6A1,
			0x5EE6FC,
			0x5EE757,
			0x5EE966,
			0x5EEA59,
			0x5EEC6C,
			0x5F2D11,
			0x5F2D91,
			0x5F3011,
			0x5F3044,
			0x5F3780,
			0x5F37C0,
			0x5F3C19,
			0x5F3C84,
			0x5F7E0F,
			0x5F83D3,
			0x5F8F78,
			0x5F97E9,
			0x5FA078,
			0x5FA748,
			0x5FC2F8,
			0x5FCA48,
			0x5FDE61,
			0x5FDF03,
			0x5FE9F8,
			0x5FEE68,
			0x5FF250,
			0x5FFD88,
			0x600578,
			0x600C48,
			0x6028D8,
			0x602C58,
			0x604868,
			0x606C0C,
			0x606C73,
			0x606D50,
			0x606E18,
			0x606FFC,
			0x607024,
			0x64A9C9,
			0x64AA22,
			0x64E9EA,
			0x650BD7,
			0x650C98,
			0x650DAE,
			0x6512C7,
			0x65DB30,
			0x65DBB3,
			0x661380,
			0x6615F7,
			0x662413,
			0x662578,
			0x6634D6,
			0x663555,
			0x66872A,
			0x6687AE,
			0x6688B9,
			0x6701A3,
			0x6703B4,
			0x673365,
			0x673475,
			0x6738D2,
			0x673935,
			0x6739A5,
			0x673B45,
			0x673D55,
	};
	for (auto& addr : mallocAddresses) {
		NyaHookLib::PatchRelative(NyaHookLib::CALL, addr, &mallocHooked);
	}
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5D8B20, &mallocHooked);
}