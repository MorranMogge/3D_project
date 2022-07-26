Texture2D ambient : register(t0);
Texture2D diffuse : register(t1);
Texture2D specular : register(t2);

SamplerState sampl;

//cbuffer lightBuffer : register (b0)
//{
//	//Light properties
//	float3 ambientLight;
//	float padd1;
//	float3 diffuseLight;
//	float padd2;
//	float3 specularLight;
//	float padd3;
//	float3 lightPos;
//	float padd4;
//	float3 cameraPos;
//	float padd5;
//}

//cbuffer materialInfo : register (b1)
//{
//	//Material properties
//	float ambientMatCoefficient;
//	float diffuseMatCoefficient;
//	float specularMatCoefficient;
//	float shininess;
//}

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
};

float4 main(PixelShaderInput input) : SV_TARGET
{
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

	return float4(finalColour,1.0f);
}