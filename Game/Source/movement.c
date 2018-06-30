//////////////////////////////
// movement.c
//////////////////////////////

//VECTOR playerSpeed;
#define speed_x skill50
#define speed_y skill51
#define speed_z skill52
//#define distToGround skill53
#define groundContact skill54

#define C_MOVE_MODE_DEFAULT IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT
#define C_TRACE_MODE_DEFAULT IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT

var playerSpeedFac = 1;
var playerMaxSpeedFac = 60;
var playerAccelerationFac = 0.5;
var playerWeaponBob = 0;
var playerHeightAboveGround = 64; // does not include vertical camera.z to player.z offset!
var playerCameraHeight = 32;
var playerJumpHangtime = 6;

void playerMove()
{
	if(!player)
	{
		VECTOR spawnPos,vMin,vMax;
		region_get("playerSpawn",1,vMin,vMax);
		vec_lerp(spawnPos,vMin,vMax,0.5); // keine lust, auf die richtigen pointer zu achten -.- wird eh nicht portiert
		player = ent_create(CUBE_MDL,spawnPos,NULL);
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
	vec_set(temp,vector(player.speed_x,player.speed_y,0));
	vec_lerp(temp,temp,targetSpeed,playerAccelerationFac*time_step);
	player.speed_x = temp.x;
	player.speed_y = temp.y;
	
	var moveDist = c_move(player,nullvector,vector(player.speed_x*time_step,player.speed_y*time_step,0),C_MOVE_MODE_DEFAULT);
	playerWeaponBob += moveDist; // no time_step!
	
	// gravity
	
	var minZ = player.min_z;
	var maxZ = player.max_z;
	player.min_z = -0.25; // fake a flat collision hull (instead of an ellipsoid)
	player.max_z = 0.25;
	c_trace(player.x,vector(player.x,player.y,player.z-1024),C_TRACE_MODE_DEFAULT);
	player.min_z = minZ;
	player.max_z = maxZ;
	
	if(!trace_hit) target.z = -99999;
	var heightWanted = target.z+playerHeightAboveGround;
	var distToGround = heightWanted-player.z;
	if(distToGround > (1+16*player.groundContact)*time_step || player.speed_z > 0)
	{
		player.groundContact = 0;
		var fac = 1;
		if(!input[INPUT_JUMP].down) playerJumpHangtime = 0;
		playerJumpHangtime = maxv(playerJumpHangtime-time_step,0);
		if(playerJumpHangtime > 0) fac = 0.667;
		player.speed_z = maxv(player.speed_z-fac*12*playerSpeedFac*time_step,-240);
	}
	else
	{
		player.z += (heightWanted-player.z)*time_step;
		player.z += (heightWanted-player.z)*time_step; // this is not a typo
		player.groundContact = 1;
		if(input[INPUT_JUMP].justPressed)
		{
			player.groundContact = 0;
			playerJumpHangtime = 6;
			player.speed_z = 60;
		}
	}
	
	c_move(player,nullvector,vector(0,0,maxv(player.speed_z*time_step,heightWanted-player.z)),C_MOVE_MODE_DEFAULT);
	
	// camera
	
	camera.x = player.x;
	camera.y = player.y;
	camera.z = player.z+playerCameraHeight;
}

//////////////////////////////
// movement.c
//////////////////////////////