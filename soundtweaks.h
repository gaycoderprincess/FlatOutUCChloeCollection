void ApplySoundTweaks() {
	// fmod tweaks, higher channel count and lower buffer size
	NyaHookLib::Patch(0x41FDA7 + 1, 4095);
	NyaHookLib::Patch(0x41FDBE + 1, 4095);
	NyaHookLib::Patch(0x41FDE7 + 1, 4095);
	NyaHookLib::Patch(0x41FD29 + 1, 256);
	NyaHookLib::Patch(0x41FD43 + 1, 256);
	NyaHookLib::Patch(0x41FCE0 + 1, 16384);
}