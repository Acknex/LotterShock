Texture TargetMap;

sampler smpSource = sampler_state
{
	Texture = <TargetMap>; 
	AddressU = Clamp;
	AddressV = Clamp; 
	MipFilter = Point;
};

float4 vecViewPort;
float4 vecSkill1;

#define viewDistance 45
#define alphaLevel 0.3

float4 reveal(float2 Tex : TEXCOORD0, float2 pixel : VPOS) : COLOR0
{
	float2 uv = Tex;
	float4 color = tex2D(smpSource, uv);
	
	float2 dist = vecSkill1.xy - uv;
   float a = length(dist * vecViewPort.xy) / viewDistance;
   if(a <= 1.0)
	{
      for(int i = 0; i <= viewDistance; i++)
      {
         float a = tex2D(smpSource, uv + dist * float(i) / float(viewDistance)).r;
         if(a <= 0.1)
         {
            return color;
         }
      }
      color.a = max(color.a, alphaLevel*pow(1.0 - saturate(4.0 * a - 3.0), 0.5));
	}
	if (color.a < 0.05) color.a = 0;
	return color;
}

technique PostProcess 
{
	pass p1 
	{
      AlphaBlendEnable = false;
      VertexShader = null;
      PixelShader = compile ps_3_0 reveal();
	}
}
