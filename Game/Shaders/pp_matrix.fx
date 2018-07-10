Texture TargetMap;

Texture materials_matrix_tex_bmap;
Texture materials_noise_tex_bmap;
Texture materials_guard_tex_bmap;

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
        AddressU = Wrap;
        AddressV = Wrap;
        MipFilter = Point;
};

sampler smpNoise = sampler_state
{
        Texture = <materials_noise_tex_bmap>;
        AddressU = Wrap;
        AddressV = Wrap;
        MipFilter = Point;
};

sampler smpGuard = sampler_state
{
        Texture = <materials_guard_tex_bmap>;
        AddressU = Clamp;
        AddressV = Clamp;
        MipFilter = Point;
};

float4 vecViewPort;
float4 vecSkill1;
float4 vecTime;

float materials_matrix_str_var;

const float tileSize = 16.0;

float2 align(float2 val)
{
    return tileSize * floor((1.0 / tileSize) * val);
}

float4 pp_matrix(float2 Tex : TEXCOORD0) : COLOR0
{
    float2 guard_topleft = vecViewPort.zw * align((vecViewPort.xy - float2(464, 656)) / 2.0);
    float2 guard_size = vecViewPort.zw * float2(464, 656);

    float2 tileUV = fmod(Tex * vecViewPort.xy, tileSize) / tileSize;

    float2 uv = align(Tex * vecViewPort.xy) * vecViewPort.zw;

    float4 noise = tex2D(smpNoise, uv);

    float4 color = tex2D(g_samSrcColor, Tex);

    color = lerp(color, float4(0.0, 0.0, 0.0, 1.0), clamp(materials_matrix_str_var, 0.0, 1.0));

    if(noise.z < materials_matrix_str_var)
    {
        float4 tile = tex2D(smpMatrix, 0.125 * floor(7.0 * noise.xy + floor(3.0 * materials_matrix_str_var - noise.z)) + 0.125 * tileUV);

        color = lerp(color, tile, tile.g);
    }

    if(materials_matrix_str_var >= 1.4)
    {
        // sample some other noise
        noise = tex2D(smpNoise, uv + float2(0.3, 0.7));

        if(noise.z < (materials_matrix_str_var - 1.4))
        {
            float4 guard = tex2D(smpGuard, clamp((Tex - guard_topleft) / guard_size, 0.0, 1.0));

            color = lerp(color, guard, guard.a);
        }
    }

    return color;
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
