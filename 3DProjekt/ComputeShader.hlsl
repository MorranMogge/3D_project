Texture2D<float4> inPosition : register(t0);
Texture2D<float4> inNormal : register(t1);
Texture2D<float4> inAmbient : register(t2);
Texture2D<float4> inDiffuse : register(t3);
Texture2D<float4> inSpecular : register(t4);
RWTexture2D<float4> backBuffer : register(u0);

cbuffer cam : register(b0)
{
    float3 cameraPosition;
    float padding;
};

cbuffer imGui : register(b1)
{
    float imposition;
    float imnormal;
    float imcolour;
    float imwireframe;
}

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

[numthreads(32, 32, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{   
    int3 location = DTid;
    if (imposition == 1) 
        backBuffer[DTid.xy] = inPosition.Load(location);
    else if (imnormal == 1)
        backBuffer[DTid.xy] = abs(inNormal.Load(location));
    else if (imcolour == 1)
        backBuffer[DTid.xy] = inDiffuse.Load(location);
    else
    {
        float4 position = inPosition.Load(location);
        float4 normal = inNormal.Load(location);
        float shinyness = inSpecular.Load(location).w;
        
        //For each of the spot lights
        float3 vecToLight1 = spotPosition1 - position.xyz;
        float3 vecToLight2 = spotPosition2 - position.xyz;
        float3 vecToLight3 = spotPosition3 - position.xyz;

        float distanceToLight1 = length(vecToLight1);
        float distanceToLight2 = length(vecToLight2);
        float distanceToLight3 = length(vecToLight3); 
        
        float3 diffuse = inDiffuse.Load(location).xyz;
        float3 vectorToCam = normalize(cameraPosition - position.xyz);
        
        float shadowComp0 = position.w;
        float shadowComp1 = normal.w;
        float shadowComp2 = inAmbient.Load(location).w;
        float shadowComp3 = inDiffuse.Load(location).w;

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
                spotLightFactor1 = diffuse * spotColour1 * fallOff;
                
                //Specular part
                reflection = normalize(reflect(-vecToLight1, normal.xyz));
                float3 specComp = fallOff * spotColour1 * inSpecular.Load(location).xyz * pow(max(dot(reflection, vectorToCam), 0.0f), shinyness);

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
                
                //Diffuse part
                spotLightFactor2 = diffuse * spotColour2 * fallOff;
                
                //Specular part
                reflection = normalize(reflect(-vecToLight2, normal.xyz));
                float3 specComp = fallOff * spotColour2 * inSpecular.Load(location).xyz * pow(max(dot(reflection, normalize(vectorToCam)), 0.0f), shinyness);

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
                
                //Diffuse part
                spotLightFactor3 = diffuse * spotColour3 * fallOff;
                
                //Specular part
                reflection = normalize(reflect(-vecToLight3, normal.xyz));
                float3 specComp = fallOff * spotColour3 * inSpecular.Load(location).xyz * pow(max(dot(reflection, vectorToCam), 0.0f), shinyness);

                spotLightFactor3 += specComp;
            }
        }

        float3 lightDirection = -normalize(direction);
        diffuse *= max(dot(normal.xyz, lightDirection), 0.0f) * colour;
        reflection = normalize(reflect(direction, normal.xyz));
        float3 specularClr = colour * inSpecular[DTid.xy].xyz * pow(max(dot(reflection, vectorToCam), 0.0f), shinyness);
        
        diffuse += specularClr;
        float3 ambient = 0.25 * inAmbient[DTid.xy].xyz;
        float3 finalColour = saturate(ambient + diffuse * shadowComp0 + 
                                       spotLightFactor1 * shadowComp1 +
                                       spotLightFactor2 * shadowComp2 +
                                       spotLightFactor3 * shadowComp3);
        
        if (padding1 == 1)
            backBuffer[DTid.xy] = float4(diffuse * shadowComp0, 1.0f);
        else if (spot1Padding1 == 1) 
            backBuffer[DTid.xy] = float4(spotLightFactor1 * shadowComp1, 1.0f);
        else if (spot2Padding1 == 1)
            backBuffer[DTid.xy] = float4(spotLightFactor2 * shadowComp2, 1.0f);
        else if (spot3Padding1 == 1)
            backBuffer[DTid.xy] = float4(spotLightFactor3 * shadowComp3, 1.0f);
        else
            backBuffer[DTid.xy] = float4(finalColour, 1.0f);
    }
}