struct tChatMessage {
	std::string string;
	double timer = 0;
};
std::vector<tChatMessage> aChatMessages;

void ProcessMessageWindow() {
	static CNyaTimer gTimer;
	gTimer.Process();

	tNyaStringData data;
	data.x = 1 - (0.05 * GetAspectRatioInv());
	data.y = 0.73;
	//if (pGameFlow->nGameState == GAME_STATE_RACE) data.y = 0.65;
	data.size = 0.03;
	data.XRightAlign = true;
	for (int i = aChatMessages.size() - 1; i >= 0; i--) {
		auto message = &aChatMessages[i];

		if (message->timer > 10) continue;
		message->timer += gTimer.fDeltaTime;
		if (data.y < 0) continue;

		data.x = 1 - (0.05 * GetAspectRatioInv());

		double alpha = 1;
		if (message->timer > 9) alpha = std::lerp(1, 0, message->timer - 9);
		if (alpha <= 0) alpha = 0;
		if (alpha >= 1) alpha = 1;
		data.outlinea = data.a = alpha * 255;

		DrawString(data, message->string, &DrawStringFO2);
		data.y -= 0.03;
	}
}

void PushOnScreenMessage(const std::string& str) {
	aChatMessages.push_back({str, 0});
}