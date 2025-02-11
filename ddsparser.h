bool IsNativelySupportedDDSFormat(uint32_t format) {
	if (format == 0x31545844) return true; // DXT1
	if (format == 0x33545844) return true; // DXT3
	if (format == 0x34545844) return true; // DXT4
	if (format == 0x35545844) return true; // DXT5
	return false;
}

bool __fastcall NewDDSParser(DevTexture* pThis, void*, File* pFile, uint8_t* header) {
	if (IsNativelySupportedDDSFormat(*(uint32_t*)(&header[0x50]))) {
		if (ParseGameDDS(pThis, pFile, header)) return true;
	}

	if (pThis->pD3DTexture) {
		pThis->pD3DTexture->Release();
		pThis->pD3DTexture = nullptr;
	}

	//WriteLog(std::format("loading texture {}", pThis->sPath.Get()));

	size_t fileSize = pFile->pFileCodec->GetFileSize();

	auto tmp = new uint8_t[fileSize];
	*(uint32_t*)tmp = 0x20534444; // DDS32
	memcpy(&tmp[4], header, 0x7C);
	File::ReadBytes(pFile, &tmp[0x7C+4], fileSize-0x7C-0x4, 0);

	// fix header
	if (tmp[0x4C] == 0x18) {
		tmp[0x4C] = 0x20;
	}

	auto hr = D3DXCreateTextureFromFileInMemory(pDeviceD3d->pD3DDevice, tmp, fileSize, &pThis->pD3DTexture);
	delete[] tmp;
	if (hr != S_OK) {
		WriteLog(std::format("Failed to load {} - {:X}", pThis->sPath.Get(), (uint32_t)hr));
		return false;
	}

	pThis->nLoadState = 5;
	if ((header[0x20] & 1) != 0) {
		pThis->nFlags |= 0x1000;
	}
	return true;
}

void ApplyDDSParserPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x626DF8, &NewDDSParser);
}