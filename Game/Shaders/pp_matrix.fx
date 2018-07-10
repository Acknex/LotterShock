Texture TargetMap;

Texture materials_matrix_tex_bmap;
Texture materials_noise_tex_bmap;

sampler g_samSrcColor = sampler_state
{
        Texture = <TargetMap>;
        AddressU = Clamp;
        AddressV = Clamp;
        MipFilter = Linear;
};

sampler smpMatrix = sampler_state
{
        Texture = <materials_matrix_tex_bmap>;
        AddressU = Clamp;
        AddressV = Clamp;
        MipFilter = Point;
};

sampler smpNoise = sampler_state
{
        Texture = <materials_noise_tex_bmap>;
        AddressU = Wrap;
        AddressV = Wrap;
        MipFilter = Point;
};

float4 vecViewPort;
float4 vecSkill1;

float materials_matrix_str_var;

const float tileSize = 16.0;

float4 pp_matrix(float2 Tex : TEXCOORD0) : COLOR0
{
    float2 tileUV = fmod(Tex * vecViewPort.xy, tileSize) / tileSize;

    float2 uv = tileSize * floor((1.0 / tileSize) * Tex * vecViewPort.xy) * vecViewPort.zw;

    float4 noise = tex2D(smpNoise, uv);

    if(noise.z < materials_matrix_str_var)
    {
        float4 tile = tex2D(smpMatrix, 0.125 * floor(7.0 * noise.xy) + 0.125 * tileUV);

        return tile;
    }
    else
    {
        float4 color = tex2D(g_samSrcColor, Tex);

        return color;
    }
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
