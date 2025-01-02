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
    return input.texcoord.x > xSplit ? float4(0.0, 0.0, 1.0, 1.0) : float4(1.0, 0.0, 0.0, 1.0f);
}