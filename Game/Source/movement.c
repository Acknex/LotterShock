//////////////////////////////
// movement.c
//////////////////////////////

//VECTOR playerSpeed;
#define PLAYER_SPEED_X skill70
#define PLAYER_SPEED_Y skill71
#define PLAYER_SPEED_Z skill72
//#define PLAYER_DIST_TO_GROUND skill73
#define PLAYER_GROUND_CONTACT skill74

#define PLAYER_C_MOVE_MODE_DEFAULT (IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT | GLIDE)
#define PLAYER_C_TRACE_MODE_DEFAULT (IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT)

var playerSpeedFac = 1;
var playerMaxSpeedFac = 72;
var playerWeaponBob = 0;
var playerHeightAboveGround = 96; // does not include vertical camera.z to player.z offset!
var playerCameraHeight = 108;
var playerJumpHangtime = 6;
ANGLE playerPanSmoothed; // for weapon sway
ANGLE playerWeaponSway;
ANGLE playerAngle;
var playerHealth = 100;
var playerHealthMax = 100;
var playerLives = 3;
var playerCrouching = 0;
var playerCrouchPerc = 0;
var playerSlideCounter = 0;
var playerSlidePerc = 0;
VECTOR playerSlideDir;
var playerSlidePan = 0;

void movement_close()
{
	player = NULL;
}

var playerGetLives()
{
	return playerLives;
}

var playerGetHealth()
{
	return playerHealth;
}

var playerGetMaxHealth()
{
	return playerHealthMax;
}

void playerAddHealth(var amount)
{
	playerHealth = clamp(playerHealth+amount,0,playerHealthMax);
}

	void p_playerSlide_smoke(PARTICLE* p)
	{
		vec_add(p.vel_x,vector(random(2)-1,random(2)-1,random(2)-1));
		vec_scale(p.vel_x,2+random(2));
		p.x += (random(2)-1)*48*(1-abs(normal.x));
		p.y += (random(2)-1)*48*(1-abs(normal.y));
		p.z += (random(2)-1)*48*(1-abs(normal.z));
		p.vel_z *= 0.5;
		p.bmap = smokeSprite1_bmp;
		set(p,MOVE);
		//p.gravity = -0.2;
		p.alpha = 18+random(5);
		vec_fill(p.blue,60+random(30));
		p.size = 40+random(25);
		p.event = p_bullet_impact_smoke_fade;
	}
	
