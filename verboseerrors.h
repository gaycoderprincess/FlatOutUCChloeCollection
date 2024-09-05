void __stdcall TextureErrorHooked(const char* message, const char* path) {
	std::string err;
	if (sTextureFolder[0] && sSharedTextureFolder[0]) {
		err = std::format("Cannot find texture {} in either {} or {}", path, sTextureFolder, sSharedTextureFolder);
	}
	else if (sTextureFolder[0] || sSharedTextureFolder[0]) {
		err = std::format("Cannot find texture {} in {}{}", path, sTextureFolder, sSharedTextureFolder);
	}
	else err = std::format("Cannot find texture {}", path);
	MessageBoxA(nullptr, err.c_str(), "Fatal error", 0x10);
	exit(0);
}

void __stdcall OutOfBoundsAccessHooked(const char* message, const char* propertyName, int num) {
	MessageBoxA(nullptr, std::format("PropertyDb: out-of-bounds access of property '{}' (offset {})", propertyName, num).c_str(), "Fatal error", 0x10);
	exit(0);
}

void __attribute__((naked)) __fastcall OutOfBoundsAccessErrorASM() {
	__asm__ (
		"mov edx, [eax]\n"
		"push esi\n\t"
		"push edx\n\t"
		"push 0x6F3AA4\n\t"
		"call %0\n\t"
			:
			: "i" (OutOfBoundsAccessHooked)
	);
}

void ApplyVerboseErrorsPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x5A71FD, &TextureErrorHooked);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5B2635, &OutOfBoundsAccessErrorASM);
}