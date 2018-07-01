const float4x4 matWorldViewProj; // World*view*projection matrix. 
const float4x4 matWorld; // World matrix. 
const float4x4 matWorldView;
const float4 vecViewDir;  // The sun direction vector.
float4 vecViewPos;
float4 vecSkill41;
float4 vecSkill1;
float4 vecTime;
float fAmbient;
//float3 vRotatedDirection_var;
float4x4 rotateMatrix_flt;

texture entSkin1; // Model texture
texture skyCute_cubemap_bmap;

sampler ColorMapSampler = sampler_state 
{ 
	Texture = <entSkin1>; 
	AddressU  = Wrap; 
	AddressV  = Wrap; 
}; 

samplerCUBE skyMapSampler = sampler_state 
{ 
	Texture = <skyCute_cubemap_bmap>; 
	AddressU  = Wrap; 
	AddressV  = Wrap; 
}; 

void DiffuseVS( 
in float4 InPos: POSITION, 
in float3 InNormal: NORMAL, 
in float2 InTex: TEXCOORD0, 
out float4 OutPos: POSITION, 
out float2 OutTex: TEXCOORD0, 
out float3 OutNormal: TEXCOORD1,
out float3 OutPos2: TEXCOORD2) 
{ 
	OutPos = mul(InPos, matWorldViewProj); 
	//OutNormal = normalize(mul(InNormal, matWorldView));
	OutNormal = normalize(mul(InNormal, matWorld));
	OutTex = InTex; 
	OutPos2 = mul(InNormal, rotateMatrix_flt);
} 

// Pixel Shader: 
float4 DiffusePS( 
in float2 InTex: TEXCOORD0, 
in float3 InNormal: TEXCOORD1,
in float3 InPos: TEXCOORD2): COLOR 
{ 

	InNormal = normalize(InNormal);
	//float3 viewDir = normalize(InPos-vecViewPos);
	float time = vecTime.w*0.05;
	float noise = tex2D(ColorMapSampler,InTex+float2(time,time*0.3));
	noise += tex2D(ColorMapSampler,InTex+float2(time*0.17,time*0.57));
	noise = noise*0.5;
	float4 tint =float4(0.1,1,1,1);
	float4 sky = texCUBE(skyMapSampler,InPos);
	float borderFac = (1+InNormal.x);
	float4 Color = tint*borderFac*borderFac*2*noise+sky;
	
	return Color;
} 

// Technique: 
technique DiffuseTechnique 
{ 
	pass P0 
	{ 
		ZWriteEnable = true;
		AlphaBlendEnable=False;
		VertexShader = compile vs_3_0 DiffuseVS(); 
		PixelShader  = compile ps_3_0 DiffusePS(); 
	} 
} 
