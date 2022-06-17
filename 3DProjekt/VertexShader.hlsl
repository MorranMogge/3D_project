struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct VertexShaderOutput
{
    //float4 position : TEST;
	float4 position : SV_POSITION;
	float4 newPos : NEWPOSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
};

cbuffer constantBuf : register (b0)
{
	float4x4 worldMatrix;
	//float4x4 projectionMatrix;
};

cbuffer Matrices : register(b1)
{
	matrix viewProj;
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	//Byta namn
	float4x4 temp = mul(worldMatrix, viewProj);
	//float4x4 test = mul(worldMatrix, projectionMatrix);
	output.newPos = mul(float4(input.position, 1.0f), worldMatrix);
	//output.position = mul(float4(input.position, 1.0f), test);
	output.position = mul(float4(input.position, 1.0f), temp);
	//Fixa normal
	output.normal = mul(float4(input.normal, 0.0f), worldMatrix);
	output.uv = input.uv;
	return output;
}