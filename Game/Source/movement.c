//////////////////////////////
// movement.c
//////////////////////////////

#include "input.h"

//VECTOR playerSpeed;
#define PLAYER_SPEED_X skill70
#define PLAYER_SPEED_Y skill71
#define PLAYER_SPEED_Z skill72
//#define PLAYER_DIST_TO_GROUND skill73
#define PLAYER_GROUND_CONTACT skill74

#define PLAYER_C_MOVE_MODE_DEFAULT (IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT | GLIDE)
#define PLAYER_C_TRACE_MODE_DEFAULT (IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT)

#define GROUP_PLAYER 3

#define PLAYER_STEP_DELAY_TIME 5

var playerSpeedFac = 1;
var playerMaxSpeedFac = 72; // "fac" shouldn't be there
var playerWeaponBob = 0;
var playerHeightAboveGround = 96; // does not include vertical camera.z to player.z offset!
var playerCameraHeight = 108;
var playerJumpHangtime = 6;
ANGLE playerPanSmoothed; // for weapon sway
ANGLE playerWeaponSway;
ANGLE playerAngle;
var playerHealth;
var playerHealthMax = 100;
var playerHealthOld = 100;
var playerHealthKnockbackEffect = 100;
var playerLives = 3;
var playerCrouching = 0;
var playerCrouchPerc = 0;
var playerSlideCounter = 0;
var playerSlidePerc = 0;
VECTOR playerSlideDir;
var playerSlidePan = 0;
var playerHoverPossible = 0;
var playerExtraJumpsLeft = 0;
var playerHasEntMorphBall = 1;
var playerEntMorphBallActive = 0;
var playerEntMorphBallPerc = 0;
var playerEntMorphBallCameraPan = 0;
var playerJumpNotOk = 0;

var playerStepDelay = PLAYER_STEP_DELAY_TIME;

var playerEntMorphBallPan = 0;
var playerEntMorphBallTilt = 0;
var playerEntMorphBallSpeedAdaptFac = 1;
VECTOR playerEntMorphBallPinkFlarePos;
var playerChromaticAbbTime = 0;
var playerDamageCooldownTime = 0;
var playerSndFlying;

SOUND* player_snd_step1 = "steps_1.wav";
SOUND* player_snd_step2 = "steps_2.wav";
SOUND* player_snd_step3 = "steps_3.wav";
SOUND* player_snd_jump1 = "jump_1.wav";
SOUND* player_snd_jump2 = "jump_2.wav";
SOUND* player_snd_jump3 = "jump_3.wav";
SOUND* player_snd_flying = "player_flying.wav";
SOUND* player_snd_flying_stop = "player_flying_stop.wav";
SOUND* player_snd_hit1 = "player_hit1.wav";
SOUND* player_snd_hit2 = "player_hit2.wav";
SOUND* player_snd_hit3 = "player_hit3.wav";
SOUND* player_snd_death1 = "player_death1.wav";
SOUND* player_snd_death2 = "player_death2.wav";

void player_initSpawn()
{
	playerHealthMax = 100;
	playerHealth = playerHealthMax;
    movement_cheat_invincibility = false;
    movement_cheat_clipmode = false;

    VECTOR spawnPos,vMin,vMax;
    if(region_get("playerSpawn",1,vMin,vMax) == 0) // kein spawnareal gefunden
    {
        error("region 'playerSpawn' not found!");
        return;
    }
    vec_lerp(spawnPos,vMin,vMax,0.5);
    player = ent_create("cbabe_male.mdl", spawnPos, NULL);
    //set(player,INVISIBLE);
    player.material = matObject;
    player.group = GROUP_PLAYER;
    playerHealth = playerHealthMax;

    // HACK: place player on the ground
    movement_update();
}

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
    if((amount < 0) && movement_cheat_invincibility)
        return; // haha

	if(amount < 0 && playerDamageCooldownTime > 0)
		return; // Prevent instagibbing
	
	playerHealth = clamp(playerHealth+amount,0,playerHealthMax);
	if(amount < 0) 
	{
		if(playerHealth == 0)
		{
			switch(integer(random(2)))
			{
				case 0: snd_play(player_snd_death1, 100, 0); break;
				case 1: snd_play(player_snd_death2, 100, 0); break;
			}
		}
		else
		{
			switch(integer(random(3)))
			{
				case 0: snd_play(player_snd_hit1, 100, 0); break;
				case 1: snd_play(player_snd_hit2, 100, 0); break;
				case 2: snd_play(player_snd_hit3, 100, 0); break;
			}
		}
		playerChromaticAbbTime = 0.5;
		playerDamageCooldownTime = 20;
	}
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

