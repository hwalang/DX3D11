Texture2D g_texture0 : register(t0); // wood box
Texture2D g_texture1 : register(t1); // stone
SamplerState g_sampler : register(s0);

cbuffer PixelShaderConstantBuffer : register(b0)
{
    float xSplit;
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    // blue or red
    // return input.texcoord.x > xSplit ? float4(0.0, 0.0, 1.0, 1.0) : float4(1.0, 0.0, 0.0, 1.0f);
    // return g_texture0.Sample(g_sampler, input.texcoord);
    return input.texcoord.x > xSplit ?
    g_texture0.Sample(g_sampler, input.texcoord)
    : g_texture1.Sample(g_sampler, input.texcoord);
    // return g_texture0.Sample(g_sampler, input.texcoord + float2(xSplit, 0.0));
}