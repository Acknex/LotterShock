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

float4 desync(float2 Tex : TEXCOORD0) : COLOR0 
{
	float4 color = 0.0;
	color.r = tex2D(g_samSrcColor, Tex.xy + float2(-0.727, -0.727) * vecSkill1 * vecViewPort.zw).r;
	color.g = tex2D(g_samSrcColor, Tex.xy + float2(0.727, -0.727) * vecSkill1 * vecViewPort.zw).g;
	color.b = tex2D(g_samSrcColor, Tex.xy + float2(0.0, 1.0) * vecSkill1 * vecViewPort.zw).b;
	color.a = 1;
	
	return color;
}

technique PostProcess 
{
	pass p1 
	{
		AlphaBlendEnable = false;
		VertexShader = null;
		PixelShader = compile ps_3_0 desync();
	}
}