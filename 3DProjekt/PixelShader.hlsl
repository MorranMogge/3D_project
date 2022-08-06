Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);
Texture2DArray depthTexture : register(t3);

SamplerState sampl : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer cam : register(b0)
{
	float3 cameraPosition;
	float padding;
};

cbuffer directionalLight : register(b1)
{
    float3 direction;
    float padding1;
    float3 colour;
    float padding2;
}

cbuffer spotLight1 : register(b2)
{
    float3 spotPosition1;
    float cone1;
    float3 spotDirection1;
    float reach1;
    float3 spotColour1;
    float spot1Padding1;
    float3 spotAttenuation1;
    float spot1Padding2;
}

cbuffer spotLight2 : register(b3)
{
    float3 spotPosition2;
    float cone2;
    float3 spotDirection2;
    float reach2;
    float3 spotColour2;
    float spot2Padding1;
    float3 spotAttenuation2;
    float spot2Padding2;
}

cbuffer spotLight3 : register(b4)
{
    float3 spotPosition3;
    float cone3;
    float3 spotDirection3;
    float reach3;
    float3 spotColour3;
    float spot3Padding1;
    float3 spotAttenuation3;
    float spot3Padding2;
}

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
    input.lightPos2.xy /= input.lightPos2.w;
    input.lightPos3.xy /= input.lightPos3.w;
    input.lightPos4.xy /= input.lightPos4.w;
    
    float2 smTexcoord1 = float2(0.5f * input.lightPos1.x + 0.5f, -0.5f * input.lightPos1.y + 0.5f);
    float2 smTexcoord2 = float2(0.5f * input.lightPos2.x + 0.5f, -0.5f * input.lightPos2.y + 0.5f);
    float2 smTexcoord3 = float2(0.5f * input.lightPos3.x + 0.5f, -0.5f * input.lightPos3.y + 0.5f);
    float2 smTexcoord4 = float2(0.5f * input.lightPos4.x + 0.5f, -0.5f * input.lightPos4.y + 0.5f);
    
    float depth1 = input.lightPos1.z / input.lightPos1.w;
    float depth2 = input.lightPos2.z / input.lightPos2.w;
    float depth3 = input.lightPos3.z / input.lightPos3.w;
    float depth4 = input.lightPos4.z / input.lightPos4.w;
    
    float SHADOW_EPSILON = 0.000125f;
    float d0 = (depthTexture.Sample(shadowSampler, float3(smTexcoord1, 0.0f)).r + SHADOW_EPSILON < depth1) ? 0.0f : 1.0f;
    float d1 = (depthTexture.Sample(shadowSampler, float3(smTexcoord2, 1.0f)).r + SHADOW_EPSILON < depth2) ? 0.0f : 1.0f;
    float d2 = (depthTexture.Sample(shadowSampler, float3(smTexcoord3, 2.0f)).r + SHADOW_EPSILON < depth3) ? 0.0f : 1.0f;
    float d3 = (depthTexture.Sample(shadowSampler, float3(smTexcoord4, 3.0f)).r + SHADOW_EPSILON < depth4) ? 0.0f : 1.0f;
	
	float3 ambientColour = ambient.Sample(sampl, input.uv).xyz;
	float3 diffuseColour = diffuse.Sample(sampl, input.uv).xyz;
	float3 specularColour = specular.Sample(sampl, input.uv).xyz;
	
	input.normal = normalize(input.normal);

    float3 lightDirection = -normalize(direction);
    diffuseColour *= max(dot(input.normal.xyz, lightDirection), 0.0f) * colour;
        
	float3 ambientClr = 0.25 * ambientColour;
    float3 diffuseClr = diffuseColour;
    
    //float3 specularClr = (1.0 * specularColour * pow(max(dot(reflection, vectorToCam), 0.0f), 25));

	
    float3 finalColour = (ambientClr + diffuseClr * d0);
	//finalColour += specularClr;

    return float4(finalColour, 1.0f);
}