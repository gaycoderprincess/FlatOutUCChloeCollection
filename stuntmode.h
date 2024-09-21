bool bIsStuntMode = false;
int nStuntModeTime = 5 * 60 * 1000; // 5 minutes
float fStuntModeJumpYaw = 0;
float fStuntModeJumpRoll = 0;
float fStuntModeJumpPitch = 0;
bool bStuntModeRagdolled = false;
bool bStuntModeGhosting = false;
double fStuntModeTimeTwoWheeling = 0;
double fStuntModeLastReset = 0;

enum eAirControlMode {
	AIRCONTROL_DEFAULT,
	AIRCONTROL_ON,
	AIRCONTROL_YAWONLY,
	AIRCONTROL_OFF
};
int nStuntModeAirControlMode = AIRCONTROL_DEFAULT;

float fAirControlLimitStart = 4;
float fAirControlLimitEnd = 5;
float fAirControlBaseSpeedPitch = 5;
float fAirControlBaseSpeedYaw = 7;
float fAirControlBaseSpeedRoll = 5;
float fAirControlAngVelDecay = 0.99;

float fStuntModeMinTwoWheelTime = 0.5;
float fStuntModeMaxSpinSpeed = 10;
float fStuntModeSpinMinimum = std::numbers::pi * 0.75;
float fStuntModeRollMinimum = std::numbers::pi * 0.5; // 90 degrees
float fStuntMode360RollTolerance = std::numbers::pi * 0.25; // 45 degrees
float fStuntMode360SpinTolerance = std::numbers::pi * 0.4; // 72 degrees
float fStuntModeTrickLandingTolerance = 0.2;
float fStuntModePerfectLandingTolerance = 0.05;

int nStuntModeAirtimeBonus = 4 * 0.5;
int nStuntModePerfectRollBonus = 10000 * 0.5;
int nStuntModePerfectFlipBonus = 10000 * 0.5;
int nStuntModePerfectSpinBonus = 5000 * 0.5;
int nStuntModePerfectLandingBonus = 2000 * 0.5;
int nStuntModeCrashOutBonus = 1000 * 0.5;
int nStuntModeCheesePenalty = -10000 * 0.5;
int nStuntModeResetPenalty = -5000 * 0.5;
float fStuntModeTwoWheelMultiplier = 2000 * 0.5;
float fStuntModeSpinMultiplier = 400 * 0.5;
float fStuntModeGrindMultiplier = 50 * 0.5;

bool IsCarTwoWheeling(Car* car) {
	// twowheeling is always either 0 3 or 1 2 on ground

	// 0 1 front, 2 3 rear
	if (car->aTires[0].bOnGround && car->aTires[3].bOnGround) return !car->aTires[1].bOnGround && !car->aTires[2].bOnGround;
	if (car->aTires[1].bOnGround && car->aTires[2].bOnGround) return !car->aTires[0].bOnGround && !car->aTires[3].bOnGround;
	return false;
}

bool IsCarOnAllWheels(Car* car) {
	for (int i = 0; i < 4; i++) {
		if (!car->aTires[i].bOnGround) return false;
	}
	return true;
}

bool IsCarOnNoWheels(Car* car) {
	for (int i = 0; i < 4; i++) {
		if (car->aTires[i].bOnGround) return false;
	}
	return true;
}

float fStuntModeGrindingTimer = 0;
float fStuntModeGrindingDistance = 0;
float fStuntModeLandOnAllWheelsTimer = 0;
float fStuntModeInAirTimer = 0;
bool bStuntModeDontCashOutNextLanding = false;

