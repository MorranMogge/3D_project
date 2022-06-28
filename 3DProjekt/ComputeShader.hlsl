//Texture2D<float>    InputTex : register(t0);
Texture2D<float4> inPosition : register(t0);
Texture2D<float4> inColour : register(t1);
Texture2D<float4> inNormal : register(t2);
RWTexture2D<float4> backBuffer : register(u0);

cbuffer cam : register(b0)
{
    float3 cameraPosition; // Not Working!
    float padding;
};

[numthreads(32, 18, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{
    //float grayScaleValue = backBuffer[DTid.xy].x + backBuffer[DTid.xy].y + backBuffer[DTid.xy].z;
    //grayScaleValue /= 3;
    //backBuffer[DTid.xy] = position[DTid.xy];
    //backBuffer[DTid.xy] = float4(grayScaleValue.xxx, backBuffer[DTid.xy].w);
    //backBuffer[DTid.xy] = float4(1.0f, 1.0f, 1.0f, 1.0f) - backBuffer[DTid.xy];
    
    float4 normal = inNormal[DTid.xy];
    float4 colour = inColour[DTid.xy];
    float4 position = inPosition[DTid.xy];
    float3 lightPos = float3(0, 10, 0);
    
    normal = normalize(normal);
    float3 vectorToLight = normalize(lightPos - position.xyz);
    float3 vectorToCam = normalize(cameraPosition - position.xyz);
    float3 reflection = reflect(-vectorToLight, normal.xyz);
    reflection = normalize(reflection);

    float3 ambientClr = 1 * colour;
    float3 diffuseClr = 1 * colour * max(dot(normal.xyz, vectorToLight), 0.0f);
    float3 specularClr = (1 * colour * pow(max(dot(reflection, vectorToCam), 0.0f), 75));

	
    float3 finalColour = (ambientClr + diffuseClr);
	//finalColour += specularClr;

    //float3 number = (input.normal + float3(1.0f, 1.0f, 1.0f)) / 2;
    
    backBuffer[DTid.xy] = float4(finalColour,1.0f);
}