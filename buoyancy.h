/*volatile float fBuoyancyMaxDepth = 30;
volatile float fBuoyancyForce = 1;
float GetBuoyancyGravity(float yPos, float* outMoveSpeed) {
	float f = -9.81;
	if ((pEnvironment->bWaterPlane || bInvisWaterPlane) && yPos < fWaterPlaneY) {
		//fLastBuoyancyResult = std::lerp(1.0, 0.0, (fWaterPlaneY - yPos) / fBuoyancyMaxDepth);
		//fLastBuoyancyResult = ((fBuoyancyMaxDepth - (fWaterPlaneY - yPos)) / fBuoyancyMaxDepth) * fBuoyancyForce;
		f *= fLastBuoyancyResult;
	}
	return f;
}*/

// the above code was awful so here's something definitely not based off of a certain project that shall not be named, pls do not sue me
volatile float fBuoyancyAngMult = 0.7;
volatile float fBuoyancyTarget = 40;
volatile float fWaterDrag = 0.01;
float fLastBuoyancyResult = 0;
float fLastWaterDragResult = 0;
float fLastWaterSubmergedResult = 0;

NyaVec3 vWaterAngPush;
float GetAverageAmountSubmerged(NyaMat4x4 matrix, NyaVec3 min, NyaVec3 max) {
	if (!pEnvironment->bWaterPlane && !bInvisWaterPlane) return 0;

	vWaterAngPush = {0,0,0};

	auto step = (max - min) / 2.0;
	if (step.x <= 0) return 0;
	if (step.y <= 0) return 0;
	if (step.z <= 0) return 0;

	NyaVec3 stepRatio;
	if (step.z > step.x && step.z > step.y) {
		stepRatio.x = step.x / step.z;
		stepRatio.y = step.y / step.z;
		stepRatio.z = 1.0;
	} else if (step.y > step.x && step.y > step.z) {
		stepRatio.x = step.x / step.y;
		stepRatio.y = 1.0;
		stepRatio.z = step.z / step.y;
	}
	else {
		stepRatio.x = 1.0;
		stepRatio.y = step.y / step.x;
		stepRatio.z = step.z / step.x;
	}

	float submerged = 0;

	int numChecks = 0;
	int fractionCounter = 1;
	for (float x = min.x; x <= max.x * 1.01; x += step.x) {
		for (float y = min.y; y <= max.y * 1.01; y += step.y) {
			for (float z = min.z; z <= max.z * 1.01; z += step.z) {
				auto pos = matrix.p;
				pos += matrix.x * x;
				pos += matrix.y * y;
				pos += matrix.z * z;
				if (pos.y < fWaterPlaneY) {
					auto amount = fWaterPlaneY - pos.y;
					submerged += amount;

					auto localPos = matrix.x * x;
					localPos += matrix.y * y;
					localPos += matrix.z * z;

					float relativeWaterPlane = std::abs(fWaterPlaneY - matrix.p.y);

					NyaVec3 avgWaterLevel;
					avgWaterLevel.x = localPos.x * stepRatio.x;
					avgWaterLevel.y = (relativeWaterPlane + localPos.y) / 2.0 * stepRatio.y;
					avgWaterLevel.z = localPos.z * stepRatio.z;

					auto fraction = 1.0 / (double)fractionCounter;
					vWaterAngPush = vWaterAngPush * (1.0 - fraction) + avgWaterLevel * amount * fraction;
					fractionCounter++;
				}
				numChecks++;
			}
		}
	}

	return submerged / (double)numChecks;
}

bool ProcessBuoyancy(float mass, NyaVec3* outVel, NyaVec3* outAngVel, float amountSubmerged) {
	if (amountSubmerged <= 0) return false;

	fLastWaterSubmergedResult = amountSubmerged;

	float force = 100.0 / fBuoyancyTarget * 9.81 * mass;

	float delta = 0.01;
	fLastBuoyancyResult = amountSubmerged * force * delta;
	float impulseRatio = fLastBuoyancyResult / (9.81 * mass * delta);
	outVel->y += fLastBuoyancyResult * (1.0 / mass);
	NyaVec3 angForce = {0,fLastBuoyancyResult,0};
	*outAngVel += vWaterAngPush.Cross(angForce) * (1.0 / mass) * fBuoyancyAngMult;

	fLastWaterDragResult = 1.0 - (fWaterDrag * impulseRatio);

	// basic NaN check
	if (fLastWaterDragResult <= 1.0 && !(fLastWaterDragResult > 1.0) && fLastWaterDragResult > 0.0) {
		*outVel *= fLastWaterDragResult;
		*outAngVel *= fLastWaterDragResult;
	}

	if (impulseRatio > 0.5 && outVel->y < -5) {
		outVel->y = -5;
	}
	return true;
}

void __fastcall PropBuoyancy(DynamicObject* pProp) {
	auto min = pProp->pMesh->pSomeStruct->aModels[0].pModel->vBBoxMin;
	auto max = pProp->pMesh->pSomeStruct->aModels[0].pModel->vBBoxMax;

	ProcessBuoyancy(pProp->fMass, pProp->GetVelocity(), pProp->GetAngVelocity(), GetAverageAmountSubmerged(*pProp->GetMatrix(), min, max));
}

uintptr_t PropBuoyancyASM_jmp = 0x587FA2;
void __attribute__((naked)) __fastcall PropBuoyancyASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, edi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"pop edi\n"
		"pop esi\n"
		"add esp, 0x18\n"
		"ret 4\n\t"
			:
			: "m" (PropBuoyancyASM_jmp), "i" (PropBuoyancy)
	);
}

void __fastcall CarBuoyancy(Car* pCar) {
	// no car water physics for stone skipping or you'll auto-reset
	if (pGameFlow->nStuntType == STUNT_STONESKIPPING && pCar->nIsRagdolled) return;

	static CNyaTimer gTimer;

	if (ProcessBuoyancy(pCar->fMass, pCar->GetVelocity(), pCar->GetAngVelocity(), GetAverageAmountSubmerged(*pCar->GetMatrix(), pCar->vCollisionFullMin, pCar->vCollisionFullMax))) {
		pCar->pPlayer->nTimeInAir = 0; // remove nitro gain for airtime when in water
		if (pCar->pPlayer->nPlayerType == PLAYERTYPE_LOCAL) {
			GetAchievement("WATER_FLOAT")->fInternalProgress += gTimer.Process();
		}
		if (!bIsInMultiplayer && pCar->pPlayer->nPlayerId == 13) {
			Achievements::AwardAchievement(GetAchievement("NEVILLE_WATER"));
		}
	}
}

uintptr_t CarBuoyancyASM_jmp = 0x42AC82;
void __attribute__((naked)) __fastcall CarBuoyancyASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebp\n\t"
		"call %1\n\t"
		"popad\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"add esp, 0x58\n\t"
		"ret 0xC\n\t"
			:
			: "m" (CarBuoyancyASM_jmp), "i" (CarBuoyancy)
	);
}

void ApplyBuoyancyPatches() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x5882E7, &PropBuoyancyASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42B494, &CarBuoyancyASM);
}