void ProcessAirControl(Player* pPlayer) {
	const float fDeltaTime = 0.01;

	auto car = pPlayer->pCar;
	if (car->nIsRagdolled) return;
	if (pPlayer->nTimeInAir < 300) return;

	for (int i = 0; i < 3; i++) {
		car->vAngVelocity[i] *= fAirControlAngVelDecay;
	}

	bool yawOnly = nStuntModeAirControlMode == AIRCONTROL_YAWONLY;

	double steeringInput = pPlayer->fSteeringController;
	if (pPlayer->nSteeringKeyboardLeft || pPlayer->nSteeringKeyboardRight) {
		steeringInput = 0;
		if (pPlayer->nSteeringKeyboardLeft) steeringInput = -1;
		if (pPlayer->nSteeringKeyboardRight) steeringInput += 1;
	}
	double upDownInput = GetPadKeyState(NYA_PAD_KEY_LSTICK_Y) / 32767.0;
	if (pPlayer->nIsUsingKeyboard) {
		bool upPressed = pPlayer->fGasPedal > 0.5;
		bool downPressed = pPlayer->fBrakePedal > 0.5;
		if (upPressed || downPressed) {
			upDownInput = 0;
			if (downPressed) upDownInput = -1;
			if (upPressed) upDownInput += 1;
		}
	}
	if (yawOnly) upDownInput = 0;

	if (abs(steeringInput) < 0.1 && abs(upDownInput) < 0.1) return;

	if (abs(steeringInput) < 0.1) steeringInput = 0;
	if (abs(upDownInput) < 0.1) upDownInput = 0;

	NyaVec3 angVel = {car->vAngVelocity[0], car->vAngVelocity[1], car->vAngVelocity[2]};
	NyaVec3 angVelNew = angVel;
	for (int i = 0; i < 3; i++) {
		if (car->fHandbrake > 0.5 && !yawOnly) {
			angVelNew[i] += car->mMatrix[(4 * 2) + i] * -steeringInput * fAirControlBaseSpeedRoll * fDeltaTime;
		}
		else {
			angVelNew[i] += car->mMatrix[(4 * 1) + i] * steeringInput * fAirControlBaseSpeedYaw * fDeltaTime;
		}
		angVelNew[i] += car->mMatrix[(4 * 0) + i] * upDownInput * fAirControlBaseSpeedPitch * fDeltaTime;
	}

	float airControlFactor = 1;
	// cap speed if trying to go faster
	if (angVelNew.length() > angVel.length()) {
		float angSpeed = angVel.length();
		if (angSpeed > fAirControlLimitStart)
			airControlFactor = (fAirControlLimitStart - angSpeed) /
							   (fAirControlLimitEnd - fAirControlLimitStart);
		if (angSpeed > fAirControlLimitEnd) airControlFactor = 0;
	}

	for (int i = 0; i < 3; i++) {
		car->vAngVelocity[i] = std::lerp(angVel[i], angVelNew[i], airControlFactor);
	}
}

void ResetStuntTricks() {
	fStuntModeInAirTimer = 0;
	fStuntModeJumpPitch = 0;
	fStuntModeJumpRoll = 0;
	fStuntModeJumpYaw = 0;
}

NyaMat4x4 carMatrix;

bool IsAirControlOn() {
	if (nStuntModeAirControlMode == AIRCONTROL_ON) return true;
	if ((nStuntModeAirControlMode == AIRCONTROL_DEFAULT || nStuntModeAirControlMode == AIRCONTROL_YAWONLY) && bIsStuntMode && pGame->nGameRules == GR_ARCADE_RACE) return true;
	return false;
}

