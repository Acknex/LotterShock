//////////////////////////////
// projectiles.h
//////////////////////////////

#ifndef PROJECTILES_H
	#define PROJECTILES_H

#include "dmgsys.h"
#include "materials.h"

	typedef struct PROJECTILE
	{
		int type;
		int friendly;
		int bounces;
		int ID;
		ENTITY* ent;
		VECTOR pos,prevPos,speed;
		var lifetime;
                var dmg;
                ENTITY * source;
		struct PROJECTILE *next;
	} PROJECTILE;

	PROJECTILE *projectileList = NULL;

	//////////////////////////////

	#define PROJECTILE_TYPE_SHOTGUN 0
	#define PROJECTILE_TYPE_CELL 1

	#define PROJECTILE_C_TRACE_MODE_DEFAULT (IGNORE_PASSABLE | IGNORE_SPRITES | IGNORE_PUSH | IGNORE_CONTENT)

	//////////////////////////////

	BMAP* projBounce1_bmp = "projBounce1.tga";
	BMAP* projBounce2_bmp = "projBounce2.tga";
	BMAP* bulletHoleCool_bmp = "bulletHoleCool.tga";
	BMAP* smokeSprite1_bmp = "smokeSprite1.tga";

	//////////////////////////////


	void projectiles_close()
	{
		PROJECTILE *proj = projectileList;
		while(proj)
		{
			PROJECTILE *next = proj->next;
			sys_free(proj);
			proj = next;	
		}
		projectileList = NULL;
	}

        PROJECTILE * projectileCreate(int type, int friendly, VECTOR* pos, VECTOR* speed)
	{
		PROJECTILE *projNew = (PROJECTILE*)sys_malloc(sizeof(PROJECTILE));
                memset(projNew, 0, sizeof(PROJECTILE));

		projNew->next = projectileList;
		projectileList = projNew; 
		projNew->type = type;
		projNew->friendly = friendly;
		vec_set(projNew->pos,pos);
		vec_set(projNew->prevPos,pos);
		vec_set(projNew->speed,speed);
		projNew->ent = NULL;
		projNew->bounces = 0;
		static int ID = 0;
		projNew->ID = ID++;
		
		switch(type)
		{
                    case PROJECTILE_TYPE_SHOTGUN:
			projNew->bounces = 1;
			projNew->lifetime = 16;
			projNew->friendly = 1;
			break;

                    case PROJECTILE_TYPE_CELL:
                        projNew->bounces = 0;
			projNew->lifetime = 48;
			projNew->friendly = 1;
			break;
		}
                return projNew;
	}
	
	void p_projectile_shotgun(PARTICLE* p)
	{
		p.bmap = NULL;
		set(p,STREAK | BRIGHT);
		//set(p,BRIGHT);
		p.alpha = 200;
		vec_set(p.blue,vector(20,190,255));
		p.size = 24;
		p.lifespan = 0.01;
		p.event = NULL;
	}

	void p_projectile_cell(PARTICLE* p)
	{
		p.bmap = NULL;
		set(p,BRIGHT);
		//set(p,BRIGHT);
		p.alpha = 200;
		vec_set(p.blue,vector(255,100,10));
		p.size = 90+random(8);
		p.lifespan = 0.01;
		p.event = NULL;
	}
	
	void p_bullet_impact_smoke_fade(PARTICLE* p)
	{
		p.vel_x *= 1-0.35*time_step;
		p.vel_y *= 1-0.35*time_step;
		p.alpha -= 0.5*time_step;
		p.size += time_step;
		if(p.alpha <= 0) p.lifespan = 0;
	}
	
	void p_bullet_impact_smoke(PARTICLE* p)
	{
		p.bmap = smokeSprite1_bmp;
		set(p,MOVE);
		p.vel_z *= 0.5;
		//p.gravity = -0.2;
		p.alpha = 18+random(5);
		vec_fill(p.blue,90+random(30));
		p.size = 50+random(15);
		p.event = p_bullet_impact_smoke_fade;
	}
	
	
	void p_projBounce_fade_event(PARTICLE* p)
	{
		var blubb = p.lifespan/p.skill_a*100;
		if(blubb > 100)
		{
			blubb = 100;
			var blubb2 = maxv((p.lifespan - p.skill_b) * 4.0, 0);
			if(blubb2 <= 100)
				blubb += blubb2;
		}
		p.alpha = blubb;
		//vec_fill(p.blue,255*p.lifespan/p.skill_a); // bringt nichts
	}

	void p_decal_setup_fade(PARTICLE* p, var lifespan, var fadeStart)
	{
		if(!p) return; // ent_decal kann failen!
		set(p,TRANSLUCENT);
		p.event = p_projBounce_fade_event;
		p.lifespan = lifespan;
		p.skill_a = fadeStart;
		p.skill_b = lifespan - 25;
	}

	void projectiles_update()
	{
		VECTOR temp;
		PROJECTILE *prev = NULL;
		PROJECTILE *proj = projectileList;
		while(proj)
		{
			vec_set(proj->prevPos,proj->pos);
			vec_set(temp,proj->speed);
			vec_scale(temp,time_step);
			vec_add(proj->pos,temp);
			//proj->speed.z -= time_step;
                        if(proj->friendly && player)
                            set(player,PASSABLE);

                        if(proj->friendly)
                            dmgsys_set_src(DMGSYS_PLAYER, proj->source, proj->dmg);
                        else
                            dmgsys_set_src(DMGSYS_ENEMY, proj->source, proj->dmg);
                        c_trace(proj->prevPos,proj->pos,PROJECTILE_C_TRACE_MODE_DEFAULT | USE_POLYGON | SCAN_TEXTURE | ACTIVATE_SHOOT);
			if(proj->friendly && player) reset(player,PASSABLE);
			if(trace_hit)
			{
				vec_set(proj->pos,target);
				if(proj->bounces > 0)
				{
					proj->friendly = 0;
					var size = 12+random(4);
					BMAP* bmp = projBounce1_bmp;
					if(proj->type == PROJECTILE_TYPE_CELL)
					{
						bmp = projBounce2_bmp;
						size = 32+random(8);
					}
					PARTICLE* p = ent_decal(you, bmp, size, random(360));
					if(p)
					{
						p->material = matDecalGlow;
					}
					p_decal_setup_fade(p, 16+random(6), 12);
					vec_add(proj->pos,normal);
					vec_set(bounce,normal);
					vec_scale(bounce,-2*vec_dot(proj->speed,normal));
					vec_add(proj->speed,bounce);
					vec_rotate(proj->speed,vector(random(6)-3,random(6)-3,0));
					proj->bounces--;
				}
				else
				{
					proj->lifetime = 0;
					BMAP* bmp = projBounce1_bmp;
					if(proj->type == PROJECTILE_TYPE_CELL)
					{
						bmp = projBounce2_bmp;
					}
					PARTICLE* p = ent_decal(you, bmp, 9+random(2), 0);
					if(p)
					{
						matDecalBullethole.skin1 = bulletHoleCool_bmp;
						p->material = matDecalBullethole;
					}
					p_decal_setup_fade(p, 120+random(10), 10);
					effect(p_bullet_impact_smoke,1+(random(2) > 1),target,normal);
				}
			}

			EVENT pfnc = NULL;
			if(proj->type == 0) pfnc = p_projectile_shotgun;
			if(proj->type == 1) pfnc = p_projectile_cell;
			if(pfnc) effect(pfnc,1,proj->pos,vector(proj->pos.x-proj->prevPos.x,proj->pos.y-proj->prevPos.y,proj->pos.z-proj->prevPos.z));

			PROJECTILE *next = proj->next;
			proj->lifetime -= time_step;
			if(proj->lifetime < 0)
			{
				if(projectileList == proj) projectileList = NULL;
				if(prev) prev->next = next;
				sys_free(proj);
				proj = NULL;
			}
			if(proj) prev = proj;
			proj = next;	
		}
	}

#endif





//////////////////////////////
// projectiles.h
//////////////////////////////
