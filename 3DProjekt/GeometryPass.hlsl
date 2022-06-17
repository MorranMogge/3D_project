Texture2D baseColour : register(t0);
SamplerState sampl;

struct PSOutput 
{ 
	float4 position : SV_Target0; 
	float4 colour : SV_Target1; 
	float4 normal : SV_Target2; 
};

struct PixelShaderInput
{ 
    float4 position : TEST;
	float4 newPos : NEWPOSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

PSOutput main(PixelShaderInput input) : SV_TARGET
{
	PSOutput output;
	output.colour = float4(baseColour.Sample(sampl, input.uv).xyz, 1.0f);
	output.normal = float4(input.normal, 0); 
	output.position = input.position;
	//return float4(0, 0, 0, 0);
	return output;
}