void __fastcall ProcessPlayerCarStunt(Player* pPlayer) {
	if (IsAirControlOn()) {
		ProcessAirControl(pPlayer);
	}

	if (!bIsStuntMode) return;
	if (pGame->nGameRules != GR_ARCADE_RACE) return;
	*(float*)0x765044 = nStuntModeAirtimeBonus;

	// air control
	const float fDeltaTime = 0.01;
	fStuntModeLastReset += fDeltaTime;

	auto car = pPlayer->pCar;
	auto playerScore = GetPlayerScore<PlayerScoreArcadeRace>(1);

	// below map resets since we've disabled the out of map reset
	if (car->mMatrix[13] < -50 && fStuntModeLastReset > 3) {
		int eventData[9] = {EVENT_RESPAWN_PLAYER, 0, (int)pPlayer->nPlayerId, 0xFFFF, 0};
		PostEvent(eventData);
		ResetStuntTricks();
		fStuntModeLastReset = 0;

		AddArcadeRaceScore(L"RESET PENALTY", 0, pGame, nStuntModeCheesePenalty, playerScore->nUnknownScoringRelated);
	}

	if (pPlayer->nGhosting && !bStuntModeGhosting && fStuntModeLastReset > 3 && !bStuntModeRagdolled && playerScore->fScore >= -nStuntModeResetPenalty) {
		AddArcadeRaceScore(L"RESET PENALTY", 0, pGame, nStuntModeResetPenalty, playerScore->nUnknownScoringRelated);
	}
	bStuntModeGhosting = pPlayer->nGhosting;

	if (car->nIsRagdolled && !bStuntModeRagdolled) {
		AddArcadeRaceScore(L"CRASH OUT!", 0, pGame, nStuntModeCrashOutBonus, playerScore->nUnknownScoringRelated);
	}
	bStuntModeRagdolled = car->nIsRagdolled;

	NyaVec3 linVel = {car->vVelocity[0], car->vVelocity[1], car->vVelocity[2]};

	if (pPlayer->nTimeInAir <= 0 && IsCarTwoWheeling(car) && linVel.length() > 10) { // 36km/h minimum
		fStuntModeTimeTwoWheeling += fDeltaTime;
	}
	else {
		if (fStuntModeTimeTwoWheeling > fStuntModeMinTwoWheelTime) {
			AddArcadeRaceScore(L"TWO WHEELING", 0, pGame, fStuntModeTwoWheelMultiplier * fStuntModeTimeTwoWheeling,
							   playerScore->nUnknownScoringRelated);
		}
		fStuntModeTimeTwoWheeling = 0;
	}

	memcpy(&carMatrix, car->mMatrix, sizeof(carMatrix));

	if (pPlayer->nTimeInAir > 50 && !car->nIsRagdolled) {
		auto carRotation = carMatrix;
		carRotation.p = {0,0,0};
		carRotation = carRotation.Invert();

		NyaVec3 angVel = {car->vAngVelocity[0], car->vAngVelocity[1], car->vAngVelocity[2]};
		auto angVelRelative = carRotation * angVel;

		fStuntModeJumpPitch += angVelRelative.x * fDeltaTime;
		fStuntModeJumpYaw += angVelRelative.y * fDeltaTime;
		fStuntModeJumpRoll += angVelRelative.z * fDeltaTime;

		// cap spin speed
		if (angVel.length() > fStuntModeMaxSpinSpeed) {
			angVel /= angVel.length();
			angVel *= fStuntModeMaxSpinSpeed;
			for (int i = 0; i < 3; i++) {
				car->vAngVelocity[i] = angVel[i];
			}
		}

		fStuntModeInAirTimer += fDeltaTime;
		if (fStuntModeLandOnAllWheelsTimer > 0) {
			fStuntModeInAirTimer = 0;
		}
		fStuntModeLandOnAllWheelsTimer = 0;
		fStuntModeGrindingTimer = 0;
		fStuntModeGrindingDistance = 0;
	} else {
		if (pPlayer->nGhosting) {
			ResetStuntTricks();
		}

		if (IsCarOnAllWheels(car)) {
			if (fStuntModeLandOnAllWheelsTimer < fStuntModePerfectLandingTolerance && fStuntModeInAirTimer > 1.5) {
				AddArcadeRaceScore(L"PERFECT LANDING!", 0, pGame, nStuntModePerfectLandingBonus,
								   playerScore->nUnknownScoringRelated);
			}
			if (abs(fStuntModeJumpYaw) > fStuntModeSpinMinimum) {
				AddArcadeRaceScore(L"SPIN", 0, pGame, fStuntModeSpinMultiplier * abs(fStuntModeJumpYaw),
								   playerScore->nUnknownScoringRelated);
			}
			if (abs(fStuntModeJumpRoll) > fStuntModeRollMinimum) {
				AddArcadeRaceScore(L"ROLL", 0, pGame, fStuntModeSpinMultiplier * abs(fStuntModeJumpRoll),
								   playerScore->nUnknownScoringRelated);
			}
			if (fStuntModeLandOnAllWheelsTimer < fStuntModeTrickLandingTolerance) {
				float roll = abs(fStuntModeJumpRoll);
				float pitch = abs(fStuntModeJumpPitch);

				int numRolls = 0;
				while (roll > std::numbers::pi * 2) {
					roll -= std::numbers::pi * 2;
					numRolls++;
				}
				if (roll > std::numbers::pi) {
					numRolls++;
				}
				if (numRolls > 0) {
					wchar_t tmp[64];
					_snwprintf(tmp, 64, L"%dx ROLL!", numRolls);
					AddArcadeRaceScore(tmp, 0, pGame, nStuntModePerfectRollBonus * numRolls,
									   playerScore->nUnknownScoringRelated);
				}

				int numFlips = 0;
				while (pitch > std::numbers::pi * 2) {
					pitch -= std::numbers::pi * 2;
					numFlips++;
				}
				if (pitch > std::numbers::pi) {
					numFlips++;
				}
				if (numFlips > 0) {
					wchar_t tmp[64];
					_snwprintf(tmp, 64, L"%dx FLIP!", numFlips);
					AddArcadeRaceScore(tmp, 0, pGame, nStuntModePerfectFlipBonus * numFlips,
									   playerScore->nUnknownScoringRelated);
				}

				for (int i = 1; i < 9; i++) {
					double fTarget = std::numbers::pi * i;
					if (abs(abs(fStuntModeJumpYaw) - fTarget) < fStuntMode360SpinTolerance) {
						wchar_t tmp[64];
						_snwprintf(tmp, 64, L"%d!", 180 * i);
						AddArcadeRaceScore(tmp, 0, pGame, nStuntModePerfectSpinBonus * i,
										   playerScore->nUnknownScoringRelated);
						break;
					}
				}
			}
			ResetStuntTricks();
			fStuntModeLandOnAllWheelsTimer = 0;
		}
		else {
			fStuntModeLandOnAllWheelsTimer += fDeltaTime;
			if (IsCarOnNoWheels(car) && abs(carMatrix.x.y) < 0.2 && carMatrix.y.y > 0) {
				fStuntModeGrindingTimer += fDeltaTime;
				NyaVec3 vel = {car->vVelocity[0], 0, car->vVelocity[2]};
				fStuntModeGrindingDistance += vel.length() * fDeltaTime;
			}
			else if (fStuntModeGrindingTimer > 0.5 && fStuntModeGrindingDistance > 10) {
				AddArcadeRaceScore(L"GRINDING", 0, pGame, fStuntModeGrindMultiplier * abs(fStuntModeGrindingDistance),
								   playerScore->nUnknownScoringRelated);
				fStuntModeGrindingTimer = 0;
				fStuntModeGrindingDistance = 0;
			}
		}
	}
}

