struct VSin
{
	float3 position : POSITION;
};

cbuffer lightViewProjection : register(b1)
{
	float4x4 lightViewProj;
};

cbuffer worldMatrix : register(b0)
{
	float4x4 wldMatrix;
};

float4 main(VSin input) : SV_POSITION
{
	float4 outPos;
	outPos = mul(float4(input.position, 1.0f), wldMatrix);
	outPos = mul(outPos, lightViewProj);
	return outPos;
}