#ifndef MATERIALS_H
#define MATERIALS_H

#include <mtlView.c>

BMAP *bmapPPGlow = NULL;
BMAP *bmapPPOriginal = NULL;

MATERIAL *matLevel =
{
	effect = "Shaders/walls_unlit.fx";
}

MATERIAL *matFlesh =
{
	effect = "Shaders/flesh_basic.fx";
}

MATERIAL *matDecalBlood =
{
	effect = "Shaders/decal_blood.fx";
}

MATERIAL *matDecalBasic =
{
	effect = "Shaders/decal_basic.fx";
}

MATERIAL *matPPBlurVertical =
{
	effect = "Shaders/pp_blur.fx";
}

MATERIAL *matPPBlurHorizontal =
{
	effect = "Shaders/pp_blur.fx";
}

MATERIAL *matPPCombine =
{
	effect = "Shaders/pp_combine.fx";
}

void SetupPostprocessing()
{
	matPPBlurHorizontal.skill1 = floatv(1.0);
	matPPBlurHorizontal.skill2 = floatv(0.0);
	
	matPPBlurVertical.skill1 = floatv(0.0);
	matPPBlurVertical.skill2 = floatv(1.0);
	
	pp_add(matPPBlurVertical);
	pp_add(matPPBlurHorizontal);
	pp_add(matPPCombine);
}

void SetupDefaultMaterials()
{
	fog_color = 2;
	camera.fog_start = 0.0;
	camera.fog_end = 30000.0;
	
	effect_load(mtl_model, "Shaders/walls_unlit.fx");
	effect_load(mtl_flat, "Shaders/walls_unlit.fx");
	effect_load(mtl_shaded, "Shaders/walls_unlit.fx");
}

#endif