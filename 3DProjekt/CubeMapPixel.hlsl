TextureCube tex  : TEXTURE :  register(t0);
SamplerState sampl  :   register(s0);

cbuffer camera : register(b0)
{
    float3 camPos;
    float padding;
}

struct VertexShaderOutput
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};


float4 main(VertexShaderOutput input) : SV_TARGET
{
    float3 temp = input.worldPos - camPos;
    
    temp = normalize(temp);
    input.normal = normalize(input.normal);
    float3 loc;
    
    loc = reflect(temp, input.normal);
    loc = normalize(loc);
    
    float3 colour = tex.Sample(sampl, loc).xyz;
    return float4(colour, 1.0f);
}