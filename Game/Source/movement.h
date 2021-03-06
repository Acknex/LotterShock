#ifndef MOVEMENT_H
	#define MOVEMENT_H

    var playerHasHazmat = 0;
    var playerHasDoubleJump = 0;

    bool movement_cheat_clipmode = false;

    bool movement_cheat_invincibility = false;

	void movement_close();

	void movement_update();

	var playerGetCameraBob();

	var playerGetSpeed();

	void player_initSpawn();
	
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
