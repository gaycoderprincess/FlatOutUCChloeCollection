void HookLoop() {
	*(float*)0x716034 = 480.1f; // hack to fix font scale
	ProcessCustomInputWindow();
	ProcessPlayStats();
	ProcessPacenotes();
	ProcessRallyHUD();
	ProcessSplitHUD();
	CareerTimeTrial::OnTick();
	CareerRally::OnTick();
	ProcessNewReset();
	SpeedtrapMode::OnTick();
	DriftMode::OnTick();
	LapKnockoutMode::OnTick();
	CommonMain();
	*(float*)0x716034 = 480.0f;
}

void UpdateD3DProperties() {
	g_pd3dDevice = pDeviceD3d->pD3DDevice;
	ghWnd = pDeviceD3d->hWnd;
	nResX = nGameResolutionX;
	nResY = nGameResolutionY;
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

void OnD3DReset() {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
}