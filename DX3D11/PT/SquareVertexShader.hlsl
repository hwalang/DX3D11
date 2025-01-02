cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VertexInput
{
    float3 pos : POSITION;
    float3 color : COLOR0;
    float2 texcoord : TEXCOORD0; // pixel shader로 넘겨주기 위해서 vertex shader에서 texcoord를 받는다.
};

struct PixelInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

PixelInput main(VertexInput input)
{
    PixelInput output;
    float4 pos = float4(input.pos, 1.0f);
    
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;
    output.texcoord = input.texcoord;
    
    return output;
}