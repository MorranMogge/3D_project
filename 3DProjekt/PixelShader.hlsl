Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);
Texture2D depthTexture : register(t3);

SamplerState sampl : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer cam : register(b0)
{
	float3 cameraPosition;
	float padding;
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

float4 main(PixelShaderInput input) : SV_TARGET
{
	
    input.lightPos1.xy /= input.lightPos1.w;

    float2 smTexcoord = float2(0.5f * input.lightPos1.x + 0.5f, -0.5f * input.lightPos1.y + 0.5f);
    float depth = input.lightPos1.z / input.lightPos1.w;
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
	
	float3 ambientColour = ambient.Sample(sampl, input.uv).xyz;
	float3 diffuseColour = diffuse.Sample(sampl, input.uv).xyz;
	float3 specularColour = specular.Sample(sampl, input.uv).xyz;
    float3 lightPos = float3(0.0f, 10.0f, 0.0f);
	
	input.normal = normalize(input.normal);
    float3 vectorToLight = normalize(lightPos - input.worldPos.xyz);
    float3 vectorToCam = normalize(cameraPosition - input.worldPos.xyz);
	float3 reflection = reflect(-vectorToLight, input.normal.xyz);
	reflection = normalize(reflection);

	float3 ambientClr = 0.25 * ambientColour;
    float3 diffuseClr = 0.75 * diffuseColour * max(dot(input.normal.xyz, vectorToLight), 0.0f);
    float3 specularClr = (1.0 * specularColour * pow(max(dot(reflection, vectorToCam), 0.0f), 25));

	
	float3 finalColour = (ambientClr + diffuseClr);
	//finalColour += specularClr;

    return float4(shadowco * finalColour, 1.0f);
}