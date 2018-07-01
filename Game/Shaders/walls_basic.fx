#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

Texture entSkin1;
Texture entSkin2;
sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Point; MagFilter = Point; MinFilter = Point; };
sampler sLightmap = sampler_state { Texture = <entSkin2>; MipFilter = Point; MagFilter = Point; MinFilter = Point; };

struct out_ps // Output to the pixelshader fragment
{
	float4 Pos : POSITION;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 worldPos : TEXCOORD2;
	float3 normal : TEXCOORD3;
	float3 vcolor : TEXCOORD4;
};

out_ps vs(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL
	float2 inTexCoord0 : TEXCOORD0
	float2 inTexCoord1 : TEXCOORD1,
	float3 inTexCoord2 : TEXCOORD2)
{
	out_ps Out;
	
	Out.Pos = DoTransform(inPos);
	Out.uv0 = inTexCoord0;
	Out.uv1 = inTexCoord1;
	Out.worldPos = mul(matWorld, float4(inPos.xyz, 1.0));
	Out.normal = mul(matWorld, float4(inNormal, 1.0));
	Out.vcolor = inTexCoord2;
	
	return Out;
}

float4 ps(out_ps In): COLOR
{
	In.normal = normalize(In.normal);
	float viewDistance = distance(vecViewPos.xyz - In.worldPos);
	
	float4 color;
	color.rgb = tex2D(sTexture, In.uv0);
	color.a = 1.0f;
	
	float3 light = tex2D(sLightmap, In.uv1).rgb;
	
	for(int i = 0; i < 8; i++)
	{
		float3 lightDir = vecLightPos[i].xyz - In.worldPos;
		float lightDistance = length(lightDir);
		float lightFactor = saturate(dot(In.normal, lightDir/lightDistance));
		float lightAttenuation = vecLightPos[i].w / (lightDistance * lightDistance);
		
		light += lightFactor * lightAttenuation * vecLightColor[i].rgb;
	}
	
	color.rgb *= light;
	color.rgb *= In.vcolor/128.0;
	float fogAttenuation = max(viewDistance - vecFog.x, 0.0) * vecFog.z;
	
	color = lerp(color, vecFogColor, fogAttenuation);
	
	
	return color;
}


technique object
{
	pass one
	{
		VertexShader = compile vs_3_0 vs();
		PixelShader = compile ps_3_0 ps();
		AlphaBlendEnable = false;
		ZWriteEnable = true;
	}
}
