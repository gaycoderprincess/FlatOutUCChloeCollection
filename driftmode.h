namespace DriftMode {
	int nTimeLimit = 5 * 60 * 1000; // 5 minutes

	bool bSimpleUI = false;

	const float fMinDotAngleForDrift = 0.97;
	const float fMinDotAngleForMultiplier = 0.92;
	const float fMinDotAngleForSpinOut = 0.05;
	const float fMaxDriftTimeout = 3;
	float fDriftScoreSpeedFactor = 50;

	float fDriftHandlingFactor = 0.6;
	float fDriftHandlingFactorFwd = 0.5;
	float fDriftHandlingTopSpeed = 100;
	float fDriftTurnTopSpeed = 70;
	float fDriftTurnSpeed = 5;
	float fDriftTurnAngSpeedCap = 10;
	//float fDriftVelocityCap = 130;
	float fDriftHandlingSpeedDropoffFactor = 0.1;

	float fDriftPositionMultiplier[32] = {
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			1,
	};

	void GetRaceTypeString(wchar_t* str, size_t len) {
		_snwprintf(str, len, L"DRIFT");
	}

	void GetRaceDescString(wchar_t* str, size_t len) {
		const wchar_t* descString = L"\nSlide your car through the corners.\n\nMaintain the fastest speed possible.\n\nConnect your drifts from one corner to the next.\n\nHitting a wall ends your drift.";
		_snwprintf(str, len, descString);
	}

	void AddScore(int playerId, const wchar_t* str, float amount, int category) {
		auto playerScore = GetPlayerScore<PlayerScoreArcadeRace>(playerId+1);
		GameFlow::AddArcadeRaceScore(str, category, pGameFlow, amount, playerScore->nPosition);
	}

	bool bLastDriftDirection = false;
	bool bDriftDirectionInited = false;
	double fCurrentDriftChain = 0;
	double fDriftChainTimer = 0;
	double fDriftNotifTimer = 0;
	int nDriftChainMultiplier = 1;
	std::string sDriftNotif;
	double fLastCarHealth = 0;
	bool bLastGhosting = 0;
	void DrawHUD() {
		static CNyaTimer gTimer;
		gTimer.Process();

		tNyaStringData data;
		data.x = 0.5;
		data.y = 0.3 - 0.04;
		data.size = 0.04;
		data.XCenterAlign = true;
		data.outlinea = 255;
		if (fCurrentDriftChain > 0) {
			DrawString(data, std::format("+{:.0f}", fCurrentDriftChain), &DrawStringFO2);
			data.y += data.size;
			DrawString(data, std::format("{}x", nDriftChainMultiplier), &DrawStringFO2);
		}
		if (fDriftNotifTimer > 0) {
			data.y += data.size;
			if (fDriftNotifTimer < 0.5) {
				data.a = data.outlinea = fDriftNotifTimer * 2 * 255;
			}
			DrawString(data, sDriftNotif, &DrawStringFO2);
		}
		fDriftNotifTimer -= gTimer.fDeltaTime;
	}

	void AddNotif(const std::string& str) {
		fDriftNotifTimer = 3;
		sDriftNotif = str;
	}

	void OnTick() {
		static CNyaTimer gTimer;
		gTimer.Process();

		if (pGameFlow->nRaceState != RACE_STATE_RACING) {
			bDriftDirectionInited = false;
			fCurrentDriftChain = 0;
			fDriftChainTimer = 0;
			fDriftNotifTimer = 0;
			nDriftChainMultiplier = 1;
			fLastCarHealth = 0;
			return;
		}

		if (!bIsDriftEvent) return;
		if (pLoadingScreen) return;
		if (pGameFlow->nGameState != GAME_STATE_RACE) return;
		if (pGameFlow->nGameRules != GR_ARCADE_RACE) return;
		DrawHUD();
	}

	void EndDriftChain(bool cashOut) {
		fDriftChainTimer = fMaxDriftTimeout + 1;
		if (cashOut && fCurrentDriftChain > 0) {
			AddScore(0, L"DRIFT", fCurrentDriftChain, 1);
			AddNotif(std::format("+{:.0f} pts", fCurrentDriftChain * nDriftChainMultiplier));
		}
		fCurrentDriftChain = 0;
		nDriftChainMultiplier = 1;
		bDriftDirectionInited = false;
	}

	void ProcessDriftGas(Player* pPlayer) {
		auto car = pPlayer->pCar;
		auto swd = car->GetMatrix()->x;
		auto fwd = car->GetMatrix()->z;
		auto vel = *car->GetVelocity();

		if ((vel.length() * 3.6) < 10) return;
		if (pPlayer->nTimeInAir) return;
		if (pPlayer->fGasPedal <= 0) return;

		auto pVel = pPlayer->pCar->GetVelocity();

		auto cross = fwd.Cross(vel);
		auto speed = pVel->length();

		// lower drift speed if at too large of an angle
		auto velNorm = vel;
		velNorm.Normalize();
		auto dot = fwd.Dot(velNorm);
		if (dot < 0) return;

		if (dot < 0.55) {
			cross.y *= dot * 2;
		}
		if (speed >= fDriftHandlingTopSpeed / 3.6) {
			auto dropoff = 1 - ((speed - (fDriftHandlingTopSpeed / 3.6)) * fDriftHandlingSpeedDropoffFactor);
			if (dropoff < 0) dropoff = 0;
			cross.y *= dropoff;
		}

		*pVel += swd * cross.y * fDriftHandlingFactor * pPlayer->fGasPedal * 0.01;
		*pVel += fwd * std::abs(cross.y) * fDriftHandlingFactorFwd * pPlayer->fGasPedal * 0.01;

		//if (pVel->length() > fDriftHandlingTopSpeed * 3.6) {
		//	pVel->Normalize();
		//	*pVel *= speed;
		//}
	}

	double GetPlayerSteeringInput(Player* pPlayer) {
		double steeringInput = GetPadKeyState(NYA_PAD_KEY_LSTICK_X) / 32767.0;
		if (pPlayer->nIsUsingKeyboard) {
			steeringInput = 0;
			if (pPlayer->nSteeringKeyboardLeft) steeringInput = -1;
			if (pPlayer->nSteeringKeyboardRight) steeringInput += 1;
		}
		return steeringInput;
	}

	void ProcessDriftSteering(Player* pPlayer) {
		auto car = pPlayer->pCar;
		auto vel = *car->GetVelocity();

		auto steerAngle = GetPlayerSteeringInput(pPlayer);
		if (std::abs(steerAngle) > 0) {
			auto turnSpeed = fDriftTurnSpeed;
			if (vel.length() < fDriftTurnTopSpeed) {
				turnSpeed *= vel.length() / fDriftTurnTopSpeed;
			}
			auto angVel = pPlayer->pCar->GetAngVelocity();
			auto tmpAngVel = *angVel;
			tmpAngVel += pPlayer->pCar->GetMatrix()->y * turnSpeed * steerAngle * 0.01;
			if (tmpAngVel.length() < fDriftTurnAngSpeedCap || tmpAngVel.length() < angVel->length()) {
				*angVel = tmpAngVel;
			}
		}
	}

	void ProcessDriftHandling(Player* pPlayer) {
		if (pPlayer->pCar->GetMatrix()->y.y < 0.5) return;
		ProcessDriftGas(pPlayer);
		ProcessDriftSteering(pPlayer);

		//auto vel = *pPlayer->pCar->GetVelocity();
		//vel.y = 0;
		//if (vel.length() > fDriftVelocityCap * 3.6) {
		//	vel.Normalize();
		//	vel *= fDriftVelocityCap * 3.6;
		//	pPlayer->pCar->GetVelocity()->x = vel.x;
		//	pPlayer->pCar->GetVelocity()->z = vel.z;
		//}
	}

	void __fastcall ProcessPlayerCarDrift(Player* pPlayer) {
		if (!bIsDriftEvent) return;
		if (pGameFlow->nGameRules != GR_ARCADE_RACE) return;

		// cash out any drifts in the last second of play
		auto score = GetPlayerScore<PlayerScoreArcadeRace>(1);
		if (score->nTimeLeft < 1000) {
			if (fCurrentDriftChain > 0) {
				EndDriftChain(true);
			}
			return;
		}

		auto carHealth = 1 - pPlayer->pCar->fDamage;
		if (fLastCarHealth > carHealth && fCurrentDriftChain > 0) {
			AddNotif("HIT WALL!\nDRIFT ENDED");
			EndDriftChain(false);
		}
		fLastCarHealth = carHealth;

		if (pPlayer->nGhosting && !bLastGhosting && fCurrentDriftChain > 0) {
			AddNotif("RESET PENALTY!\nDRIFT ENDED");
			EndDriftChain(false);
		}
		bLastGhosting = pPlayer->nGhosting;

		for (int i = 0; i < 32; i++) {
			fDriftPositionMultiplier[i] = nDriftChainMultiplier;
		}

		fDriftChainTimer += 0.01;

		auto car = pPlayer->pCar;
		auto fwd = car->GetMatrix()->z;
		fwd.y = 0;
		auto vel = *car->GetVelocity();
		vel.y = 0;
		auto velNorm = vel;
		velNorm.Normalize();

		ProcessDriftHandling(pPlayer);
		if (pPlayer->pCar->GetMatrix()->y.y < 0.5) {
			if (fCurrentDriftChain > 0) {
				AddNotif("FLIPPED!\nDRIFT ENDED");
				EndDriftChain(false);
			}
		}
		else {
			if ((vel.length() * 3.6) > 50 && !pPlayer->nTimeInAir) {
				// 1 - moving forwards
				// 0 - moving sideways
				// -1 - moving backwards
				// ideal drifts seem around 0.7-0.6 or so
				auto dot = fwd.Dot(velNorm);
				// spun out, break drift
				if (dot <= fMinDotAngleForSpinOut) {
					if (fCurrentDriftChain > 0) {
						AddNotif("SPUN OUT!\nDRIFT ENDED");
						EndDriftChain(false);
					}
					return;
				}
				else if (dot <= fMinDotAngleForDrift) {
					fDriftChainTimer = 0;

					auto dotFactor = 1 - dot;
					dotFactor = sqrt(dotFactor); // remove to make it more angle-based
					auto speedFactor = vel.length();
					//speedFactor *= spdFactor; // add to make it more speed-based

					auto pts = dotFactor * speedFactor * fDriftScoreSpeedFactor * 0.01;
					fCurrentDriftChain += pts;

					auto cross = fwd.Cross(vel);
					auto dir = cross.y >= 0;

					if (dot < fMinDotAngleForMultiplier) {
						if (dir != bLastDriftDirection && bDriftDirectionInited && nDriftChainMultiplier < 5) {
							nDriftChainMultiplier++;
						}
						bLastDriftDirection = dir;
						bDriftDirectionInited = true;
					}
				}
			}

			if (fDriftChainTimer > 3 && fCurrentDriftChain > 0) {
				EndDriftChain(true);
				return;
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
				:  "m" (ProcessPlayerCarASM_call), "i" (ProcessPlayerCarDrift)
		);
	}

	float GetLapDecayMultiplier() {
		return 0.0f;
	}

	void ApplyPatches(bool apply) {
		bIsDriftEvent = apply;
		DriftCamera::bEnabled = apply;
		SetArcadeRaceMultiplierPointer(apply ? fDriftPositionMultiplier : fArcadeRacePositionMultiplier);
		// remove scenery crash bonus
		NyaHookLib::Patch<uint8_t>(0x48D175, apply ? 0xEB : 0x75);
		// remove airtime bonus
		NyaHookLib::Patch<uint8_t>(0x48D109, apply ? 0xEB : 0x75);
		// remove checkpoint bonus and time gain
		NyaHookLib::Patch<uint64_t>(0x48CA41, apply ? 0x05D99000000126E9 : 0x05D9000001258A0F);
		// don't render checkpoints
		NyaHookLib::Patch<uint64_t>(0x4F507A, apply ? 0xB9809000000F22E9 : 0xB98000000F21840F);
		// don't read starttime
		NyaHookLib::Patch<uint16_t>(0x48C78F, apply ? 0x9090 : 0x0C7F);
		NyaHookLib::Patch(0x48C797, apply ? nTimeLimit : 120000);

		static float fZero = 0.0;
		// immediately apply bonuses
		NyaHookLib::Patch(0x46F344 + 2, apply ? &fZero : (float*)0x6DB840);

		NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x48CA54, apply ? (uintptr_t)&GetLapDecayMultiplier : 0x48E130);

		static uint64_t _0x4DC005Backup = *(uint64_t*)0x4DC005; // this is going to change as part of it calls my malloc hook
		if (apply && bSimpleUI) {
			// load FragDerbyOnline HUD
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4DC005, 0x4DC186);
			NyaHookLib::Patch<uint16_t>(0x4DC192, 0x9090);
			NyaHookLib::Patch<uint16_t>(0x4DC19B, 0x9090);
		}
		else {
			NyaHookLib::Patch<uint64_t>(0x4DC005, _0x4DC005Backup);
			NyaHookLib::Patch<uint16_t>(0x4DC192, 0x4D75);
			NyaHookLib::Patch<uint16_t>(0x4DC19B, 0x2275);
		}
		//NyaHookLib::Patch(0x4DC1AE + 1, apply ? "Data.Overlay.HUD.Speedtrap" : "Data.Overlay.HUD.FragDerbyOnline");
		//NyaHookLib::Patch(0x4DC017 + 1, apply ? "Data.Overlay.HUD.SpeedtrapArcade" : "Data.Overlay.HUD.ArcadeRace");

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

		static bool bOnce = true;
		if (bOnce) {
			ProcessPlayerCarASM_call = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47A010, &ProcessPlayerCarASM);
			bOnce = false;
		}
	}
}