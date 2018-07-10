Texture TargetMap;

sampler smpSource = sampler_state
{
    Texture = <TargetMap>;
    AddressU = Border;
    AddressV = Border;
    MipFilter = Linear;
};

float4 vecViewPort;
float materials_crt_str_var;

float4 pp_crt(float2 Tex : TEXCOORD0) : COLOR0
{
    float2 center = 0.5 * vecViewPort.xy * vecViewPort.zw;

    float2 xy = Tex - center;

    xy.y *= 1.0 + pow(materials_crt_str_var, 8.0) * abs(xy.x);

    float4 color = tex2D(smpSource, center + xy);
    color.rgb += color.a * 0.3 * materials_crt_str_var;
    return color;
}

technique PostProcess
{
        pass p1
        {
                AlphaBlendEnable = false;
                VertexShader = null;
                PixelShader = compile ps_3_0 pp_crt();
        }
}
