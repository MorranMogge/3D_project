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
    float3 spotAttenuation1;
    float spot1Padding2;
}

cbuffer spotLight2 : register(b4)
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

cbuffer spotLight3 : register(b5)
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

[numthreads(32, 32, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{   
    if (imposition == 1) 
        backBuffer[DTid.xy] = inPosition[DTid.xy];
    else if (imnormal == 1)
        backBuffer[DTid.xy] = inNormal[DTid.xy];
    else if (imcolour == 1)
        backBuffer[DTid.xy] = inDiffuse[DTid.xy];
    else
    {
        //Fun calculations
        float4 position = inPosition[DTid.xy];
        float4 normal = inNormal[DTid.xy];

        //For each of the spot lights
        float3 vecToLight1 = spotPosition1 - position.xyz;
        float3 vecToLight2 = spotPosition2 - position.xyz;
        float3 vecToLight3 = spotPosition3 - position.xyz;

        float distanceToLight1 = length(vecToLight1);
        float distanceToLight2 = length(vecToLight2);
        float distanceToLight3 = length(vecToLight3);

        float3 spotLightFactor1 = float3(0, 0, 0);
        float3 spotLightFactor2 = float3(0, 0, 0);
        float3 spotLightFactor3 = float3(0, 0, 0);

        float3 diffuse = inDiffuse[DTid.xy].xyz;

        //If the light should affect the pixel
        if (distanceToLight1 <= reach1)
        {
            vecToLight1 /= distanceToLight1;
            float lightAmount = dot(vecToLight1, normal.xyz);
            if (lightAmount > 0)
            {
                spotLightFactor1 = diffuse * spotColour1;
                spotLightFactor1 /= (spotAttenuation1 + (spotAttenuation1 * distanceToLight1) + (spotAttenuation1 * distanceToLight1 * distanceToLight1));
                spotLightFactor1 *= pow(max(dot(-vecToLight1, spotDirection1), 0.0f), cone1);
            }
        }

        if (distanceToLight2 <= reach2)
        {
            vecToLight2 /= distanceToLight2;
            float lightAmount = dot(vecToLight2, normal.xyz);
            if (lightAmount > 0)
            {
                spotLightFactor2 = diffuse * spotColour2;
                spotLightFactor2 /= (spotAttenuation2 + (spotAttenuation2 * distanceToLight2) + (spotAttenuation2 * distanceToLight2 * distanceToLight2));
                spotLightFactor2 *= pow(max(dot(-vecToLight2, spotDirection2), 0.0f), cone2);
            }
        }

        if (distanceToLight3 <= reach3)
        {
            vecToLight3 /= distanceToLight3;
            float lightAmount = dot(vecToLight3, normal.xyz);
            if (lightAmount > 0)
            {
                spotLightFactor3 = diffuse * spotColour3;
                spotLightFactor3 /= (spotAttenuation3 + (spotAttenuation3 * distanceToLight3) + (spotAttenuation3 * distanceToLight3 * distanceToLight3));
                spotLightFactor3 *= pow(max(dot(-vecToLight3, spotDirection3), 0.0f), cone3);
            }
        }

        float3 lightDirection = normalize(-direction);
        diffuse *= max(dot(lightDirection, normal.xyz),0.0f)*colour;
        float3 ambient = 0.25 * inAmbient[DTid.xy].xyz;
        float3 finalColour = saturate(ambient + diffuse + spotLightFactor1 + spotLightFactor2 + spotLightFactor3);
        //backBuffer[DTid.xy] = float4(float3((reach1 - distanceToLight1)/reach1, 0.0, 0.0), 1.0f);
        float shadowco = inPosition[DTid.xy].w + inNormal[DTid.xy].w + inAmbient[DTid.xy].w + inDiffuse[DTid.xy].w;
        if (shadowco <= 0.3)
            shadowco = 0.3f;
        finalColour *= shadowco;
        backBuffer[DTid.xy] = float4(finalColour, 1.0f);

     //   float4 normal = inNormal[DTid.xy];
     //   float4 colour = inAmbient[DTid.xy];
     //   float4 position = inPosition[DTid.xy];
     //   float3 lightPos = float3(0,50,-25);
    
     //   normal = normalize(normal);
     //   float3 vectorToLight = normalize(lightPos - position.xyz);
     //   float3 vectorToCam = normalize(cameraPosition - position.xyz);
     //   float3 reflection = reflect(-vectorToLight, normal.xyz);
     //   reflection = normalize(reflection);

     //   float3 ambientClr = 0.25 * colour;
     //   float3 diffuseClr = 0.75 * inDiffuse[DTid.xy].xyz * max(dot(normal.xyz, vectorToLight), 0.0f);
     //   float3 specularClr = (1 * inSpecular[DTid.xy].xyz * pow(max(dot(reflection, vectorToCam), 0.0f), 25));

	
     //   float3 finalColour = (ambientClr + diffuseClr);
	    ////finalColour += specularClr;
    
     //   backBuffer[DTid.xy] = float4(finalColour, 1.0f);
    }
}