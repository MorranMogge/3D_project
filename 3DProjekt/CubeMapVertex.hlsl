cbuffer constantBuf : register(b0)
{
    float4x4 worldMatrix;
};

cbuffer matrices : register(b1)
{
    float4x4 viewProj;
}

struct VertexShaderOutput
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

struct VsIn
{
    float3 normal : NORMAL;
    float3 pos : POSITION;
};

VertexShaderOutput main(VsIn input)
{
    VertexShaderOutput output;
    output.worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
    output.position = mul(mul(float4(input.pos, 1.0f), worldMatrix), viewProj);
    output.normal = mul(float4(input.normal, 0.0f), worldMatrix);
	return output;
}