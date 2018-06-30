#ifndef MOVEMENT_H
	#define MOVEMENT_H

	void movement_close();

	void movement_update();

	var playerGetCameraBob();

	var playerGetSpeed();

	VECTOR* playerGetSpeedVec();

	void playerAddSpeed(VECTOR* v);

	ANGLE* playerGetWeaponSway();

	var playerGetLives();

	var playerGetHealth();

	var playerGetMaxHealth();
	
	// amount can be negative to reduce health
	void playerAddHealth(var amount);
	
	var playerEntMorphBallIsActive();

	void playerSetLight(COLOR* color, var lightRange, var duration);

#endif // MOVEMENT_H