BMAP* pinkFlare_bmp = "pinkFlare.tga";

void p_pinkFlare_fade(PARTICLE* p)
{
	p.size = p.lifespan*8;
}

void p_pinkFlare(PARTICLE* p)
{
	p.bmap = pinkFlare_bmp;
	set(p,BRIGHT);
	p.alpha = 2;
	vec_fill(p.blue,255);
	p.size = 72;
	p.lifespan = 12;
	p.event = p_pinkFlare_fade;
}

//VECTOR playerEntMorhBallCamPos;


var playerEntMorphBallIsActive()
{
	return (playerEntMorphBallActive || playerEntMorphBallPerc);
}

var weaponsCurrentPrev = 0;

void playerEntMorphBallDo()
{
	VECTOR temp,temp2;
	if(input_hit(INPUT_MORPHBALL) && playerEntMorphBallPerc == 100) playerEntMorphBallActive = 2;
	if(playerEntMorphBallActive == 2)
	{
		playerEntMorphBallPerc = maxv(playerEntMorphBallPerc-10*time_step,0);
		if(!playerEntMorphBallPerc) playerEntMorphBallActive = 0;
	}
	else playerEntMorphBallPerc = minv(playerEntMorphBallPerc+10*time_step,100);
	
	vec_set(player.min_x,vector(-40,-40,-24));
	vec_set(player.max_x,vector(40,40,24));
	
	//var dist = vec_dist(player.x,camera.x);
	/*if(dist < 512)
	{
	}*/

	var PLAYER_SPEED_Z_PREV = player.PLAYER_SPEED_Z;
	player.PLAYER_SPEED_Z = 0;
	VECTOR targetSpeed;
	if(playerEntMorphBallActive != 1 || playerEntMorphBallPerc < 50) vec_set(targetSpeed,nullvector);
	else
	{
        vec_set(targetSpeed,vector((input_down(INPUT_UP)-input_down(INPUT_DOWN)*0.667),(input_down(INPUT_LEFT)-input_down(INPUT_RIGHT)),0));
		if(targetSpeed.x || targetSpeed.y)
		{
			vec_normalize(targetSpeed,playerMaxSpeedFac);
			vec_rotate(targetSpeed,vector(camera.pan,0,0));
		}
	}
	vec_lerp(player.PLAYER_SPEED_X,player.PLAYER_SPEED_X,targetSpeed,playerEntMorphBallSpeedAdaptFac*0.25*time_step*(0.2+0.8*((targetSpeed.x || targetSpeed.y))));
	playerEntMorphBallSpeedAdaptFac = minv(playerEntMorphBallSpeedAdaptFac+0.1*time_step,1);
	
	player.min_x -= 2;
	player.min_y -= 2;
	player.max_x += 2;
	player.max_y += 2;
	var moveDist = c_move(player,nullvector,vector(player.PLAYER_SPEED_X*time_step,player.PLAYER_SPEED_Y*time_step,0),PLAYER_C_MOVE_MODE_DEFAULT);
	player.min_x += 2;
	player.min_y += 2;
	player.max_x -= 2;
	player.max_y -= 2;
	if(HIT_TARGET)
	{
		effect(p_bullet_impact_smoke,3+(random(2) > 1),target,normal);
		vec_set(bounce,normal);
		vec_scale(bounce,-2*vec_dot(player.PLAYER_SPEED_X,normal));
		vec_add(player.PLAYER_SPEED_X,bounce);
		playerEntMorphBallSpeedAdaptFac = 1;
	}

	if(vec_to_angle(temp,player.PLAYER_SPEED_X) > 1) // && groundContact
	{
		playerEntMorphBallPan += ang(temp.x-playerEntMorphBallPan)*time_step;
		playerEntMorphBallPan = ang(playerEntMorphBallPan);
	}
	if(playerEntMorphBallActive != 1)
	{
		playerEntMorphBallTilt += (-360-playerEntMorphBallTilt)*0.35*time_step;
	}
	else
	{
		playerEntMorphBallTilt -= moveDist*2;
		playerEntMorphBallTilt %= 360;
	}
	vec_set(player.pan,vector(playerEntMorphBallPan,0,0));
	ang_rotate(player.pan,vector(0,playerEntMorphBallTilt,0));

	player.PLAYER_SPEED_Z = PLAYER_SPEED_Z_PREV;
	ent_animate(player,"ball",clamp(playerEntMorphBallPerc*1.1-10,0,100),0);

	var trace_dist = 1024;
	player.min_x += 8;
	player.min_y += 8;
	player.max_x -= 8;
	player.max_y -= 8;
	me = player; // for USE_BOX
	c_trace(player.x,vector(player.x,player.y,player.z-trace_dist),PLAYER_C_TRACE_MODE_DEFAULT | USE_BOX);
	me = NULL;
	player.min_x -= 8;
	player.min_y -= 8;
	player.max_x += 8;
	player.max_y += 8;
	
	if(!trace_hit) target.z = -99999;
	var heightWanted = target.z+46+(100-playerEntMorphBallPerc)*0.5;
	
	if(player.z > heightWanted+(2+4*player.PLAYER_GROUND_CONTACT)*time_step || player.PLAYER_SPEED_Z > 0)
	{
		player.PLAYER_GROUND_CONTACT = 0;
		playerJumpHangtime = maxv(playerJumpHangtime-time_step,0);
		var fac = 1;
        if(!input_down(INPUT_JUMP)) playerHoverPossible = playerJumpHangtime = 0;
		if(playerJumpHangtime > 0) fac = 0.667;
		player.PLAYER_SPEED_Z = maxv(player.PLAYER_SPEED_Z-fac*24*playerSpeedFac*time_step,-240);
		c_move(player,nullvector,vector(0,0,player.PLAYER_SPEED_Z*time_step),PLAYER_C_MOVE_MODE_DEFAULT);
		if(HIT_TARGET && normal.z < 0 && target.z > player.z) player.PLAYER_SPEED_Z = minv(player.PLAYER_SPEED_Z,0);
		player.z = maxv(player.z,heightWanted);

        /*if(input_hit(INPUT_JUMP) && playerExtraJumpsLeft > 0)
		{
			playerExtraJumpsLeft--;
			playerJumpHangtime = 6;
			player.PLAYER_SPEED_Z = minv(player.PLAYER_SPEED_Z*0.1+80,100);
		}*/
	}
	else
	{
		player.z += (heightWanted-player.z)*time_step;
		player.PLAYER_SPEED_Z = 0;
		player.PLAYER_GROUND_CONTACT = 1;
		static var smoke_counter = 0;
		smoke_counter += moveDist;
		if(smoke_counter > 10)
		{
			smoke_counter -= 10;
			effect(p_playerSlide_smoke,2,target,normal);
		}
		playerExtraJumpsLeft = 0; //playerHasDoubleJump;
        /*if(input_hit(INPUT_JUMP))
		{
			player.PLAYER_GROUND_CONTACT = 0;
			playerJumpHangtime = 6;
			player.PLAYER_SPEED_Z = 80;
			effect(p_playerSlide_smoke,30,target,normal);
		}*/
	}

	///////////////////////
	// camera

	vec_set(temp,vector(-400,0,player.z+260));
	vec_rotate(temp,vector(playerEntMorphBallCameraPan,0,0));
	vec_add(temp,player.x);

	var fac = vec_length(vector(player.PLAYER_SPEED_X,player.PLAYER_SPEED_Y,0));
	fac = pow(fac/80.0,0.5);
	
	if(playerEntMorphBallActive != 1) vec_for_bone(temp2,player,"Bone19");
	else vec_set(temp2,player.x);
	c_ignore(GROUP_PLAYER,0);
	me = player;
	var dist = c_trace(vector(player.x,player.y,player.z+64),temp,PLAYER_C_TRACE_MODE_DEFAULT | USE_BOX);
	me = NULL;
	if(trace_hit)
	{
		VECTOR temp3;
		vec_diff(temp,target,player.x);
		vec_set(temp3,temp);
		vec_normalize(temp3,1);
		vec_normalize(temp,maxv(32,vec_length(temp)-16));
		vec_add(temp,player.x);
		
		if(vec_dot(camera.x,temp3) > vec_dot(temp,temp3)+8) vec_lerp(camera.x,camera.x,temp,0.25*time_step);
	}
	vec_lerp(temp,temp2,temp,playerEntMorphBallPerc*0.01);
	vec_lerp(camera.x,camera.x,temp,time_step*(1-playerEntMorphBallPerc*0.0065));
	vec_diff(temp,player.x,camera.x);
	vec_to_angle(temp2,temp);
	temp2.z = 0;
	if(playerEntMorphBallActive != 1)
	{
		VECTOR temp3;
		ang_diff(temp3,vector(playerEntMorphBallPan,0,0),temp2);
		vec_scale(temp3,(100-playerEntMorphBallPerc)*0.005);
		ang_add(temp2,temp3);
	playerEntMorphBallCameraPan += ang(playerEntMorphBallPan-playerEntMorphBallCameraPan)*(1-playerEntMorphBallPerc*0.01)*0.25*time_step;
	}
	ang_diff(temp,temp2,camera.pan);
	vec_scale(temp,time_step); // minv(1,2*time_step)
	ang_add(camera.pan,temp);
	playerEntMorphBallCameraPan += clamp(ang(playerEntMorphBallPan-playerEntMorphBallCameraPan)*0.1,-7,7)*fac*time_step;
	camera.pan = playerEntMorphBallCameraPan;
	
	vec_diff(temp,player.x,playerEntMorphBallPinkFlarePos);
	vec_normalize(temp,1);
	while(vec_dist(playerEntMorphBallPinkFlarePos,player.x) > 1)
	{
		vec_add(playerEntMorphBallPinkFlarePos,temp);
		effect(p_pinkFlare,1,playerEntMorphBallPinkFlarePos,nullvector);
	}
	
	if(!playerEntMorphBallPerc && !playerEntMorphBallActive)
	{
		weapons_disabled = 0;
		weapons.current = weaponsCurrentPrev;
		camera.pan = player.pan = playerEntMorphBallPan;
		player.tilt = 0;
		player.roll = 0;
		vec_set(playerAngle,camera.pan);
	}
	
}

