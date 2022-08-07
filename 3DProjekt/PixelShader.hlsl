Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);
Texture2DArray depthTexture : register(t3);

SamplerState sampl : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer cam : register(b1)
{
	float3 cameraPosition;
	float padding;
};

cbuffer materialInfo : register(b0)
{
    float shinyness;
    float3 padding123;
};

cbuffer directionalLight : register(b2)
{
    float3 direction;
    float padding1;
    float3 colour;
    float padding2;
}

cbuffer spotLight1 : register(b3)
{
    float3 spotPosition1;
    float cone1;
    float3 spotDirection1;
    float reach1;
    float3 spotColour1;
    float spot1Padding1;
}

cbuffer spotLight2 : register(b4)
{
    float3 spotPosition2;
    float cone2;
    float3 spotDirection2;
    float reach2;
    float3 spotColour2;
    float spot2Padding1;
}

cbuffer spotLight3 : register(b5)
{
    float3 spotPosition3;
    float cone3;
    float3 spotDirection3;
    float reach3;
    float3 spotColour3;
    float spot3Padding1;
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
    
    float2 Texcoord0 = float2(0.5f * input.lightPos1.x + 0.5f, -0.5f * input.lightPos1.y + 0.5f);
    float2 Texcoord1 = float2(0.5f * input.lightPos2.x + 0.5f, -0.5f * input.lightPos2.y + 0.5f);
    float2 Texcoord2 = float2(0.5f * input.lightPos3.x + 0.5f, -0.5f * input.lightPos3.y + 0.5f);
    float2 Texcoord3 = float2(0.5f * input.lightPos4.x + 0.5f, -0.5f * input.lightPos4.y + 0.5f);
    
    float depth0 = input.lightPos1.z / input.lightPos1.w;
    float depth1 = input.lightPos2.z / input.lightPos2.w;
    float depth2 = input.lightPos3.z / input.lightPos3.w;
    float depth3 = input.lightPos4.z / input.lightPos4.w;
    
    float SHADOW_EPSILON = 0.000125f;
    float shadowComp0 = (depthTexture.Sample(shadowSampler, float3(Texcoord0, 0.0f)).r + SHADOW_EPSILON < depth0) ? 0.0f : 1.0f;
    float shadowComp1 = (depthTexture.Sample(shadowSampler, float3(Texcoord1, 1.0f)).r + SHADOW_EPSILON < depth1) ? 0.0f : 1.0f;
    float shadowComp2 = (depthTexture.Sample(shadowSampler, float3(Texcoord2, 2.0f)).r + SHADOW_EPSILON < depth2) ? 0.0f : 1.0f;
    float shadowComp3 = (depthTexture.Sample(shadowSampler, float3(Texcoord3, 3.0f)).r + SHADOW_EPSILON < depth3) ? 0.0f : 1.0f;
    
	float3 ambientColour = ambient.Sample(sampl, input.uv).xyz;
	float3 diffuseColour = diffuse.Sample(sampl, input.uv).xyz;
	float3 specularColour = specular.Sample(sampl, input.uv).xyz;
	
    input.normal = normalize(input.normal);
    
    float3 position = input.worldPos.xyz;
    float3 normal = input.normal.xyz;
    
        
    //For each of the spot lights
    float3 vecToLight1 = spotPosition1 - position.xyz;
    float3 vecToLight2 = spotPosition2 - position.xyz;
    float3 vecToLight3 = spotPosition3 - position.xyz;

    float distanceToLight1 = length(vecToLight1);
    float distanceToLight2 = length(vecToLight2);
    float distanceToLight3 = length(vecToLight3);
        
    float3 vectorToCam = normalize(cameraPosition - position.xyz);

    float3 spotLightFactor1 = float3(0, 0, 0);
    float3 spotLightFactor2 = float3(0, 0, 0);
    float3 spotLightFactor3 = float3(0, 0, 0);
    float3 reflection = float3(0, 0, 0);
        
    float lightAmount;
        
        //If the light should affect the pixel
    if (distanceToLight1 < reach1)
    {
        vecToLight1 /= distanceToLight1;
        lightAmount = dot(vecToLight1, normal.xyz);
        if (lightAmount > 0.0f)
        {
            float3 fallOff = pow(max(dot(-vecToLight1, spotDirection1), 0.0f), cone1);
                
            //Diffuse part
            spotLightFactor1 = diffuseColour * spotColour1 * fallOff;
                
            //Specular part
            reflection = normalize(reflect(-vecToLight1, normal.xyz));
            float3 specComp = fallOff * spotColour1 * specularColour * pow(max(dot(reflection, vectorToCam), 0.0f), shinyness);

            spotLightFactor1 += specComp;
        }
    }

    if (distanceToLight2 < reach2)
    {
        vecToLight2 /= distanceToLight2;
        lightAmount = dot(vecToLight2, normal.xyz);
        if (lightAmount > 0.0f)
        {
            float3 fallOff = pow(max(dot(-vecToLight2, spotDirection2), 0.0f), cone2);
                
            spotLightFactor2 = diffuseColour * spotColour2 * fallOff;
                
            reflection = normalize(reflect(-vecToLight2, normal.xyz));
            float3 specComp = fallOff * spotColour2 * specularColour * pow(max(dot(reflection, normalize(vectorToCam)), 0.0f), shinyness);

            spotLightFactor2 += specComp;
        }
    }

    if (distanceToLight3 < reach3)
    {
        vecToLight3 /= distanceToLight3;
        lightAmount = dot(vecToLight3, normal.xyz);
        if (lightAmount > 0.0f)
        {
            float3 fallOff = pow(max(dot(-vecToLight3, spotDirection3), 0.0f), cone3);
                
            spotLightFactor3 = diffuseColour * spotColour3 * fallOff;
                
            reflection = normalize(reflect(-vecToLight3, normal.xyz));
            float3 specComp = fallOff * spotColour3 * specularColour * pow(max(dot(reflection, vectorToCam), 0.0f), shinyness);

            spotLightFactor3 += specComp;
        }
    }

    float3 lightDirection = -normalize(direction);
    diffuseColour *= max(dot(normal.xyz, lightDirection), 0.0f) * colour;
    reflection = normalize(reflect(direction, normal.xyz));
    float3 specularClr = colour * specularColour * pow(max(dot(reflection, vectorToCam), 0.0f), shinyness);
        
    diffuseColour += specularClr;
    float3 ambient = 0.25 * ambientColour;
    float3 finalColour = saturate(ambient + diffuseColour * shadowComp0 +
                                         spotLightFactor1 * shadowComp1 +
                                         spotLightFactor2 * shadowComp2 +
                                         spotLightFactor3 * shadowComp3);
    
    if (padding1 == 1)
        return float4(diffuseColour * shadowComp0, 1.0f);
    else if (spot1Padding1 == 1) 
        return float4(spotLightFactor1 * shadowComp1, 1.0f);
    else if (spot2Padding1 == 1)
        return float4(spotLightFactor2 * shadowComp2, 1.0f);
    else if (spot3Padding1 == 1)
        return float4(spotLightFactor3 * shadowComp3, 1.0f);
    else
        return float4(finalColour, 1.0f);
}