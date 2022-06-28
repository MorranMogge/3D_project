Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);

SamplerState sampl;

struct PSout
{
    float4 position : SV_Target0;
    float4 colour : SV_Target1;
    float4 normal : SV_Target2;
};


struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 newPos : NEWPOSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

PSout main(PixelShaderInput input) : SV_TARGET
{
    PSout output;
    output.position = input.newPos;
    output.normal = float4(input.normal, 0.0);
    output.colour = float4(ambient.Sample(sampl, input.uv).xyz,1.0f);
    ///return input.normal;
    return output;
}