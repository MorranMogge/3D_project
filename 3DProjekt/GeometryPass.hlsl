Texture2D ambientMap : register(t0);
Texture2D diffuseMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D depthTexture : register(t3);

SamplerState sampl          : register(s0);
SamplerState shadowSampler  : register(s1);

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
    float4 lightPos : LIGHTPOS;
};

PSout main(PixelShaderInput input) : SV_TARGET
{
    PSout output;
    
    input.lightPos.xy /= input.lightPos.w;

    float2 smTexcoord = float2(0.5f * input.lightPos.x + 0.5f, -0.5f * input.lightPos.y + 0.5f);
    float depth = input.lightPos.z / input.lightPos.w;
    float SHADOW_EPSILON = 0.000125f;
    float dx = 1.f / 1024.0f;
    float dy = 1.f / 1024.0f;
    float d0 = (depthTexture.Sample(shadowSampler, smTexcoord + float2(0.0f, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
    float d1 = (depthTexture.Sample(shadowSampler, smTexcoord + float2(dx, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
    float d2 = (depthTexture.Sample(shadowSampler, smTexcoord + float2(0.0f, dy)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
    float d3 = (depthTexture.Sample(shadowSampler, smTexcoord + float2(dx, dy)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
    
    float shadowco = (d0 + d1 + d2 + d3) / 4;
    if (shadowco <= 0.3) 
        shadowco = 0.3f;
    
    output.position = input.worldPos;
    output.normal = input.normal;
    output.ambient = shadowco * float4(ambientMap.Sample(sampl, input.uv).xyz, 1.0f);
    output.diffuse = shadowco * float4(diffuseMap.Sample(sampl, input.uv).xyz, 1.0f);
    output.specular = shadowco * float4(specularMap.Sample(sampl, input.uv).xyz, shinyness); //Here we also save the information of the shinyness
    return output;
}