#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

float4x4 matWorldInv;
float4 vecAmbient;
float4 vecFogColor;
float fAlpha;

Texture entSkin1;
samplerCUBE sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Linear; MagFilter = Linear; MinFilter = Linear; };

struct out_ps // Output to the pixelshader fragment
{
	float4 Pos : POSITION;
	float3 uv0 : TEXCOORD0;
};

struct out_frag // fragment color output
{
	float4 glow : COLOR0;
	float4 color : COLOR1;
};

out_ps vs(
	float4 inPos : POSITION,
	float3 inTexCoord0 : TEXCOORD0)
{
	out_ps Out;
	
	Out.Pos = DoTransform(inPos);
	Out.uv0 = mul(inPos, matWorld);
	
	return Out;
}

out_frag ps(out_ps In)
{
	out_frag Out;
	
	Out.color.rgb = texCUBE(sTexture, normalize(In.uv0));
	Out.color.a = 1.0;
	Out.glow = 0.0;
	
	return Out;
}


technique object
{
	pass one
	{
		VertexShader = compile vs_3_0 vs();
		PixelShader = compile ps_3_0 ps();
	}
}
