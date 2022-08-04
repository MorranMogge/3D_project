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
    float4 lightPos1 : LIGHTPOS1;
    float4 lightPos2 : LIGHTPOS2;
    float4 lightPos3 : LIGHTPOS3;
    float4 lightPos4 : LIGHTPOS4;
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
    float4x4 lightViewProj1;
}

cbuffer lightMtx : register(b3)
{
    float4x4 lightViewProj2;
}

cbuffer lightMtx : register(b4)
{
    float4x4 lightViewProj3;
}

cbuffer lightMtx : register(b5)
{
    float4x4 lightViewProj4;
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4x4 temp = mul(worldMatrix, viewProj);
	
    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(float4(input.position, 1.0f), temp);
	output.normal = mul(float4(input.normal, 0.0f), worldMatrix);
    output.lightPos1 = mul(output.worldPos, lightViewProj1);
    output.lightPos2 = mul(output.worldPos, lightViewProj2);
    output.lightPos3 = mul(output.worldPos, lightViewProj3);
    output.lightPos4 = mul(output.worldPos, lightViewProj4);
	output.uv = input.uv;
	return output;
}