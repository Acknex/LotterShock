Texture TargetMap;
sampler g_samSrcColor = sampler_state
{
	Texture = <TargetMap>; 
	AddressU = Clamp;
	AddressV = Clamp; 
	MipFilter = Linear;
};

float4 vecViewPort;
float4 vecSkill1;

//const float kernel[11] = {0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003};
const float kernel[17] =  {0.003924, 0.008962, 0.018331, 0.033585, 0.055119, 0.081029, 0.106701, 0.125858, 0.13298, 0.125858, 0.106701, 0.081029, 0.055119, 0.033585, 0.018331, 0.008962, 0.003924};

float4 blur(float2 Tex : TEXCOORD0) : COLOR0 
{
	float4 color = 0.0;

	for(int i = -8; i <= 8; i++)
	{
		color += tex2D(g_samSrcColor, Tex.xy + vecSkill1.xy * i * vecViewPort.zw) * kernel[i + 8];
	}
	
	return color;
}

technique PostProcess 
{
	pass p1 
	{
		AlphaBlendEnable = false;
		VertexShader = null;
		PixelShader = compile ps_3_0 blur();
	}
}