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
	float4 worldPos : TEXCOORD1;
	float3 normal : TEXCOORD2;
	float fog : TEXCOORD4;
	float3 vcolor : TEXCOORD5;
};

struct out_frag // fragment color output
{
	float4 glow : COLOR0;
	float4 color : COLOR1;
};

out_ps vs(
	float4 inPos : POSITION,
	float3 inNormal : NORMAL,
	float2 inTexCoord0 : TEXCOORD0,
	float3 inVertColor : TEXCOORD2)
{
	out_ps Out;
	
	Out.Pos = DoTransform(inPos);
	Out.fog = (Out.Pos.z - vecFog.x) * vecFog.z;
	Out.uv0 = inTexCoord0;
	Out.worldPos = mul(inPos, matWorld);
	Out.normal = mul(inNormal, matWorld);
	Out.vcolor = inVertColor;
		
	return Out;
}

out_frag ps(out_ps In)
{
	out_frag Out;
	
	In.normal = normalize(In.normal);
	
	float4 color;
	color.rgb = tex2D(sTexture, In.uv0);
	color.a = 1.0f;
	
	float redGlow = (color.r > 0.5)? color.r : 0.0;
	//float grnGlow = (color.g > 0.5)? color.g : 0.0;
	//Out.glow.rgb = float3(redGlow * 5.0, grnGlow * 5.0, 0.0);
	In.vcolor += 1;
	float glowFactor = length(color.rgb) > 0.9? 1.0 : 0.0;
	Out.glow.rgb = glowFactor * color.rgb * In.vcolor;
	Out.glow.r = redGlow * 5.0;
	Out.glow.a = 1.0;
	
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
	
	if(glowFactor > 0.5) light = 1.0;
	color.rgb *= light;
	In.vcolor = lerp(In.vcolor,1,0.667);
	color.rgb *= In.vcolor;	Out.color = lerp(color, vecFogColor, saturate(In.fog));
	
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
