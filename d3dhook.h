void HookLoop() {
	*(float*)0x716034 = 480.1f; // hack to fix font scale
	ProcessCustomInputWindow();
	ProcessPlayStats();
	ProcessPacenotes();
	ProcessRallyHUD();
	ProcessCareerTimeTrial();
	ProcessNewReset();
	CommonMain();
	*(float*)0x716034 = 480.0f;
}

void UpdateD3DProperties() {
	g_pd3dDevice = *(IDirect3DDevice9**)(0x7242B0 + 0x60);
	ghWnd = *(HWND*)(0x7242B0 + 0x7C);
	nResX = *(int*)0x764A84;
	nResY = *(int*)0x764A88;
}

bool bDeviceJustReset = false;
void D3DHookMain() {
	if (!g_pd3dDevice) {
		UpdateD3DProperties();
		InitHookBase();
	}

	if (bDeviceJustReset) {
		ImGui_ImplDX9_CreateDeviceObjects();
		bDeviceJustReset = false;
	}
	HookBaseLoop();
}