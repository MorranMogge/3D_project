Texture2D ambientMap : register(t0);
Texture2D diffuseMap : register(t1);
Texture2D specularMap : register(t2);

SamplerState sampl;

cbuffer materialInfo
{
    float shinyness;
    float3 padding;
};
    
struct PSout
{
    float4 position : SV_Target0;
    float4 ambient : SV_Target1;
    float4 normal : SV_Target2;
    float4 diffuse : SV_Target3;
    float4 specular : SV_Target4;
};


struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPos : WORLDPOSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
};

PSout main(PixelShaderInput input) : SV_TARGET
{
    PSout output;
    output.position = input.worldPos;
    output.normal = input.normal;
    output.ambient = float4(ambientMap.Sample(sampl, input.uv).xyz, 1.0f);
    output.diffuse = float4(diffuseMap.Sample(sampl, input.uv).xyz, 1.0f);
    output.specular = float4(specularMap.Sample(sampl, input.uv).xyz, shinyness); //Here we also save the information of the shinyness
    return output;
}