var input_axis_fac = 1;
var playerLightRange = 0;
var playerLightDuration = 0;
var playerLightDurationMax = 2;
void playerSetLight(COLOR* color, var lightRange, var duration)
{
	if(!player) return;
	ENTITY* entMuzzleFlashHelper = (ENTITY*)player.skill2;
	if(!entMuzzleFlashHelper) player.skill2 = entMuzzleFlashHelper = ent_create(NULL,player.x,NULL);
	vec_set(entMuzzleFlashHelper.blue,color);
	playerLightRange = entMuzzleFlashHelper.lightrange = lightRange;
	playerLightDurationMax = playerLightDuration = duration;
}

void movement_update()
{
	if(playerHealth <= 0)
	{
		playerHealth = 0;
		// draw_quad(NULL,vector(0,0,0),NULL,vector(screen_size.x+1,screen_size.y+1,0),NULL,COLOR_RED,50,0);
		if(camera.tilt < 80)
		{
			camera.tilt = minv(camera.tilt+ 120*time_step/16, 80);
		}
		if(camera.roll < 30)
		{
			camera.roll = minv(camera.roll + 120*time_step/16, 80);
		}
		pp_desync(sinv(15*total_ticks)*15, sinv(7*total_ticks)*25);
		return;
	}
#ifdef DEBUG
    if(key_t || movement_cheat_clipmode)
#else
    if(movement_cheat_clipmode)
#endif
	{
        camera.pan += -input_axis(INPUT_AXIS_LOOK_HORIZONTAL);
		camera.pan %= 360;
        camera.tilt = clamp(camera.tilt+input_axis(INPUT_AXIS_LOOK_VERTICAL),-85,85);
		player.pan = camera.pan;
		VECTOR temp;
        vec_set(temp,vector((input_down(INPUT_UP)-input_down(INPUT_DOWN)*0.667),(input_down(INPUT_LEFT)-input_down(INPUT_RIGHT)),0));
		vec_scale(temp,80*time_step);
		vec_rotate(temp,camera.pan);
		vec_add(player.x,temp);
		vec_add(camera.x,temp);
		return;
	}

	// desync screen on hits and when health is low
	playerChromaticAbbTime = maxv(0, playerChromaticAbbTime - time_step/16);
	var additionalEffect = maxv(40-playerHealth, 0)/40;
	pp_desync(
		playerChromaticAbbTime/0.4*30,
		sinv(6*total_ticks)*10*additionalEffect
	);

	if(playerDamageCooldownTime > 0)
		playerDamageCooldownTime -= 1 * time_step;
	
	// DEBUG_VAR(playerDamageCooldownTime, 0);

	vec_set(player.blue,vector(230,245,255));
	player.lightrange = 1000;
	sun_light = 0;
	
	if(player.z < -3000)
	{
		VECTOR spawnPos,vMin,vMax;
		if(region_get("playerSpawn",1,vMin,vMax))
		{
			vec_lerp(player.x,vMin,vMax,0.5);
			vec_set(playerEntMorphBallPinkFlarePos,player.x);
		}
	}
	
	if(playerHasEntMorphBall && input_hit(INPUT_MORPHBALL) && !playerEntMorphBallPerc)
	{
		vec_set(playerEntMorphBallPinkFlarePos,player.x);
		playerEntMorphBallActive = 1;
		playerEntMorphBallPan = player.pan;
		playerEntMorphBallCameraPan = playerEntMorphBallPan = player.pan;
		weaponsCurrentPrev = weapons.current;
        weapons.current = 0;
        weapons_disabled = 1;
        weapons_update();
	}
	if(playerEntMorphBallActive)
	{
		//weapons_close();
		camera.genius = NULL;
		playerEntMorphBallDo();
		return;
	}
	camera.genius = player;
	vec_set(player.min_x,vector(-48,-48,-48));
	vec_set(player.max_x,vector(48,48,138));
	
	// rotation
	
    var turnSpeedX = input_axis(INPUT_AXIS_LOOK_HORIZONTAL);
    var turnSpeedY = input_axis(INPUT_AXIS_LOOK_VERTICAL);
	/*if(mouse_right)
	{
		turnSpeedX = mouse_force.x*10;
		turnSpeedY = mouse_force.y*10;
	}*/
	vec_set(camera.pan,playerAngle);
	VECTOR temp;
    if(playerSlideCounter > 0) camera.pan = clamp(camera.pan-turnSpeedX,playerSlidePan-90,playerSlidePan+90);
	else
	{
        camera.pan += -turnSpeedX;
		camera.pan %= 360;
	}
	/*if(playerSlideCounter > 0)
	{
		var anglediff = ang(playerSlidePan-camera.pan);
		camera.pan = ;
	}*/
	
    camera.tilt = clamp(camera.tilt+turnSpeedY,-85,85);
	player.pan = camera.pan;
	move_min_z = 0.75;
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
	if(player.PLAYER_GROUND_CONTACT) playerCrouching = input_down(INPUT_CROUCH) | (playerSlideCounter > 0);
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
			vec_normalize(playerSlideDir,playerMaxSpeedFac*1.125);
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
        vec_set(targetSpeed,vector((input_down(INPUT_UP)-input_down(INPUT_DOWN)*0.667),(input_down(INPUT_LEFT)-input_down(INPUT_RIGHT)),0));
		if(targetSpeed.x || targetSpeed.y)
		{
			if(player.PLAYER_GROUND_CONTACT != 0)
			{
				
				playerStepDelay += time_step;
				if(playerStepDelay > PLAYER_STEP_DELAY_TIME)
				{
					playerStepDelay -= PLAYER_STEP_DELAY_TIME;
					switch(integer(random(3)))
					{
						case 0: snd_play(player_snd_step1, 100, 0); break;
						case 1: snd_play(player_snd_step2, 100, 0); break;
						case 2: snd_play(player_snd_step3, 100, 0); break;
					}
				}
			}
			else {
				playerStepDelay = PLAYER_STEP_DELAY_TIME;
			}
			playerAccelerationFac = 0.5;
			vec_normalize(targetSpeed,playerMaxSpeedFac*playerSpeedFac);
			vec_rotate(targetSpeed,vector(camera.pan,0,0));
		}
		else {
			playerStepDelay = PLAYER_STEP_DELAY_TIME;
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
	
		playerJumpNotOk = maxv(playerJumpNotOk-time_step,0);
	if(!trace_hit) target.z = -99999;
	else
	{
		if(normal.z < 0.5)
		{
			playerJumpNotOk = 2;
			player.PLAYER_SPEED_X += normal.x*20*time_step;
			player.PLAYER_SPEED_Y += normal.y*20*time_step;
		}
		if(playerSlideCounter > 0) effect(p_playerSlide_smoke,2,target,normal);
	}
	var heightWanted = target.z+playerHeightAboveGround;
	var distToGround = player.z-heightWanted;
	
	if(distToGround > (1+2*player.PLAYER_GROUND_CONTACT)*time_step || player.PLAYER_SPEED_Z > 0 || playerJumpNotOk)
	{
		player.PLAYER_GROUND_CONTACT = 0;
		playerJumpHangtime = maxv(playerJumpHangtime-time_step,0);
		playerHoverPossible = maxv(playerHoverPossible-time_step,0);
		var fac = 1;
        if(!input_down(INPUT_JUMP))
		{
			playerHoverPossible = playerJumpHangtime = 0;
		}
		if(playerJumpHangtime > 0) fac = 0.667;
		if(playerHoverPossible)
		{
			fac *= clamp(player.PLAYER_SPEED_Z*0.05,0,1);
			effect(p_playerSlide_smoke,2,target,vector(0,0,-30-random(30)));
		}
		else {
			if(snd_playing(playerSndFlying))
			{
				snd_stop(playerSndFlying);
				snd_play(player_snd_flying_stop, 100, 0);
				playerSndFlying = 0;
			}
		}
		player.PLAYER_SPEED_Z = maxv(player.PLAYER_SPEED_Z-fac*24*playerSpeedFac*time_step,-240);
		c_move(player,nullvector,vector(0,0,player.PLAYER_SPEED_Z*time_step),PLAYER_C_MOVE_MODE_DEFAULT);
		if(HIT_TARGET && normal.z < 0 && target.z > player.z) player.PLAYER_SPEED_Z = minv(player.PLAYER_SPEED_Z,0);
		if(!playerJumpNotOk) player.z = maxv(player.z,heightWanted);

        if(input_hit(INPUT_JUMP) && playerExtraJumpsLeft > 0 && !playerJumpNotOk)
		{
			playerSndFlying = snd_play(player_snd_flying, 100, 0);
			playerExtraJumpsLeft--;
			playerJumpHangtime = 6;
			playerHoverPossible = 16;
			player.PLAYER_SPEED_Z = minv(player.PLAYER_SPEED_Z*0.1+80,100);
		}
	}
	else
	{
		playerExtraJumpsLeft = playerHasDoubleJump;
		player.z += (heightWanted-player.z)*0.5*time_step;
		player.PLAYER_SPEED_Z = 0;
		player.PLAYER_GROUND_CONTACT = 1;
        if(input_hit(INPUT_JUMP) && !playerJumpNotOk)
		{
			player.PLAYER_GROUND_CONTACT = 0;
			playerJumpHangtime = 6;
			player.PLAYER_SPEED_Z = 80;
			switch(integer(random(3)))
			{
				case 0: snd_play(player_snd_jump1, 100, 0); break;
				case 1: snd_play(player_snd_jump2, 100, 0); break;
				case 2: snd_play(player_snd_jump3, 100, 0); break;
			}
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
    camera.roll += (-(input_down(INPUT_LEFT)-input_down(INPUT_RIGHT))*2+sinv(playerWeaponBob*0.5)*(0.5+playerCrouchPerc*0.05)-camera.roll)*0.5*(1-0.01*playerSlidePerc)*time_step;
	
	vec_set(playerAngle,camera.pan);
	var progress = weaponGetAttackProgress();
	if(progress > 0)
	{
		var kickbackFac = weaponGetKickbackFac(progress, 30)*0.2;
		if(weapons_get_current() == WEAPON_CELLGUN) kickbackFac *= 0.25;
		var recoilSide = sinv(total_ticks*5);
		ang_rotate(camera.pan, vector(recoilSide*25*kickbackFac,30*kickbackFac,-5*recoilSide*kickbackFac));
	}
	camera.tilt += playerSlidePerc*0.1;

	if(playerHealthOld != playerHealth)
	{
		playerHealthOld = playerHealth;
		if(playerHealthOld > playerHealth) playerHealthKnockbackEffect = 0;
	}
	if(playerHealthKnockbackEffect < 100)
	{
		playerHealthKnockbackEffect = minv(playerHealthKnockbackEffect+6*time_step,100);
		var kickbackFac = weaponGetKickbackFac(playerHealthKnockbackEffect, 50)*0.5;
		var recoilSide = sinv(total_ticks*7);
		ang_rotate(camera.pan, vector(recoilSide*25*kickbackFac,30*kickbackFac,-15*recoilSide*kickbackFac));
	}

	playerLightDuration = maxv(playerLightDuration-time_step,0);
	ENTITY* entMuzzleFlashHelper = (ENTITY*)player.skill2;
	if(entMuzzleFlashHelper)
	{
		vec_set(entMuzzleFlashHelper.x,camera.x);
		entMuzzleFlashHelper.lightrange = playerLightRange*(playerLightDuration/playerLightDurationMax);
	}
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

#ifdef DEBUG_CHEATHP
void playerCheatHp_startup()
{
	while (1)
	{
		playerHealth = 999;
		wait(1);
	}
}
#endif

//////////////////////////////
// movement.c
//////////////////////////////
