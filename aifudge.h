void SetAIFudgeFactor() {
	static int nLastAIFudgeDisabled = -1;
	if (nLastAIFudgeDisabled != nAIFudgeDisabled) {
		NyaHookLib::Patch<uint16_t>(0x480ABD, nAIFudgeDisabled ? 0x9090 : 0x1875);
		NyaHookLib::Patch<uint8_t>(0x480AC5, nAIFudgeDisabled ? 0xEB : 0x75);
		static float fFudgeNone = 1.0;
		static float fFudgeHard = 0.75;
		NyaHookLib::Patch(0x480ACF + 2, nAIFudgeDisabled == 2 ? &fFudgeHard : &fFudgeNone);
		//NyaHookLib::Patch<uint64_t>(0x481D9A, nAIFudgeDisabled > 1 ? 0x44D990000000D4E9 : 0x44D9000000D38E0F); // disable velocity limits
		nLastAIFudgeDisabled = nAIFudgeDisabled;
	}
}