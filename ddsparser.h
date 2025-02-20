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

bool IsUITextureFolder(const std::string& str) {
	if (str.starts_with("data/menu")) return true;
	if (str.starts_with("data/global/fonts")) return true;
	if (str.starts_with("data/global/map")) return true;
	if (str.starts_with("data/global/overlay")) return true;
	return false;
}

bool __cdecl PreferTGAForUI(const char* path, int flags) {
	auto origResult = DoesFileExist(path, 0);
	if (origResult) {
		auto texFolder1 = (std::string)sTextureFolder;
		auto texFolder2 = (std::string)sSharedTextureFolder;
		auto texFolder3 = (std::string)path;
		std::transform(texFolder1.begin(), texFolder1.end(), texFolder1.begin(), [](unsigned char c){
			if (c == '\\') return (int)'/';
			return std::tolower(c);
		});
		std::transform(texFolder2.begin(), texFolder2.end(), texFolder2.begin(), [](unsigned char c){
			if (c == '\\') return (int)'/';
			return std::tolower(c);
		});
		std::transform(texFolder3.begin(), texFolder3.end(), texFolder3.begin(), [](unsigned char c){
			if (c == '\\') return (int)'/';
			return std::tolower(c);
		});
		if (IsUITextureFolder(texFolder1) || IsUITextureFolder(texFolder2) || IsUITextureFolder(texFolder3)) {
			std::string tga = path;
			tga.pop_back();
			tga.pop_back();
			tga.pop_back();
			tga += "tga";
			// fall back to TGA if there is one, return the dds as missing
			if (DoesFileExist(tga.c_str(), 0)) {
				return false;
			}
		}
	}
	return origResult;
}

void ApplyDDSParserPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x626DF8, &NewDDSParser);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x6279E3, &PreferTGAForUI);
}