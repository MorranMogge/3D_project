Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);

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
    float4 newPos : NEWPOSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

PSout main(PixelShaderInput input) : SV_TARGET
{
    PSout output;
    output.position = input.newPos;
    output.normal = float4(input.normal, 0.0);
    output.ambient = float4(ambient.Sample(sampl, input.uv).xyz,1.0f);
    output.diffuse = float4(diffuse.Sample(sampl, input.uv).xyz, 1.0f);
    output.specular = float4(specular.Sample(sampl, input.uv).xyz, shinyness); //Here we also save the information of the shinyness
    ///return input.normal;
    return output;
}