uintptr_t ProcessPlayerCarASM_call = 0x46C850;
void __attribute__((naked)) ProcessPlayerCarASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ProcessPlayerCarASM_call), "i" (ProcessPlayerCarStunt)
	);
}

void ApplyStuntModeAirControlPatch() {
	ProcessPlayerCarASM_call = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47A010, &ProcessPlayerCarASM);
}

void GetRaceTypeString(wchar_t* str, size_t len) {
	_snwprintf(str, len, L"STUNT SHOW");
}

void GetRaceDescString(wchar_t* str, size_t len) {
	const wchar_t* descString = L"Get as much score as possible by doing tricks with your car. Earn points by:\n\n· Spins, rolls and flips\n· Catching huge air\n· Two-wheeling\n· Throwing yourself out of your car\n\nYour car is given air control in this mode, so make good use of it to do tricks!";
	_snwprintf(str, len, descString);
}

void ApplyStuntModePatches(bool apply) {
	fStuntModeLastReset = 0;

	bIsStuntMode = apply;
	SetArcadeRaceMultiplierPointer(apply ? fArcadeRacePositionMultiplierStunt : fArcadeRacePositionMultiplier);
	// remove scenery crash bonus
	NyaHookLib::Patch<uint8_t>(0x48D175, apply ? 0xEB : 0x75);
	// remove checkpoint bonus and time gain
	NyaHookLib::Patch<uint64_t>(0x48CA41, apply ? 0x05D99000000126E9 : 0x05D9000001258A0F);
	// don't render checkpoints
	NyaHookLib::Patch<uint64_t>(0x4F507A, apply ? 0xB9809000000F22E9 : 0xB98000000F21840F);
	// don't read starttime
	NyaHookLib::Patch<uint16_t>(0x48C78F, apply ? 0x9090 : 0x0C7F);
	NyaHookLib::Patch(0x48C797, apply ? nStuntModeTime : 120000);
	// disable resetmap.4b
	NyaHookLib::Patch<uint64_t>(0x4D81F8, apply ? 0x448D9000000175E9 : 0x448D00000174840F);
	// disable airtime reset and derby oob reset
	NyaHookLib::Patch<uint8_t>(0x43D69E, apply ? 0xEB : 0x75);

	// remove crash bonuses
	NyaHookLib::Patch<uint64_t>(0x48C957, apply ? 0x4B8B9000000210E9 : 0x4B8B0000020F850F);
	NyaHookLib::Patch<uint64_t>(0x48C897, apply ? 0x4B8B90000002D0E9 : 0x4B8B000002CF850F);
	NyaHookLib::Patch<uint64_t>(0x48C8D7, apply ? 0x438B9000000290E9 : 0x438B0000028F850F);
	NyaHookLib::Patch<uint64_t>(0x48C997, apply ? 0x438B90000001D0E9 : 0x438B000001CF850F);
	NyaHookLib::Patch<uint64_t>(0x48C9D7, apply ? 0x538B9000000190E9 : 0x538B0000018F850F);
	NyaHookLib::Patch<uint64_t>(0x48C917, apply ? 0x538B9000000250E9 : 0x538B0000024F850F);
	NyaHookLib::Patch<uint64_t>(0x48C9D7, apply ? 0x538B9000000190E9 : 0x538B0000018F850F);

	NyaHookLib::Patch(0x4F2494 + 1, apply ? (uintptr_t)&GetRaceTypeString : 0x4F2920);
	NyaHookLib::Patch(0x4F24BC + 1, apply ? (uintptr_t)&GetRaceDescString : 0x4F25F0);

	const char* str = "Data.Track.Forest.Forest1.A.Checkpoints.ArcadeRace";
	NyaHookLib::Patch(0x48DEB3 + 1, apply ? (uintptr_t)str : 0x6E2568);
}