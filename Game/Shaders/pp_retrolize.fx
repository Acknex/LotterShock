Texture TargetMap;

sampler smpSource = sampler_state
{
        Texture = <TargetMap>;
        AddressU = Clamp;
        AddressV = Clamp;
        MipFilter = Linear;
};

float4 vecViewPort;
float4 vecSkill1;
float4 vecTime;

float materials_matrix_str_var;

const float tileSize = 8.0;

float2 align(float2 val)
{
    return tileSize * floor((1.0 / tileSize) * val);
}

float4 pp_matrix(float2 Tex : TEXCOORD0) : COLOR0
{
    float2 uv = align(vecViewPort.xy * Tex) + 0.5 * tileSize;

    return tex2D(smpSource, vecViewPort.zw * uv);
}

technique PostProcess
{
        pass p1
        {
                AlphaBlendEnable = false;
                VertexShader = null;
                PixelShader = compile ps_3_0 pp_matrix();
        }
}
