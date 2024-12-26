namespace DriftCamera {
	bool bEnabled = false;
	bool bReset = false;

	NyaVec3 vPos = {0, 0, 0};
	//float fLookatOffset = 0.7;
	//float fFollowOffset = 1.7;
	float fLookatOffset = 0.7;
	float fLookatOffsetHigh = 1.2;
	float fFollowOffset = 2;
	NyaVec3 vLastPlayerPosition = {0, 0, 0};
	float fStringMinDistance = 2;
	float fStringMaxDistance = 3;
	float fStringVelocityMult = 1;
	float fStringMaxYDiff = -0.5;

	double GetMinStringDistance(Player* ply) {
		return abs(ply->pCar->vCollisionFullMin.z) * fStringMinDistance;
	}

	double GetMaxStringDistance(Player* ply) {
		return abs(ply->pCar->vCollisionFullMin.z) * fStringMaxDistance;
	}

	NyaVec3 GetLookatOffset(Player* ply) {
		auto lookatOffset = nHighCarCam ? fLookatOffsetHigh : fLookatOffset;
		return {0, abs(ply->pCar->vCollisionFullMax.y) * lookatOffset, 0};
	}

	NyaVec3 GetFollowOffset(Player* ply) {
		return {0, abs(ply->pCar->vCollisionFullMax.y) * fFollowOffset, 0};
	}

	NyaVec3* GetTargetPosition(Player* ply) {
		if (!ply) return nullptr;

		static NyaVec3 vec;
		auto plyMat = *ply->pCar->GetMatrix();
		vec = plyMat.p;
		plyMat.p = {0, 0, 0};
		vec += GetLookatOffset(ply);
		return &vec;
	}

	NyaVec3* GetFollowPosition(Player* ply) {
		if (!ply) return nullptr;

		static NyaVec3 vec;
		auto plyMat = *ply->pCar->GetMatrix();
		vec = plyMat.p;
		plyMat.p = {0, 0, 0};
		vec += GetFollowOffset(ply);
		return &vec;
	}

	void SetRotation(Camera *cam) {
		if (!cam) return;
		auto mat = cam->GetMatrix();
		auto plyPos = GetTargetPosition(GetPlayer(0));
		if (!plyPos) return;

		auto lookat = *plyPos - vPos;
		lookat.Normalize();
		*mat = NyaMat4x4::LookAt(lookat);
		mat->p = vPos;
	}

	void DoCamString() {
		auto ply = GetPlayer(0);
		auto minDist = GetMinStringDistance(ply);
		auto maxDist = GetMaxStringDistance(ply);

		auto plyPos = GetFollowPosition(ply);
		auto lookatFront = -(vPos - *plyPos);
		auto dist = lookatFront.length();
		lookatFront.Normalize();
		if (dist > maxDist) {
			vPos += lookatFront * dist;
			vPos -= lookatFront * maxDist;
		} else if (dist < minDist) {
			vPos += lookatFront * dist;
			vPos -= lookatFront * minDist;
		}
	}

	void DoMovement(Camera *cam) {
		auto player = GetPlayer(0);
		if (!player) return;

		auto mat = cam->GetMatrix();
		auto velocity = player->pCar->GetMatrix()->p - vLastPlayerPosition;
		if ((vPos - *GetFollowPosition(player)).length() >= fStringMaxDistance * 0.999) {
			velocity *= fStringVelocityMult;
		}

		vPos -= vLastPlayerPosition;
		vPos += player->pCar->GetMatrix()->p;
		vPos -= velocity;
		DoCamString();

		auto lookat = GetTargetPosition(player);
		if (vPos.y - lookat->y < fStringMaxYDiff) {
			vPos.y = lookat->y + fStringMaxYDiff;
		}

		vLastPlayerPosition = player->pCar->GetMatrix()->p;

		mat->p = vPos;
	}

	void SetCameraToDefaultPos(Player* ply) {
		auto mat = *ply->pCar->GetMatrix();
		vPos = vLastPlayerPosition = mat.p;
		vPos += GetFollowOffset(ply);
		vPos -= mat.z * GetMaxStringDistance(ply);
	}

	void ProcessCam(Camera *cam) {
		if (!cam) return;

		auto follow = GetPlayer(0);
		if (!bEnabled || !follow || pLoadingScreen || pGameFlow->nRaceState == RACE_STATE_TRACKINTRO || follow->pCar->nIsRagdolled) {
			bReset = true;
			return;
		}

		static bool bLastGhosting = false;
		if (bReset || (!bLastGhosting && follow->nGhosting)) {
			SetCameraToDefaultPos(follow);
		}
		bLastGhosting = follow->nGhosting;
		bReset = false;

		SetRotation(cam);
		DoMovement(cam);
		SetRotation(cam);
	}
}