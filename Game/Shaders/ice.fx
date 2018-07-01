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
	Out.worldPos = mul(inPos, matWorld);
	Out.normal = mul(inNormal, matWorld);
	
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
	
	float3 specular = 0.0;
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
			
			float3 lightHalf = normalize(viewDirection + lightDir/lightDistance);
			float lightSpecularFactor = saturate(dot(lightHalf, In.normal));
			lightSpecularFactor = pow(lightSpecularFactor, 30.0);
			specular += lightSpecularFactor * vecLightColor[i].rgb * color.r;
		}
	}
	
	color.rgb *= light;
	color.rgb += specular;
	
	color.rgb = lerp(color.rgb, vecFogColor.rgb, saturate(In.fog));
	
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
	}
}
