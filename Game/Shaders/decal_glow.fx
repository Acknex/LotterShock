#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

float4 vecAmbient;
float4 vecFogColor;
float fAlpha;

Texture entSkin1;
sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Point; MagFilter = Point; MinFilter = Point; };

struct out_ps // Output to the pixelshader fragment
{
	float4 Pos : POSITION;
	float2 uv0 : TEXCOORD0;
	float4 worldPos : TEXCOORD1;
};

struct out_frag // fragment color output
{
	float4 glow : COLOR0;
	float4 color : COLOR1;
};

out_ps vs(
	float4 inPos : POSITION,
	float2 inTexCoord0 : TEXCOORD0)
{
	out_ps Out;
	
	Out.Pos = DoTransform(inPos);
	Out.uv0 = inTexCoord0;
	Out.worldPos = mul(matWorld, float4(inPos.xyz, 1.0));
	
	return Out;
}

out_frag ps(out_ps In)
{
	out_frag Out;
	
	float4 color;
	color = tex2D(sTexture, In.uv0);

	float viewDistance = distance(vecViewPos.xyz, In.worldPos.xyz);
	float fogAttenuation = max(viewDistance - vecFog.x, 0.0) * vecFog.z;
	color.rgb = lerp(color.rgb, vecFogColor.rgb, fogAttenuation);
	
	color.a *= fAlpha;
	Out.color = color;
	Out.glow = color;
	
	return Out;
}


technique object
{
	pass one
	{
		VertexShader = compile vs_3_0 vs();
		PixelShader = compile ps_3_0 ps();
		AlphaBlendEnable = true;
		ZWriteEnable = false;
	}
}
