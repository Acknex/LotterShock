#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

Texture entSkin1;
Texture entSkin2;
sampler sTexture = sampler_state { Texture = <entSkin1>; MipFilter = Linear; MagFilter = Linear; MinFilter = Linear; };
sampler sLightmap = sampler_state { Texture = <entSkin2>; MipFilter = Linear; MagFilter = Linear; MinFilter = Linear; };

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
	Out.worldPos = mul(inPos, matWorld);
	Out.normal = mul(inNormal, matWorld);
	Out.vcolor = inTexCoord2;
	
	return Out;
}

float4 ps(out_ps In): COLOR
{
	In.normal = normalize(In.normal);
	float viewDistance = distance(vecViewPos.xyz - In.worldPos);
	
	float4 color;
	color.rgba = tex2D(sTexture, In.uv0);
	float glow = color.a;
	color.a = 1.0;
	
	float3 light = tex2D(sLightmap, In.uv1).rgb;
	
	float3 light = vecAmbient.rgb*0.25;
	
	for(int i = 0; i < 8; i++)
	{
		if(vecLightPos[i].w > 0)
		{
			float3 lightDir = vecLightPos[i].xyz - In.worldPos;
			float lightDistance = length(lightDir);
			float lightFactor = saturate(dot(In.normal, lightDir/lightDistance))*0.5+0.5;
			float lightAttenuation = saturate(1-lightDistance/vecLightPos[i].w);// / (lightDistance * lightDistance);
			light += 1.5*lightFactor * lightAttenuation*lightAttenuation * vecLightColor[i].rgb;
		}
	}
	
	color.rgb *= lerp(light, float3(1.0), glow);
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
