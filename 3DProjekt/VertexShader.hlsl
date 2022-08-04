struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
    float4 worldPos : WORLDPOSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
    float4 lightPos : LIGHTPOS;
};

cbuffer constantBuf : register (b0)
{
	float4x4 worldMatrix;
};

cbuffer Matrices : register(b1)
{
    float4x4 viewProj;
}

cbuffer lightMtx : register(b2)
{
    float4x4 lightViewProj;
}


VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4x4 temp = mul(worldMatrix, viewProj);
	
    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(float4(input.position, 1.0f), temp);
	output.normal = mul(float4(input.normal, 0.0f), worldMatrix);
    output.lightPos = mul(output.worldPos, lightViewProj);
	output.uv = input.uv;
	return output;
}