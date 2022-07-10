struct VSin
{
    float3 position : POSITION;
};

struct GSin
{
    float4 position : Position;
};


cbuffer wrld : register(b0)
{
    float4x4 worldMatrix;
};

GSin main(VSin input)
{
    GSin output;
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    return output;
}