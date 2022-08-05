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
    float4 normal : SV_Target1;
    float4 ambient : SV_Target2;
    float4 diffuse : SV_Target3;
    float4 specular : SV_Target4;
};


struct PixelShaderInput
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

PSout main(PixelShaderInput input) : SV_TARGET
{
    PSout output;
    
    float shadowco = 0;
    float2 smTexcoord1 = float2(0.5f * input.lightPos1.x + 0.5f, -0.5f * input.lightPos1.y + 0.5f);
    float2 smTexcoord2 = float2(0.5f * input.lightPos2.x + 0.5f, -0.5f * input.lightPos2.y + 0.5f);
    float2 smTexcoord3 = float2(0.5f * input.lightPos3.x + 0.5f, -0.5f * input.lightPos3.y + 0.5f);
    float2 smTexcoord4 = float2(0.5f * input.lightPos4.x + 0.5f, -0.5f * input.lightPos4.y + 0.5f);
    float depth1 = input.lightPos1.z / input.lightPos1.w;
    float depth2 = input.lightPos2.z / input.lightPos2.w;
    float depth3 = input.lightPos3.z / input.lightPos3.w;
    float depth4 = input.lightPos4.z / input.lightPos4.w;
    
    float SHADOW_EPSILON = 0.000125f;
    float dx = 1.f / 1024.0f;
    float dy = 1.f / 1024.0f;
    float d0 = (depthTexture.Sample(shadowSampler, smTexcoord1).r + SHADOW_EPSILON < depth1) ? 0.0f : 1.0f;
    float d1 = (depthTexture.Sample(shadowSampler, smTexcoord2).r + SHADOW_EPSILON < depth2) ? 0.0f : 1.0f;
    float d2 = (depthTexture.Sample(shadowSampler, smTexcoord3).r + SHADOW_EPSILON < depth3) ? 0.0f : 1.0f;
    float d3 = (depthTexture.Sample(shadowSampler, smTexcoord4).r + SHADOW_EPSILON < depth4) ? 0.0f : 1.0f;
    
    shadowco = (d0 + d1 + d2 + d3) / 4;
    if (shadowco <= 0.3) 
        shadowco = 0.3f;
    
    output.position = float4(input.worldPos.xyz, d0);
    output.normal = float4(input.normal.xyz, d1);
    output.ambient = float4(ambientMap.Sample(sampl, input.uv).xyz, d2);
    output.diffuse = float4(diffuseMap.Sample(sampl, input.uv).xyz, d3);
    output.specular = float4(specularMap.Sample(sampl, input.uv).xyz, shinyness); //Here we also save the information of the shinyness
    return output;
}