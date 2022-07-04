Texture2D<float4> inPosition : register(t0);
Texture2D<float4> inAmbient : register(t1);
Texture2D<float4> inNormal : register(t2);
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
    float impadd;
}

[numthreads(32, 18, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{
    //float grayScaleValue = backBuffer[DTid.xy].x + backBuffer[DTid.xy].y + backBuffer[DTid.xy].z;
    //grayScaleValue /= 3;
    //backBuffer[DTid.xy] = position[DTid.xy];
    //backBuffer[DTid.xy] = float4(grayScaleValue.xxx, backBuffer[DTid.xy].w);
    //backBuffer[DTid.xy] = float4(1.0f, 1.0f, 1.0f, 1.0f) - backBuffer[DTid.xy];
    //if (imposition || imnormal || imcolour)
    //{
    //    if (imposition) backBuffer[DTid.xy] = inPosition[DTid.xy];
    //    else if (imnormal) backBuffer[DTid.xy] = inNormal[DTid.xy];
    //    else backBuffer[DTid.xy] = inColour[DTid.xy];

    //}
    
    if (imposition == 1) 
        backBuffer[DTid.xy] = inPosition[DTid.xy];
    else if (imnormal == 1)
        backBuffer[DTid.xy] = inNormal[DTid.xy];
    else if (imcolour == 1)
        backBuffer[DTid.xy] = inAmbient[DTid.xy];
    else
    {
        float4 normal = inNormal[DTid.xy];
        float4 colour = inAmbient[DTid.xy];
        float4 position = inPosition[DTid.xy];
        float3 lightPos = float3(0, 10, 0);
    
        normal = normalize(normal);
        float3 vectorToLight = normalize(lightPos - position.xyz);
        float3 vectorToCam = normalize(cameraPosition - position.xyz);
        float3 reflection = reflect(-vectorToLight, normal.xyz);
        reflection = normalize(reflection);

        float3 ambientClr = 0.25 * inAmbient[DTid.xy].xyz;
        float3 diffuseClr = 0.25 * inDiffuse[DTid.xy].xyz * max(dot(normal.xyz, vectorToLight), 0.0f);
        float3 specularClr = (1 * inSpecular[DTid.xy].xyz * pow(max(dot(reflection, vectorToCam), 0.0f), 75));

	
        float3 finalColour = (ambientClr + diffuseClr);
	    //finalColour += specularClr;

    //float3 number = (input.normal + float3(1.0f, 1.0f, 1.0f)) / 2;
    
        backBuffer[DTid.xy] = float4(finalColour, 1.0f);
        //backBuffer[DTid.xy] = inNormal[DTid.xy];
    }
}