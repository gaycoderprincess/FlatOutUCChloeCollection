void SetAIFudgeFactor() {
	static int nLastAIFudgeDisabled = -1;
	if (nLastAIFudgeDisabled != nAIFudgeDisabled) {
		NyaHookLib::Patch<uint16_t>(0x480ABD, nAIFudgeDisabled ? 0x9090 : 0x1875);
		NyaHookLib::Patch<uint8_t>(0x480AC5, nAIFudgeDisabled ? 0xEB : 0x75);
		static float fFudgeNone = 1.0;
		static float fFudgeHard = 0.75;
		NyaHookLib::Patch(0x480ACF + 2, nAIFudgeDisabled == 2 ? &fFudgeHard : &fFudgeNone);
		nLastAIFudgeDisabled = nAIFudgeDisabled;
	}
}