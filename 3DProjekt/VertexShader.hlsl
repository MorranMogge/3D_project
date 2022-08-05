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

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	
    output.worldPos = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = output.worldPos;
    output.normal = normalize(mul(float4(input.normal, 0.0f), worldMatrix));
    output.lightPos1 = output.lightPos2 = output.lightPos3 = output.lightPos4 = float4(output.worldPos);
	output.uv = input.uv;
	return output;
}