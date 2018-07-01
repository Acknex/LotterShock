#include <define>
#include <transform>
#include <sun>
#include <lights>
#include <fog>
#include <normal>

float4 vecAmbient;
float4 vecFogColor;

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
	Out.worldPos = mul(inPos, matWorld);
	Out.normal = mul(inNormal, matWorld);
	
	return Out;
}

out_frag ps(out_ps In)
{
	In.normal = normalize(In.normal);
	
	float4 color;
	color.rgb = tex2D(sTexture, In.uv0);
	color.a = 1.0;
	
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
	
	color.rgb *= light;
	color = lerp(color, vecFogColor, saturate(In.fog));
	
	out_frag Out;
	Out.color = color;
	Out.glow = 0.0;
	
	return Out;
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
