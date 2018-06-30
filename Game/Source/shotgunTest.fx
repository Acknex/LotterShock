const float4x4 matWorldViewProj; // World*view*projection matrix. 
const float4x4 matWorldView;
float4 vecViewPos;
float4 vecSkill41;
float4 vecSkill1;
float fAmbient;
bool PASS_SOLID;

texture entSkin1; // Model texture

sampler ColorMapSampler = sampler_state 
{ 
	Texture = <entSkin1>; 
	AddressU  = Wrap; 
	AddressV  = Wrap; 
}; 

void DiffuseVS( 
in float4 InPos: POSITION, 
in float3 InNormal: NORMAL, 
in float2 InTex: TEXCOORD0, 
out float4 OutPos: POSITION, 
out float2 OutTex: TEXCOORD0, 
out float3 OutNormal: TEXCOORD1) 
{ 
	OutPos = mul(InPos, matWorldViewProj); 
	OutNormal = normalize(mul(InNormal, matWorldView));
	OutTex = InTex; 
} 

// Pixel Shader: 
float4 DiffusePS( 
in float2 InTex: TEXCOORD0, 
in float3 InNormal: TEXCOORD1): COLOR 
{ 
	InNormal = normalize(InNormal);
	float Diffuse = -InNormal.z*0.7+0.5;
	float4 Color = tex2D(ColorMapSampler, InTex);
	float4 final = Color*Diffuse;
	
	return (1.0+fAmbient)*final;
} 

// Technique: 
technique DiffuseTechnique 
{ 
	pass P0 
	{ 
		ZWriteEnable = true;
		AlphaBlendEnable=False;
		VertexShader = compile vs_2_0 DiffuseVS(); 
		PixelShader  = compile ps_2_0 DiffusePS(); 
	} 
} 
