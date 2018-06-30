#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

float4 vecAmbient;
Texture entSkin1;
sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Point; MagFilter = Point; MinFilter = Point; };

struct out_ps // Output to the pixelshader fragment
{
	float4 Pos : POSITION;
	float2 uv0 : TEXCOORD0;
};

struct out_frag
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
	
	return Out;
}

out_frag ps(out_ps In)
{
	float4 color;
	color = tex2D(sTexture, In.uv0);
	//color.rgb *= vecAmbient.rgb;
	
	out_frag Out;
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
	}
}
