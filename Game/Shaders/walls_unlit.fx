#include <define>
#include <transform>
//#include <sun>
//#include <lights>
#include <fog>
//#include <normal>

float4 vecAmbient;
float4 vecFogColor;
float4 vecLightPos[8];
float4 vecLightColor[8];

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
	Out.normal = mul(inNormal, (float3x3)matWorld);
	Out.vcolor = inVertColor;
	
	return Out;
}

out_frag ps(out_ps In)
{
	out_frag Out;
	
	In.normal = normalize(In.normal);
	
	float4 color = tex2D(sTexture, In.uv0);
	
	In.vcolor += 1;
	float glowFactor = saturate(color.b - color.r) * 5.0;
	Out.glow.rgb = glowFactor * color.rgb*In.vcolor;
	Out.glow.a = 1.0;
	
	//float3 light = In.worldPos/512.0; //vecAmbient.rgb; cool!
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
	//In.vcolor = lerp(float3(128,128,128),In.vcolor,saturate(glowFactor));
	//color.rgb *= In.vcolor/128.0;
	color.rgb = lerp(color.rgb,In.vcolor,saturate(glowFactor));
	In.vcolor = lerp(In.vcolor,1,0.667);
	color.rgb *= In.vcolor;
	Out.color = lerp(color, vecFogColor, saturate(In.fog));
	//Out.color.rgb = In.vcolor/255.0;
	//Out.color *= glowFactor;
	
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
