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
var playerMaxSpeedFac = 60;
var playerAccelerationFac = 0.5;
var playerWeaponBob = 0;
var playerHeightAboveGround = 64; // does not include vertical camera.z to player.z offset!
var playerCameraHeight = 32;
var playerJumpHangtime = 6;

void movement_update()
{
	if(!player)
	{
		VECTOR spawnPos,vMin,vMax;
		if(region_get("playerSpawn",1,vMin,vMax) == 0) // kein spawnareal gefunden
			return;
		vec_lerp(spawnPos,vMin,vMax,0.5); // keine lust, auf die richtigen pointer zu achten -.- wird eh nicht portiert
		player = ent_create(CUBE_MDL, spawnPos, NULL);
		set(player,INVISIBLE);
	}
	vec_set(player.min_x,vector(-32,-32,-32));
	vec_set(player.max_x,vector(32,32,48));
	
	// rotation
	
	camera.pan += -mouse_force.x*10*time_step;
	camera.pan %= 360;
	camera.tilt = clamp(camera.tilt+mouse_force.y*10*time_step,-85,85);
	player.pan = camera.pan;
	
	// movement
	
	VECTOR targetSpeed;
	vec_set(targetSpeed,vector((input[INPUT_UP].down-input[INPUT_DOWN].down*0.667),(input[INPUT_LEFT].down-input[INPUT_RIGHT].down*0.667),0));
	if(targetSpeed.x || targetSpeed.y)
	{
		vec_normalize(targetSpeed,playerMaxSpeedFac*playerSpeedFac);
		vec_rotate(targetSpeed,vector(camera.pan,0,0));
	}
	VECTOR temp;
    vec_set(temp,vector(player.PLAYER_SPEED_X,player.PLAYER_SPEED_Y,0));
	vec_lerp(temp,temp,targetSpeed,playerAccelerationFac*time_step);
    player.PLAYER_SPEED_X = temp.x;
    player.PLAYER_SPEED_Y = temp.y;
	
    var moveDist = c_move(player,nullvector,vector(player.PLAYER_SPEED_X*time_step,player.PLAYER_SPEED_Y*time_step,0),PLAYER_C_MOVE_MODE_DEFAULT);
	playerWeaponBob += moveDist; // no time_step!
	
	// gravity
	
	var trace_dist = 1024;
	var minZ = player.min_z;
	var maxZ = player.max_z;
	player.min_z = -0.25; // fake a flat collision hull (instead of an ellipsoid)
	player.max_z = 0.25;
    c_trace(player.x,vector(player.x,player.y,player.z-trace_dist),PLAYER_C_TRACE_MODE_DEFAULT);
	player.min_z = minZ;
	player.max_z = maxZ;
	
	
	if(!trace_hit) target.z = -trace_dist;
	var heightWanted = target.z+playerHeightAboveGround;
	var PLAYER_DIST_TO_GROUND = heightWanted-player.z;
	var PLAYER_PLANNED_DIST_TO_GROUND = (1+16*player.PLAYER_GROUND_CONTACT)*time_step;

	var fac = 1;
	if(!input[INPUT_JUMP].down)
		playerJumpHangtime = 0;
	if(playerJumpHangtime > 0) 
		fac = 0.667;
	var EFFECT_OF_GRAVITY = -fac*12*playerSpeedFac*time_step;
	player.PLAYER_SPEED_Z = maxv(player.PLAYER_SPEED_Z+EFFECT_OF_GRAVITY,-60);
	
	if(PLAYER_DIST_TO_GROUND > PLAYER_PLANNED_DIST_TO_GROUND || player.PLAYER_SPEED_Z > 0)
	{
		player.PLAYER_GROUND_CONTACT = 0;
		playerJumpHangtime = maxv(playerJumpHangtime-time_step,0);
	}
	else
	{
		player.PLAYER_GROUND_CONTACT = 1;
		if(input[INPUT_JUMP].justPressed)
		{
			player.PLAYER_GROUND_CONTACT = 0;
			playerJumpHangtime = 6;
			player.PLAYER_SPEED_Z = 60;
		}
	}
	
	c_move(player,nullvector,vector(0,0,maxv(player.PLAYER_SPEED_Z*time_step,heightWanted-player.z)),PLAYER_C_MOVE_MODE_DEFAULT);
	
	// camera
	
	camera.x = player.x;
	camera.y = player.y;
	camera.z = player.z+playerCameraHeight;
}

//////////////////////////////
// movement.c
//////////////////////////////
