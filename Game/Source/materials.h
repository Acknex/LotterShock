#ifndef MATERIALS_H
#define MATERIALS_H

#define PRAGMA_BIND "default.fx"
#include <mtlView.c>

BMAP *bmapPPOriginal = NULL;

BMAP * materials_matrix_tex = "matrix_font.png";
BMAP * materials_noise_tex = "matrix_noise.png";
BMAP * materials_guard_tex = "matrix_guard.png";

var materials_matrix_str = 0.0;

var materials_crt_str = 0.0;

MATERIAL *matLevel =
{
	effect = "walls_unlit.fx";
}

MATERIAL *matObject =
{
	effect = "object_basic.fx";
}

MATERIAL *matSprite =
{
	effect = "object_sprite.fx";
}

BMAP * matNoiseSpriteTexture = "best_noise_10.tga";

MATERIAL *matNoiseSprite =
{
    effect = "object_noisesprite.fx";
    skin1 = matNoiseSpriteTexture;
}

MATERIAL *matTree =
{
	effect = "tree.fx";
}

MATERIAL *matIce =
{
	effect = "ice.fx";
}

MATERIAL *matSkull =
{
	effect = "enemy_skull.fx";
}

MATERIAL *matTurret =
{
	effect = "turret.fx";
}

MATERIAL *matWeaponBasic =
{
	effect = "weapon_basic.fx";
}

MATERIAL *matFlesh =
{
	effect = "flesh_basic.fx";
}

MATERIAL *matIce =
{
	effect = "ice.fx";
}

MATERIAL *matDecalBlood =
{
	effect = "decal_blood.fx";
}

MATERIAL *matDecalBasic =
{
	effect = "decal_basic.fx";
}


MATERIAL *matSplatBlood =
{
	effect = "splat_blood.fx";
}

MATERIAL *matDecalGlow =
{
	effect = "decal_glow.fx";
}

MATERIAL *matDecalBullethole =
{
	effect = "decal_bullethole.fx";
}

MATERIAL *matPPBlurVertical =
{
	effect = "pp_blur.fx";
}

MATERIAL *matPPBlurHorizontal =
{
	effect = "pp_blur.fx";
}

MATERIAL *matPPCombine =
{
	effect = "pp_combine.fx";
}

MATERIAL *matPPDesync =
{
	effect = "desync.fx";
}

MATERIAL *matPPMatrix =
{
    effect = "pp_matrix.fx";
    flags = AUTORELOAD;
}

MATERIAL *matPPCrt =
{
    effect = "pp_crt.fx";
    flags = AUTORELOAD;
}

void pp_desync(var strength, var contrast)
{
	matPPDesync.skill1 = floatv(strength);
	matPPDesync.skill2 = floatv(contrast);
}

void UpdateRenderTargets()
{
	if(bmapPPOriginal)
	{
		if(bmap_width(bmapPPOriginal) == screen_size.x && bmap_height(bmapPPOriginal) == screen_size.y)
		{
			return;
		}
	}
	
	if(bmapPPOriginal)
		bmap_remove(bmapPPOriginal);
		
	bmapPPOriginal = bmap_createblack(screen_size.x, screen_size.y, 8888);
	
	camera.target1 = bmapPPOriginal;
    matPPCombine.skin1 = bmapPPOriginal;

    VIEW * it = camera;
    while(it != NULL)
    {
        it->size_x = screen_size.x;
        it->size_y = screen_size.y;
        it = it->stage;
    }
}

void SetupPostprocessing()
{
	UpdateRenderTargets();
	
	matPPBlurHorizontal.skill1 = floatv(2.0);
	matPPBlurHorizontal.skill2 = floatv(0.0);
	
	matPPBlurVertical.skill1 = floatv(0.0);
	matPPBlurVertical.skill2 = floatv(2.0);
	
	matPPCombine.skill1 = floatv(1.0);
	
	pp_add(matPPBlurVertical);
	pp_add(matPPBlurHorizontal);
	pp_add(matPPCombine);
	pp_add(matPPDesync);
    pp_add(matPPMatrix);
    pp_add(matPPCrt);
}

void SetupDefaultMaterials()
{
	fog_color = 2;
	camera.fog_start = 0.0;
	camera.fog_end = 20000.0;
	
	effect_load(mtl_model, "walls_unlit.fx");
	effect_load(mtl_flat, "walls_unlit.fx");
	effect_load(mtl_shaded, "walls_unlit.fx");
	effect_load(mtl_sky, "sky.fx");
}

#endif
