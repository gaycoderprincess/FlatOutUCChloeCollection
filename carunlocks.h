void SetCarUnlocks() {
	auto cars = pGameFlow->Profile.Garage.aCars;
	if (!cars) return;

	static CNyaTimer gTimer;
	static double fInterval = 0;
	fInterval += gTimer.Process();
	if (fInterval > 1) {
		for (int id = 0; id < GetNumCars(); id++) {
			if (IsCarAlwaysUnlocked(id)) {
				cars[id].bIsLocked = false;
			}
			else {
				auto newId = GetUnlockIDForCustomCar(id, id > 48);
				if (newId >= 0 && newId != id && newId < GetNumCars()) {
					cars[id].bIsLocked = cars[newId].bIsLocked;
				}
			}
		}
		fInterval = 0;
	}
}