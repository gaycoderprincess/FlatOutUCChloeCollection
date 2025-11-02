namespace ChloeCollection {
	template<typename T>
	T GetFuncPtr(const char* funcName) {
		if (auto dll = LoadLibraryA("FlatOutUCChloeCollection_gcp.asi")) {
			return (T)GetProcAddress(dll, funcName);
		}
		if (auto dll = LoadLibraryA("FlatOutUCChloeCollection_gcp.dll")) {
			return (T)GetProcAddress(dll, funcName);
		}
		return nullptr;
	}

	void SetUseTeamColor(int color) {
		static auto funcPtr = GetFuncPtr<void(__cdecl*)(int)>("ChloeCollection_SetUseTeamColor");
		if (!funcPtr) return;
		return funcPtr(color);
	}
}