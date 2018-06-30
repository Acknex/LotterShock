Texture TargetMap;
Texture mtlSkin1;

sampler samplerBloomColor = sampler_state
{
	Texture = <TargetMap>; 
	AddressU = Clamp;
	AddressV = Clamp; 
	MipFilter = Linear;
};

sampler samplerImageColor = sampler_state
{
	Texture = <mtlSkin1>; 
	AddressU = Clamp;
	AddressV = Clamp; 
	MipFilter = Linear;
};

float4 vecSkill1;

float4 bloomMix(float2 Tex : TEXCOORD0) : COLOR0 
{
	float4 bloom = tex2D(samplerBloomColor, Tex.xy);
	float4 image = tex2D(samplerImageColor, Tex.xy);
	return image + bloom * vecSkill1.x;
}

technique PostProcess 
{
	pass p1 
	{
		AlphaBlendEnable = false;
		VertexShader = null;
		PixelShader = compile ps_2_0 bloomMix();
	}
}
