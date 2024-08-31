bool IsTypeableCharacterInFO2(wchar_t c) {
	// number row
	if (c == '`') return true;
	if (c == '1') return true;
	if (c == '2') return true;
	if (c == '3') return true;
	if (c == '4') return true;
	if (c == '5') return true;
	if (c == '6') return true;
	if (c == '7') return true;
	if (c == '8') return true;
	if (c == '9') return true;
	if (c == '0') return true;
	if (c == '-') return true;
	if (c == '=') return true;

	// number row + shift
	if (c == '~') return true;
	if (c == '!') return true;
	if (c == '@') return true;
	if (c == '#') return true;
	if (c == '$') return true;
	//if (c == '%') return true; // used for printf
	if (c == '^') return true;
	if (c == '&') return true;
	if (c == '*') return true;
	if (c == '(') return true;
	if (c == ')') return true;
	if (c == '_') return true;
	if (c == '+') return true;

	// letters
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;

	// symbols next to enter
	if (c == '[') return true;
	if (c == ']') return true;
	if (c == ';') return true;
	if (c == '\'') return true;
	if (c == '\\') return true;
	if (c == ',') return true;
	if (c == '.') return true;
	if (c == '/') return true;
	if (c == '{') return true;
	if (c == '}') return true;
	if (c == ':') return true;
	if (c == '"') return true;
	if (c == '|') return true;
	if (c == '<') return true;
	if (c == '>') return true;
	if (c == '?') return true;

	// spacebar
	if (c == ' ') return true;
	return false;
}

void DisableKeyboardInput(bool disable) {
	NyaHookLib::Patch<uint64_t>(0x5AEB2F, disable ? 0x68A190000001BCE9 : 0x68A1000001BB8C0F);
}

const int nMaxInputEntry = 128;
int nInputEntryLength = 128;
double fInputWindowTimer = 0;
bool bInputWindowOpen = false;
char sInputWindowText[nMaxInputEntry];
std::string sInputWindowTitle;
std::string sInputWindowLastEntry;
void SetInputWindowOpen(bool open) {
	fInputWindowTimer = 0;
	bInputWindowOpen = open;
	memset(sInputWindowText, 0, sizeof(sInputWindowText));
	DisableKeyboardInput(open);
}

bool IsStringValidForFO2Drawing(const std::string& str) {
	for (auto& c : str) {
		if (!IsTypeableCharacterInFO2(c)) return false;
	}
	return true;
}

std::string GetClipboardText() {
	if (!OpenClipboard(nullptr)) return "";

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (!hData) return "";

	auto pszText = (char*)GlobalLock(hData);
	if (!pszText) return "";

	std::string text(pszText);
	GlobalUnlock(hData);
	CloseClipboard();
	return text;
}

void AddTextInputToString(char* str, int len) {
	std::string text = str;
	if (text.length() < len - 1 && IsStringValidForFO2Drawing(sKeyboardInput)) text += sKeyboardInput;
	if (IsKeyJustPressed(VK_BACK) && !text.empty()) text.pop_back();
	if (IsKeyPressed(VK_CONTROL) && IsKeyJustPressed('V')) {
		text += GetClipboardText();
	}
	if (text.length() < len && IsStringValidForFO2Drawing(text)) strcpy_s(str, len, text.c_str());
}

void ProcessCustomInputWindow() {
	static CNyaTimer gTimer;
	gTimer.Process();

	if (bInputWindowOpen) {
		fInputWindowTimer += gTimer.fDeltaTime;

		DrawRectangle(0, 1, 0, 1, {0,0,0,127});

		tNyaStringData data;
		data.x = 0.5;
		data.y = 0.5;
		data.size = 0.04;
		data.XCenterAlign = true;
		data.outlinea = 255;
		DrawString(data, sInputWindowTitle + ": " + (std::string)sInputWindowText, &DrawStringFO2);

		AddTextInputToString(sInputWindowText, nInputEntryLength);
		if (IsKeyJustPressed(VK_RETURN) && fInputWindowTimer > 0.5) {
			sInputWindowLastEntry = sInputWindowText;
			SetInputWindowOpen(false);
		}
		else if (IsKeyJustPressed(VK_ESCAPE)) {
			SetInputWindowOpen(false);
		}
	}
}