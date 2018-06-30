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
sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Linear; MagFilter = Linear; MinFilter = Linear; };

struct out_ps // Output to the pixelshader fragment
{
	float4 Pos : POSITION;
	float2 uv0 : TEXCOORD0;
	float fog : TEXCOORD1;
	float3 worldPos : TEXCOORD2;
	float3 normal : TEXCOORD3;
};

struct out_frag // fragment color output
{
	float4 glow : COLOR0;
	float4 color : COLOR1;
};

out_ps vs(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0)
{
	out_ps Out;
	
	Out.Pos = DoTransform(inPos);
	Out.fog = (Out.Pos.z - vecFog.x) * vecFog.z;
	Out.uv0 = inTexCoord0;
	Out.worldPos = mul(matWorld, float4(inPos.xyz, 1.0));
	Out.normal = mul(matWorld, float4(inNormal, 0.0));
	
	return Out;
}

out_frag ps(out_ps In)
{
	out_frag Out;
	
	In.normal = normalize(In.normal);
	float3 viewDirection = vecViewPos.xyz - In.worldPos;
	float viewDistance = length(viewDirection);
	
	float4 color = tex2D(sTexture, In.uv0);
	
	float3 light = vecAmbient.rgb;
	
	for(int i = 0; i < 8; i++)
	{
		float3 lightDirection = vecLightPos[i].xyz - In.worldPos;
		float lightDistance = length(lightDirection);
		float lightFactor = saturate(dot(In.normal, lightDirection/lightDistance));
		float lightAttenuation = saturate(1.0 - lightDistance/vecLightPos[i].w);
		lightAttenuation *= lightAttenuation;
		
		light += lightFactor * lightAttenuation * vecLightColor[i].rgb;
	}
	
	color.rgb *= light;
	
	color.rgb = lerp(color.rgb, vecFogColor.rgb, saturate(In.fog));
	
	//color.a = 1.0;//smoothstep(color.a, 0.5, 1.0);
	Out.color = color;
	Out.glow = float4(0.0, 0.0, 0.0, color.a);
	
	return Out;
}


technique object
{
	pass one
	{
		VertexShader = compile vs_3_0 vs();
		PixelShader = compile ps_3_0 ps();
		
		AlphaBlendEnable = false;
		AlphaTestEnable = true;
		ZWriteEnable = true;
	}
}