void movement_update()
{
	if(playerHealth <= 0)
	{
		playerHealth = 0;
		draw_quad(NULL,vector(0,0,0),NULL,vector(screen_size.x+1,screen_size.y+1,0),NULL,COLOR_RED,50,0);
		return;
	}
	if(!player)
	{
		VECTOR spawnPos,vMin,vMax;
		if(region_get("playerSpawn",1,vMin,vMax) == 0) // kein spawnareal gefunden
		return;
		vec_lerp(spawnPos,vMin,vMax,0.5); // keine lust, auf die richtigen pointer zu achten -.- wird eh nicht portiert
		player = ent_create(CUBE_MDL, spawnPos, NULL);
		set(player,INVISIBLE);
		playerHealth = playerHealthMax;
	}
	vec_set(player.min_x,vector(-48,-48,-48));
	vec_set(player.max_x,vector(48,48,138));
	
	// rotation
	
	vec_set(camera.pan,playerAngle);
	VECTOR temp;
	if(playerSlideCounter > 0) camera.pan = clamp(camera.pan-mouse_force.x*10*time_step,playerSlidePan-90,playerSlidePan+90);
	else
	{
		camera.pan += -mouse_force.x*10*time_step;
		camera.pan %= 360;
	}
	/*if(playerSlideCounter > 0)
	{
		var anglediff = ang(playerSlidePan-camera.pan);
		camera.pan = ;
	}*/
	
	camera.tilt = clamp(camera.tilt+mouse_force.y*10*time_step,-85,85);
	player.pan = camera.pan;
	move_min_z = 0.5;
	disable_z_glide = 1;
	move_friction = 0;
	ang_diff(temp,camera.pan,playerPanSmoothed);
	vec_scale(temp,minv(1,2*time_step));
	ang_add(playerPanSmoothed,temp);
	ang_diff(playerWeaponSway,camera.pan,playerPanSmoothed);
	//draw_text(str_printf(NULL,"camera: (%d,%d,%d)",(int)camera.pan,(int)camera.tilt,(int)camera.roll),10,240,COLOR_RED);
	//draw_text(str_printf(NULL,"playerPanSmoothed: (%d,%d,%d)",(int)playerPanSmoothed.pan,(int)playerPanSmoothed.tilt,(int)playerPanSmoothed.roll),10,260,COLOR_RED);
	//draw_text(str_printf(NULL,"playerWeaponSway: (%d,%d,%d)",(int)playerWeaponSway.pan,(int)playerWeaponSway.tilt,(int)playerWeaponSway.roll),10,280,COLOR_RED);
	
	
	// movement
	
	var playerCrouchingOld = playerCrouching;
	if(player.PLAYER_GROUND_CONTACT) playerCrouching = (key_ctrl) | (playerSlideCounter > 0);
	else playerCrouching = 0;
	//if(playerCrouching) playerCrouchPerc = minv(playerCrouchPerc+20*time_step,100);
	//else playerCrouchPerc = maxv(playerCrouchPerc-20*time_step,0);
	playerCrouchPerc += (110*playerCrouching-5-playerCrouchPerc)*0.5*time_step;
	playerCrouchPerc = clamp(playerCrouchPerc,0,100);
	
	playerSlideCounter = maxv(playerSlideCounter-time_step,0);
	if(playerCrouching && !playerCrouchingOld)
	{
		vec_normalize(temp,playerMaxSpeedFac*playerSpeedFac);
		vec_set(temp,vector(1,0,0));
		vec_rotate(temp,vector(camera.pan,0,0));
		if(vec_dot(vector(player.PLAYER_SPEED_X,player.PLAYER_SPEED_Y,0),temp) > playerMaxSpeedFac*0.5)
		{
			playerSlidePan = camera.pan;
			playerSlideCounter = 12;
			vec_set(playerSlideDir,vector(player.PLAYER_SPEED_X,player.PLAYER_SPEED_Y,0));
		}
	}
	
	playerSlidePerc += (110*(playerSlideCounter > 0)-5-playerSlidePerc)*0.5*time_step;
	playerSlidePerc = clamp(playerSlidePerc,0,100);
	playerWeaponSway.pan += -ang(playerSlidePan-camera.pan)*playerSlidePerc*0.01*0.1;
	playerWeaponSway.roll += -ang(playerSlidePan-camera.pan)*playerSlidePerc*0.01*0.5;
	
	
	VECTOR targetSpeed;
	var playerAccelerationFac = 1;
	vec_set(temp,vector(player.PLAYER_SPEED_X,player.PLAYER_SPEED_Y,0));
	if(playerSlideCounter)
	{
		vec_set(targetSpeed,playerSlideDir);
	}
	else
	{
		vec_set(targetSpeed,vector((input[INPUT_UP].down-input[INPUT_DOWN].down*0.667),(input[INPUT_LEFT].down-input[INPUT_RIGHT].down),0));
		if(targetSpeed.x || targetSpeed.y)
		{
			playerAccelerationFac = 0.5;
			vec_normalize(targetSpeed,playerMaxSpeedFac*playerSpeedFac);
			vec_rotate(targetSpeed,vector(camera.pan,0,0));
		}
		if(playerCrouchPerc > 25) vec_scale(targetSpeed,0.25);
	}
	vec_lerp(temp,temp,targetSpeed,playerAccelerationFac*time_step);
	player.PLAYER_SPEED_X = temp.x;
	player.PLAYER_SPEED_Y = temp.y;
	
	player.min_x -= 2;
	player.min_y -= 2;
	player.max_x += 2;
	player.max_y += 2;
	var moveDist = c_move(player,nullvector,vector(player.PLAYER_SPEED_X*time_step,player.PLAYER_SPEED_Y*time_step,0),PLAYER_C_MOVE_MODE_DEFAULT);
	player.min_x += 2;
	player.min_y += 2;
	player.max_x -= 2;
	player.max_y -= 2;
	if(player.PLAYER_GROUND_CONTACT)
	{
		playerWeaponBob += moveDist*0.7*(1+playerCrouchPerc*0.015)*(1-0.01*playerSlidePerc); // no time_step!
		playerWeaponBob %= 720;
	}
	if(HIT_TARGET) playerSlideCounter = 0;
	// gravity
	
	var trace_dist = 1024;
	var minZ = player.min_z;
	var maxZ = player.max_z;
	player.min_z = -0.25; // fake a flat collision hull (instead of an ellipsoid)
	player.max_z = 0.25;
	player.min_x += 8;
	player.min_y += 8;
	player.max_x -= 8;
	player.max_y -= 8;
	me = player; // for USE_BOX
	c_trace(player.x,vector(player.x,player.y,player.z-trace_dist),PLAYER_C_TRACE_MODE_DEFAULT | USE_BOX);
	me = NULL;
	player.min_z = minZ;
	player.max_z = maxZ;
	player.min_x -= 8;
	player.min_y -= 8;
	player.max_x += 8;
	player.max_y += 8;
	
	if(!trace_hit) target.z = -99999;
	else
	{
		if(playerSlideCounter > 0) effect(p_playerSlide_smoke,2,target,normal);
	}
	var heightWanted = target.z+playerHeightAboveGround;
	var distToGround = player.z-heightWanted;
	
	if(distToGround > (1+2*player.PLAYER_GROUND_CONTACT)*time_step || player.PLAYER_SPEED_Z > 0)
	{
		player.PLAYER_GROUND_CONTACT = 0;
		playerJumpHangtime = maxv(playerJumpHangtime-time_step,0);
		var fac = 1;
		if(!input[INPUT_JUMP].down) playerJumpHangtime = 0;
		if(playerJumpHangtime > 0) fac = 0.667;
		player.PLAYER_SPEED_Z = maxv(player.PLAYER_SPEED_Z-fac*24*playerSpeedFac*time_step,-240);
		c_move(player,nullvector,vector(0,0,player.PLAYER_SPEED_Z*time_step),PLAYER_C_MOVE_MODE_DEFAULT);
		if(HIT_TARGET && normal.z < 0 && target.z > player.z) player.PLAYER_SPEED_Z = minv(player.PLAYER_SPEED_Z,0);
		player.z = maxv(player.z,heightWanted);
	}
	else
	{
		player.z += (heightWanted-player.z)*0.5*time_step;
		player.PLAYER_SPEED_Z = 0;
		player.PLAYER_GROUND_CONTACT = 1;
		if(input[INPUT_JUMP].justPressed)
		{
			player.PLAYER_GROUND_CONTACT = 0;
			playerJumpHangtime = 6;
			player.PLAYER_SPEED_Z = 80;
		}
	}
	
	//if(player.PLAYER_SPEED_Z) c_move(player,nullvector,vector(0,0,maxv(player.PLAYER_SPEED_Z*time_step,distToGround)),PLAYER_C_MOVE_MODE_DEFAULT);
	
	// camera
	
	/*DEBUG_VAR(playerWeaponBob,200);
	draw_text(str_printf(NULL,"distToGround: %.1f",(double)distToGround),10,220,COLOR_RED);
	draw_text(str_printf(NULL,"heightWanted: %.1f",(double)heightWanted),10,240,COLOR_RED);
	draw_text(str_printf(NULL,"player.z: %.1f",(double)player.z),10,260,COLOR_RED);
	draw_text(str_printf(NULL,"player.PLAYER_GROUND_CONTACT: %d",(int)player.PLAYER_GROUND_CONTACT),10,280,COLOR_RED);*/
	
	camera.x = player.x;
	camera.y = player.y;
	camera.z = player.z+playerCameraHeight+sinv(playerWeaponBob)*6-playerCrouchPerc*1.1;
	camera.arc = 95;
	camera.roll += (-(input[INPUT_LEFT].down-input[INPUT_RIGHT].down)*2+sinv(playerWeaponBob*0.5)*(0.5+playerCrouchPerc*0.05)-camera.roll)*0.5*(1-0.01*playerSlidePerc)*time_step;
	
	vec_set(playerAngle,camera.pan);
	var progress = weaponGetAttackProgress();
	if(progress > 0)
	{
		var kickbackFac = weaponGetKickbackFac(progress, 30)*0.2;
		var recoilSide = sinv(total_ticks*10);
		ang_rotate(camera.pan, vector(recoilSide*25*kickbackFac,30*kickbackFac,-5*recoilSide*kickbackFac));
	}
	camera.tilt += playerSlidePerc*0.1;
}

var playerGetCameraBob()
{
	return playerWeaponBob;
}

var playerGetSpeed()
{
	if(!player) return 0;
	return vec_length(vector(player.PLAYER_SPEED_X,player.PLAYER_SPEED_Y,0));
}

VECTOR* playerGetSpeedVec()
{
	if(!player) return nullvector;
	return &player.PLAYER_SPEED_X;
}

ANGLE* playerGetWeaponSway() 
{
	return &playerWeaponSway;
}

void playerAddSpeed(VECTOR* v)
{
	if(!player) return 0;
	player.PLAYER_SPEED_X += v.x;
	player.PLAYER_SPEED_Y += v.y;
}

//////////////////////////////
// movement.c
//////////////////////////////
