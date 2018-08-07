#ifndef _GHOST_C_
#define _GHOST_C_

#include <acknex.h>

void trail_action(void)
{
	set(me, TRANSLUCENT);
	my->alpha = 50;
	
	var morph_speed = 4;
	
	random_seed(0);
	COLOR colours;
	vec_set(&colours, _vec(random(360), random(360), random(360)));
	
	my->material = mtl_create();
	my->material->effect = "
float4 vecTime;
float4 vecAmbient;
float4 ps(float2 tex : TEXCOORD0) : COLOR0
{
	float3 color = 0.75 + 0.25 * sin(0.25 * vecTime.w + 45 * tex.y);
	
	return float4(color * vecAmbient.rgb, tex.y);
}
  
technique alpha_test
{
	pass p0
	{
		PixelShader = compile ps_2_0 ps();
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}";
	
	while (me)
	{
		colours.red = (colours.red + time_step * morph_speed)%360;
		colours.green = (colours.green + time_step * morph_speed)%360;
		colours.blue = (colours.blue + time_step * morph_speed)%360;
		
		my->material->ambient_red = abs(sinv(colours.red)*255);
		my->material->ambient_green = abs(sinv(colours.green)*255);
		my->material->ambient_blue = abs(sinv(colours.blue)*255);
	
		wait(1);
	}
}

void warghost_action(void)
{
	my->material = mtl_create();
	my->material->effect = "
float4x4 matWorldViewProj;
float4 vecTime;
struct vsOut
{
	float4 outPos : POSITION;
	float2 outTex : TEXCOORD0;
	float4 outInfo : TEXCOORD1;
};

vsOut animTree_VS(
	uniform float scale,
	in float4 inPos: 	POSITION, 
	in float3 inNormal :	NORMAL,
	in float2 inTex: 	TEXCOORD0)
{
	vsOut Out;
	
	inPos.x += 0.2 * sin(vecTime.w + inPos.y);
	inPos.z += 0.5 * cos(vecTime.w + inPos.y);
	
	inPos.xyz += inNormal * scale;
	
	Out.outInfo = inPos;
	
	Out.outTex = inTex;
	Out.outPos = mul(inPos, matWorldViewProj);

	return Out;		
}

float4 ps(float2 tex : TEXCOORD0, float2 pos : TEXCOORD1) : COLOR0
{
	float3 color = 0.75 + 0.25 * sin(-vecTime.w + 1 * pos.y);
	
	return 0.2 * float4(color, 1) * saturate(0.125 * (pos.y + 20));
}
  
    technique alpha_test
    {
      pass p0
      {
		VertexShader = compile vs_2_0 animTree_VS(0.0);
		AlphaBlendEnable = true;
		//SrcBlend = One;
		//DestBlend = One;
      }
      pass p0
      {
		VertexShader = compile vs_2_0 animTree_VS(1.0);
		PixelShader = compile ps_2_0 ps();
		AlphaBlendEnable = true;
		SrcBlend = One;
		DestBlend = One;
      }
    }
  ";
	
	while (me)
	{
		ent_animate(me, "attack", my->skill[0], ANM_CYCLE);
		my->skill[0] += time_step * 3;
		//vec_diff(my.pan, my.x, camera.x); //removed by firo
		//vec_to_angle(my.pan, my.pan);//removed by firo
		my.pan = -90 + 25*sinv(total_ticks * 5); //temporary - firo
		my.tilt = 0;
		my.roll = 0;
		wait(1);
	}
}

void bottle_action(void)
{
	ENTITY* trail = ent_create("trail.mdl", my.x, trail_action);
	vec_set(trail.pan, my.pan);
	vec_set(trail.scale_x, my.scale_x);
	set(trail, PASSABLE);
	
	ENTITY* ghost = ent_create("warghost.mdl", my.x, warghost_action);
	vec_set(ghost.pan, my.pan);
	vec_set(ghost.scale_x, my.scale_x);
	vec_for_vertex(&(ghost->x), trail, 706);
	ghost->z += 12*ghost.scale_z;
	set(ghost, PASSABLE);
}

#endif