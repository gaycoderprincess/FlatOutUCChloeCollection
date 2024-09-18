float fNewBombExplosionPower = 10;
float fNewBombExplosionAngVelocityMult = 0.25;

void __fastcall NewBombExplosions(Player* pBomber, Player* pVictim) {
	auto bomberCar = pBomber->pCar;
	auto victimCar = pVictim->pCar;

	NyaVec3 bomberPos = { bomberCar->mMatrix[12], bomberCar->mMatrix[13], bomberCar->mMatrix[14] };
	NyaVec3 victimPos = { victimCar->mMatrix[12], victimCar->mMatrix[13], victimCar->mMatrix[14] };
	bomberPos.y -= 5; // add a bit of upwards velocity to the push

	auto dist = (victimPos - bomberPos);
	auto impulse = dist * (fNewBombExplosionPower * victimCar->fMass / 1000.0 * std::min((fPowerups_BombMaxDistance - dist.length()) * 2.0 / fPowerups_BombMaxDistance, 1.0) / std::max(dist.length(), 0.01));
	for (int i = 0; i < 3; i++) {
		victimCar->vVelocity[i] += impulse[i];
		victimCar->vAngVelocity[i] += impulse[i] * fNewBombExplosionAngVelocityMult;
	}
}

uintptr_t NewBombExplosionsASM_jmp = 0x47E16D;
void __attribute__((naked)) NewBombExplosionsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, [ebp+8]\n\t" // bomb player
		"mov edx, edi\n\t" // player in current loop iteration
		"call %1\n\t"
		"popad\n\t"

		"mov esi, [edi+0x294]\n\t"
		"mov edx, [esi+0x27C]\n\t"
		"mov eax, [edx+4]\n\t"
		"jmp %0\n\t"
			:
			: "m" (NewBombExplosionsASM_jmp), "i" (NewBombExplosions)
	);
}

void ApplyBombExplosionPatches() {
	// remove vanilla bomb velocity
	NyaHookLib::Patch<uint64_t>(0x47EC7D, 0x87D990909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47EC90, 0x84D990909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47ECA3, 0x4C8B90909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47ED40, 0x84D990909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47ED70, 0xCADE90909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47ED8A, 0x8E8B90909090D8DD);
	// in some other branch too
	NyaHookLib::Patch<uint64_t>(0x47EA5A, 0x84D990909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47EA6D, 0x84D990909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47EA80, 0x4C8B90909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47EB1B, 0x84D990909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47EB4B, 0xCADE90909090D8DD);
	NyaHookLib::Patch<uint64_t>(0x47EB65, 0x44D990909090D8DD);

	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x47E15E, &NewBombExplosionsASM);
}