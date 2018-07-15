Texture TargetMap;
Texture mtlSkin1;

sampler samplerImageColor = sampler_state
{
    Texture = <mtlSkin1>;
    AddressU = Clamp;
    AddressV = Clamp;
    MipFilter = Linear;
};

float4 vecSkill1;

float4 bloomMix(float2 Tex : TEXCOORD0) : COLOR0
{
    return tex2D(samplerImageColor, Tex.xy);
}

technique PostProcess
{
    pass p1
    {
        AlphaBlendEnable = false;
        VertexShader = null;
        PixelShader = compile ps_2_0 bloomMix();
    }
}
