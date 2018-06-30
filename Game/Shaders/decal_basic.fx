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
	viewDirection = normalize(viewDirection);
	
	float4 color;
	color = tex2D(sTexture, In.uv0);
	
	float3 light = vecAmbient.rgb;
	float3 specular = 0.0;
	
	for(int i = 0; i < 8; i++)
	{
		float3 lightDirection = vecLightPos[i].xyz - In.worldPos;
		float lightDistance = length(lightDirection);
		float lightFactor = saturate(dot(In.normal, lightDirection/lightDistance));
		float lightAttenuation = saturate(1.0 - lightDistance/vecLightPos[i].w);
		lightAttenuation *= lightAttenuation;
		
		float3 lightHalf = normalize(viewDirection + lightDirection);
		float lightSpecularFactor = saturate(dot(lightHalf, In.normal));
		lightSpecularFactor = pow(lightSpecularFactor, 10.0);
		
		light += lightFactor * lightAttenuation * vecLightColor[i].rgb;
		specular += lightSpecularFactor * vecLightColor[i].rgb * color.r;
	}
	
	color.rgb *= light;
	color.rgb += specular;
	
	float fogAttenuation = max(viewDistance - vecFog.x, 0.0) * vecFog.z;
	color.rgb = lerp(color.rgb, vecFogColor.rgb, fogAttenuation);
	
	Out.color = color;
	Out.color.a *= fAlpha;
	
	Out.glow = 0.0;
	
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
